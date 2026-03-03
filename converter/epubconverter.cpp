#include "epubconverter.h"
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QXmlStreamReader>
#include <QZipReader>
#include <QTextDocument>
#include <QPrinter>
#include <QDebug>

namespace DocumentConverter {

EpubConverter::EpubConverter(QObject *parent)
    : BaseConverter(parent)
{
    m_calibreAvailable = checkCalibreAvailable();
    m_libreOfficeAvailable = checkLibreOfficeAvailable();
}

ConversionResult EpubConverter::convert(const QString &inputPath,
                                        const QString &outputPath)
{
    if (!supports(inputPath)) {
        return {false, QString(), QStringLiteral("Format EPUB requis"), 0};
    }

    // Priorité 1 : Calibre (meilleure qualité)
    if (m_calibreAvailable) {
        return convertWithCalibre(inputPath, outputPath);
    }
    
    // Priorité 2 : LibreOffice
    if (m_libreOfficeAvailable) {
        return convertWithLibreOffice(inputPath, outputPath);
    }
    
    // Fallback : Qt natif (basique)
    return convertWithQt(inputPath, outputPath);
}

ConversionResult EpubConverter::convertWithCalibre(const QString &inputPath,
                                                   const QString &outputPath)
{
    QProcess process;
    QStringList arguments;
    
    // ebook-convert input.epub output.pdf [options]
    arguments << inputPath 
              << outputPath
              << "--pdf-page-numbers"  // Numéros de page
              << "--pdf-default-font-size" << "12"
              << "--paper-size" << "a4";
    
    // Option : utiliser la couverture si présente
    arguments << "--preserve-cover-aspect-ratio";
    
    process.start(m_calibrePath, arguments);
    
    if (!process.waitForFinished(300000)) { // 5 min timeout pour les gros livres
        process.kill();
        return {false, QString(), 
                QStringLiteral("Timeout Calibre (>5min)"), 0};
    }
    
    if (process.exitCode() != 0) {
        QString error = QString::fromUtf8(process.readAllStandardError());
        return {false, QString(), 
                QStringLiteral("Erreur Calibre: %1").arg(error), 0};
    }
    
    return {true, outputPath, QString(), 0};
}

ConversionResult EpubConverter::convertWithLibreOffice(const QString &inputPath,
                                                       const QString &outputPath)
{
    QFileInfo outputInfo(outputPath);
    QString outputDir = outputInfo.absolutePath();
    
    if (!executeLibreOfficeHeadless(inputPath, outputDir)) {
        return {false, QString(), 
                QStringLiteral("Échec conversion LibreOffice EPUB"), 0};
    }
    
    QFileInfo inputInfo(inputPath);
    QString expectedOutput = outputDir + "/" + inputInfo.completeBaseName() + ".pdf";
    
    if (!QFile::exists(expectedOutput)) {
        return {false, QString(), 
                QStringLiteral("Fichier PDF non généré"), 0};
    }
    
    if (expectedOutput != outputPath) {
        QFile::remove(outputPath);
        QFile::rename(expectedOutput, outputPath);
    }
    
    return {true, outputPath, QString(), 0};
}

ConversionResult EpubConverter::convertWithQt(const QString &inputPath,
                                              const QString &outputPath)
{
    // Extraction basique du contenu HTML depuis l'EPUB
    QZipReader zip(inputPath);
    if (!zip.exists()) {
        return {false, QString(), 
                QStringLiteral("EPUB invalide (zip corrompu)"), 0};
    }
    
    // Chercher le fichier content.opf ou *.html
    QString htmlContent;
    bool foundContent = false;
    
    for (const QZipReader::FileInfo &file : zip.fileInfoList()) {
        if (file.filePath.endsWith(".html") || 
            file.filePath.endsWith(".xhtml") ||
            file.filePath.endsWith(".htm")) {
            QByteArray data = zip.fileData(file.filePath);
            htmlContent += QString::fromUtf8(data);
            foundContent = true;
        }
    }
    
    zip.close();
    
    if (!foundContent) {
        return {false, QString(), 
                QStringLiteral("Aucun contenu HTML trouvé dans l'EPUB"), 0};
    }
    
    // Conversion HTML -> PDF via QTextDocument
    QTextDocument doc;
    doc.setHtml(htmlContent);
    
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageSize(QPageSize::A4);
    
    doc.print(&printer);
    
    return {true, outputPath, QString(), doc.pageCount()};
}

bool EpubConverter::supports(const QString &filePath) const
{
    return filePath.endsWith(".epub", Qt::CaseInsensitive);
}

bool EpubConverter::checkCalibreAvailable()
{
    m_calibrePath = findCalibrePath();
    if (m_calibrePath.isEmpty()) return false;
    
    QProcess p;
    p.start(m_calibrePath, QStringList() << "--version");
    return p.waitForFinished(5000) && p.exitCode() == 0;
}

QString EpubConverter::findCalibrePath()
{
    QStringList possiblePaths;
    
#ifdef Q_OS_WIN
    possiblePaths << "C:/Program Files/Calibre2/ebook-convert.exe"
                  << "C:/Program Files (x86)/Calibre2/ebook-convert.exe"
                  << QDir::homePath() + "/AppData/Local/Programs/Calibre/ebook-convert.exe";
#elif defined(Q_OS_MAC)
    possiblePaths << "/Applications/calibre.app/Contents/MacOS/ebook-convert"
                  << "/usr/local/bin/ebook-convert";
#else // Linux
    possiblePaths << "/usr/bin/ebook-convert"
                  << "/usr/local/bin/ebook-convert"
                  << QDir::homePath() + "/.local/bin/ebook-convert";
#endif

    // Vérifier PATH
    QProcess p;
    p.start("which", QStringList() << "ebook-convert");
    if (p.waitForFinished(3000)) {
        QString path = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        if (!path.isEmpty()) return path;
    }
    
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) return path;
    }
    
    return QString();
}

bool EpubConverter::extractEpubMetadata(const QString &epubPath,
                                        QString &title,
                                        QString &author)
{
    QZipReader zip(epubPath);
    if (!zip.exists()) return false;
    
    QByteArray container = zip.fileData("META-INF/container.xml");
    if (container.isEmpty()) {
        zip.close();
        return false;
    }
    
    // Parser container.xml pour trouver content.opf
    QString opfPath;
    QXmlStreamReader xml(container);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QStringLiteral("rootfile")) {
            opfPath = xml.attributes().value("full-path").toString();
            break;
        }
    }
    
    if (opfPath.isEmpty()) {
        zip.close();
        return false;
    }
    
    // Parser content.opf
    QByteArray opfData = zip.fileData(opfPath);
    zip.close();
    
    if (opfData.isEmpty()) return false;
    
    QXmlStreamReader opfXml(opfData);
    while (!opfXml.atEnd()) {
        opfXml.readNext();
        if (opfXml.isStartElement()) {
            if (opfXml.name() == QStringLiteral("title")) {
                title = opfXml.readElementText();
            } else if (opfXml.name() == QStringLiteral("creator")) {
                author = opfXml.readElementText();
            }
        }
    }
    
    return !title.isEmpty();
}

} // namespace DocumentConverter