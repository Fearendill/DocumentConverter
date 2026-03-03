// docxconverter.h
#ifndef DOCXCONVERTER_H
#define DOCXCONVERTER_H

#include "baseconverter.h"

namespace DocumentConverter {

class DocxConverter : public BaseConverter {
public:
    explicit DocxConverter(QObject *parent = nullptr);

    ConversionResult convert(const QString &inputPath,
                             const QString &outputPath) override;
    bool supports(const QString &filePath) const override;
    Format sourceFormat() const override { return Format::DOCX; }

private:
    ConversionResult convertWithLibreOffice(const QString &inputPath,
                                            const QString &outputPath);
    ConversionResult extractAndConvert(const QString &inputPath,
                                       const QString &outputPath);
};

} // namespace DocumentConverter

#endif // DOCXCONVERTER_H
