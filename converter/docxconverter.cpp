// docxconverter.cpp
#include "docxconverter.h"
#include <QFileInfo>
#include <QProcess>
#include <QTemporaryDir>
#include <QDir>
#include <QDebug>

namespace DocumentConverter {

DocxConverter::DocxConverter(QObject *parent)
    : BaseConverter(parent)
{
}

ConversionResult DocxConverter::convert(const QString &inputPath,
                                        const QString &outputPath)
{
    if (!supports(inputPath)) {
        return {false, QString(), QStringLiteral("Format DOCX requis"), 0};
    }

    if (!m_libreOfficeAvailable) {
        return {false, QString(),
                QStringLiteral("LibreOffice requis pour DOCX"), 0};
    }

    return convertWithLibreOffice(inputPath, outputPath);
}

ConversionResult DocxConverter::convertWithLibreOffice(const QString &inputPath,
                                                       const QString &outputPath)
{
    QFileInfo outputInfo(outputPath);
    QString outputDir = outputInfo.absolutePath();

    if (!executeLibreOfficeHeadless(inputPath, outputDir)) {
        return {false, QString(),
                QStringLiteral("Échec conversion LibreOffice"), 0};
    }

    // LibreOffice génère: input.pdf dans outputDir
    QFileInfo inputInfo(inputPath);
    QString expectedOutput = outputDir + "/" + inputInfo.completeBaseName() + ".pdf";

    if (!QFile::exists(expectedOutput)) {
        return {false, QString(),
                QStringLiteral("Fichier PDF non généré"), 0};
    }

    // Renommage si nécessaire
    if (expectedOutput != outputPath) {
        QFile::remove(outputPath);
        if (!QFile::rename(expectedOutput, outputPath)) {
            return {false, QString(),
                    QStringLiteral("Erreur renommage fichier"), 0};
        }
    }

    return {true, outputPath, QString(), 0};
}

bool DocxConverter::supports(const QString &filePath) const
{
    return filePath.endsWith(".docx", Qt::CaseInsensitive) ||
           filePath.endsWith(".doc", Qt::CaseInsensitive);
}

} // namespace DocumentConverter
