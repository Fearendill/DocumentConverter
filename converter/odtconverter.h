// odtconverter.h
#ifndef ODTCONVERTER_H
#define ODTCONVERTER_H

#include "baseconverter.h"

namespace DocumentConverter {

class OdtConverter : public BaseConverter {
public:
    explicit OdtConverter(QObject *parent = nullptr);

    ConversionResult convert(const QString &inputPath,
                             const QString &outputPath) override;
    bool supports(const QString &filePath) const override;
    Format sourceFormat() const override { return Format::ODT; }
};

} // namespace DocumentConverter

#endif // ODTCONVERTER_H
