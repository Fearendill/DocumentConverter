// odtconverter.cpp
#include "odtconverter.h"
#include <QFileInfo>

namespace DocumentConverter {

OdtConverter::OdtConverter(QObject *parent)
    : BaseConverter(parent)
{
}

ConversionResult OdtConverter::convert(const QString &inputPath,
                                       const QString &outputPath)
{
    if (!supports(inputPath)) {
        return {false, QString(), QStringLiteral("Format ODT requis"), 0};
    }

    if (!m_libreOfficeAvailable) {
        return {false, QString(),
                QStringLiteral("LibreOffice requis pour ODT"), 0};
    }

    QFileInfo outputInfo(outputPath);
    QString outputDir = outputInfo.absolutePath();

    if (!executeLibreOfficeHeadless(inputPath, outputDir)) {
        return {false, QString(),
                QStringLiteral("Échec conversion LibreOffice"), 0};
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

bool OdtConverter::supports(const QString &filePath) const
{
    return filePath.endsWith(".odt", Qt::CaseInsensitive) ||
           filePath.endsWith(".ods", Qt::CaseInsensitive) ||  // Calc
           filePath.endsWith(".odp", Qt::CaseInsensitive);     // Impress
}

} // namespace DocumentConverter
