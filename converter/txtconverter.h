// txtconverter.h
#ifndef TXTCONVERTER_H
#define TXTCONVERTER_H

#include "baseconverter.h"

namespace DocumentConverter {

class TxtConverter : public BaseConverter {
public:
    explicit TxtConverter(QObject *parent = nullptr);

    ConversionResult convert(const QString &inputPath,
                             const QString &outputPath) override;
    bool supports(const QString &filePath) const override;
    Format sourceFormat() const override { return Format::TXT; }

private:
    ConversionResult convertWithQt(const QString &inputPath,
                                   const QString &outputPath);
    ConversionResult convertWithLibreOffice(const QString &inputPath,
                                            const QString &outputPath);
};

} // namespace DocumentConverter

#endif // TXTCONVERTER_H
