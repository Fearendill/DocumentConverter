#include "epubconverter.h"
#include <QProcess>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTextDocument>
#include <QStringDecoder>
#include <QPrinter>
#include <QPageSize>
#include <QDebug>
#include <QRegularExpression>

namespace DocumentConverter {

EpubConverter::EpubConverter(QObject *parent)
    : BaseConverter(parent)
{
    m_calibreAvailable = checkCalibreAvailable();
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

    // Fallback : Qt natif (extraction + QPrinter)
    return convertWithQt(inputPath, outputPath);
}

ConversionResult EpubConverter::convertWithCalibre(const QString &inputPath,
                                                   const QString &outputPath)
{
    QProcess process;
    QStringList arguments;

    // ebook-convert input.epub output.pdf
    arguments << inputPath
              << outputPath
              << "--pdf-page-numbers"
              << "--pdf-default-font-size" << "12"
              << "--paper-size" << "a4"
              << "--preserve-cover-aspect-ratio";

    process.start(m_calibrePath, arguments);

    if (!process.waitForFinished(300000)) { // 5 min timeout
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
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        return {false, QString(),
                QStringLiteral("Impossible de créer le dossier temporaire"), 0};
    }

    // Étape 1 : Extraire l'EPUB
    if (!extractEpub(inputPath, tempDir.path())) {
        return {false, QString(),
                QStringLiteral("Extraction EPUB échouée (unzip/tar non disponible ?)"), 0};
    }

    // Étape 2 : Trouver et lire les fichiers HTML
    QStringList htmlFiles = findHtmlFiles(tempDir.path());
    if (htmlFiles.isEmpty()) {
        return {false, QString(),
                QStringLiteral("Aucun contenu HTML trouvé dans l'EPUB"), 0};
    }

    // Étape 3 : Concaténer le contenu HTML
    QString htmlContent;
    htmlContent.reserve(100000); // Réserver espace pour perf

    for (const QString &filePath : htmlFiles) {
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            continue;
        }

        QByteArray data = file.readAll();
        file.close();

        // Détecter l'encodage (simplifié : UTF-8 par défaut, fallback Latin-1)
        QString text;
        auto toUtf16 = QStringDecoder(QStringDecoder::Utf8);
        text = toUtf16(data);
        if (toUtf16.hasError()) {
            text = QString::fromLatin1(data);
        }

        htmlContent += text + "\n<br><hr><br>\n"; // Séparateur entre chapitres
    }

    if (htmlContent.isEmpty()) {
        return {false, QString(),
                QStringLiteral("Impossible de lire le contenu HTML"), 0};
    }

    // Étape 4 : Nettoyer le HTML basique pour QTextDocument
    // Supprimer les scripts et styles complexes qui bloquent QTextDocument
    htmlContent.remove(QRegularExpression("<script[^>]*>.*?</script>",
                                          QRegularExpression::DotMatchesEverythingOption));
    htmlContent.remove(QRegularExpression("<style[^>]*>.*?</style>",
                                          QRegularExpression::DotMatchesEverythingOption));

    // Étape 5 : Conversion en PDF
    QTextDocument doc;
    doc.setHtml(htmlContent);

    // Configuration page A4 avec marges
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

    doc.print(&printer);

    // Estimation page count
    int pageCount = doc.pageCount();

    return {true, outputPath, QString(), pageCount};
}

bool EpubConverter::extractEpub(const QString &epubPath, const QString &destDir)
{
    // Essayer unzip d'abord (plus commun)
    QProcess unzip;
    unzip.start("unzip", QStringList() << "-q" << "-o"
                << "-d" << destDir << epubPath);

    if (unzip.waitForStarted(5000) && unzip.waitForFinished(30000)) {
        return unzip.exitCode() == 0;
    }

    unzip.kill();

    // Fallback : essayer tar (certains Linux sans unzip)
    QProcess tar;
    tar.start("tar", QStringList() << "-xf" << epubPath << "-C" << destDir);

    if (tar.waitForStarted(5000) && tar.waitForFinished(30000)) {
        return tar.exitCode() == 0;
    }

    tar.kill();

    // Dernier fallback : 7z (Windows souvent)
#ifdef Q_OS_WIN
    QProcess sevenZip;
    sevenZip.start("7z", QStringList() << "x" << "-y"
                   << "-o" + destDir << epubPath);

    if (sevenZip.waitForStarted(5000) && sevenZip.waitForFinished(30000)) {
        return sevenZip.exitCode() == 0;
    }
#endif

    return false;
}

QStringList EpubConverter::findHtmlFiles(const QString &dir)
{
    QStringList htmlFiles;

    // Chercher récursivement tous les fichiers HTML
    QStringList filters;
    filters << "*.html" << "*.xhtml" << "*.htm";

    QDirIterator it(dir, filters, QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo info(filePath);
        QString fileName = info.fileName().toLower();

        // Ignorer les fichiers de navigation/toc généralement
        if (fileName.contains("toc") || fileName.contains("nav") ||
            fileName.contains("index")) {
            // Les garder quand même mais en dernier
            htmlFiles.append(filePath);
        } else {
            // Insérer au début (contenu principal)
            htmlFiles.prepend(filePath);
        }
    }

    // Trier pour garder l'ordre logique si numérotés
    std::sort(htmlFiles.begin(), htmlFiles.end(),
              [](const QString &a, const QString &b) {
                  return QFileInfo(a).fileName() < QFileInfo(b).fileName();
              });

    return htmlFiles;
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
                  << QDir::homePath() + "/AppData/Local/Programs/Calibre/ebook-convert.exe"
                  << "ebook-convert.exe"; // Dans PATH
#elif defined(Q_OS_MAC)
    possiblePaths << "/Applications/calibre.app/Contents/MacOS/ebook-convert"
                  << "/usr/local/bin/ebook-convert";
#else // Linux
    possiblePaths << "/usr/bin/ebook-convert"
                  << "/usr/local/bin/ebook-convert"
                  << QDir::homePath() + "/.local/bin/ebook-convert";
#endif

    // Vérifier PATH d'abord
    QProcess p;
#ifdef Q_OS_WIN
    p.start("where", QStringList() << "ebook-convert");
#else
    p.start("which", QStringList() << "ebook-convert");
#endif

    if (p.waitForFinished(3000)) {
        QString output = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
        if (!output.isEmpty()) {
            QString path = output.split("\n").first().trimmed();
            if (QFile::exists(path)) return path;
        }
    }

    // Vérifier chemins spécifiques
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) return path;
    }

    return QString();
}

} // namespace DocumentConverter
