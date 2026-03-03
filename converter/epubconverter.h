#ifndef EPUBCONVERTER_H
#define EPUBCONVERTER_H

#include "baseconverter.h"
#include <QTemporaryDir>
#include <QStringDecoder>

namespace DocumentConverter {

class EpubConverter : public BaseConverter {
public:
    explicit EpubConverter(QObject *parent = nullptr);

    ConversionResult convert(const QString &inputPath,
                             const QString &outputPath) override;
    bool supports(const QString &filePath) const override;
    Format sourceFormat() const override { return Format::EPUB; }

private:
    ConversionResult convertWithCalibre(const QString &inputPath,
                                        const QString &outputPath);
    ConversionResult convertWithLibreOffice(const QString &inputPath,
                                            const QString &outputPath);
    ConversionResult convertWithQt(const QString &inputPath,
                                   const QString &outputPath);

    bool extractEpub(const QString &epubPath, const QString &destDir);
    QStringList findHtmlFiles(const QString &dir);
    bool checkCalibreAvailable();
    QString findCalibrePath();

    QString m_calibrePath;
    bool m_calibreAvailable = false;
};

} // namespace DocumentConverter

#endif // EPUBCONVERTER_H
