// txtconverter.cpp
#include "txtconverter.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QTextCursor>
#include <QPrinter>
#include <QPageSize>
#include <QFileInfo>

namespace DocumentConverter {

TxtConverter::TxtConverter(QObject *parent)
    : BaseConverter(parent)
{
}

ConversionResult TxtConverter::convert(const QString &inputPath,
                                       const QString &outputPath)
{
    if (!supports(inputPath)) {
        return {false, QString(), QStringLiteral("Format non supporté"), 0};
    }

    // Priorité à LibreOffice si disponible pour meilleure mise en page
    if (m_libreOfficeAvailable) {
        return convertWithLibreOffice(inputPath, outputPath);
    }

    return convertWithQt(inputPath, outputPath);
}

ConversionResult TxtConverter::convertWithQt(const QString &inputPath,
                                           const QString &outputPath)
{
    QFile file(inputPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {false, QString(),
                QStringLiteral("Impossible d'ouvrir le fichier"), 0};
    }

    QTextStream stream(&file);
    stream.setAutoDetectUnicode(true);

    QString content = stream.readAll();
    file.close();

    // Création du document avec mise en page
    QTextDocument document;
    document.setPlainText(content);

    // Configuration de la page A4
    QPageSize pageSize(QPageSize::A4);

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath);
    printer.setPageSize(pageSize);
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);

    document.print(&printer);

    // Calcul approximatif du nombre de pages
    int pageCount = document.pageCount();

    return {true, outputPath, QString(), pageCount};
}

ConversionResult TxtConverter::convertWithLibreOffice(const QString &inputPath,
                                                       const QString &outputPath)
{
    QFileInfo outputInfo(outputPath);
    QString outputDir = outputInfo.absolutePath();
    QString baseName = outputInfo.completeBaseName();

    if (!executeLibreOfficeHeadless(inputPath, outputDir)) {
        // Fallback sur Qt si LibreOffice échoue
        return convertWithQt(inputPath, outputPath);
    }

    // Renommage du fichier généré
    QString generatedPath = outputDir + "/" + baseName + ".pdf";
    if (generatedPath != outputPath && QFile::exists(generatedPath)) {
        QFile::remove(outputPath);
        QFile::rename(generatedPath, outputPath);
    }

    return {true, outputPath, QString(), 0}; // Page count unknown via LO
}

bool TxtConverter::supports(const QString &filePath) const
{
    return filePath.endsWith(".txt", Qt::CaseInsensitive) ||
           filePath.endsWith(".text", Qt::CaseInsensitive);
}

} // namespace DocumentConverter
