#ifndef CONVERTER_H
#define CONVERTER_H

#include <QString>
#include <QObject>

namespace DocumentConverter {

enum class Format {
    TXT,
    DOCX,
    ODT,
    RTF,
    HTML,
    PDF
};

struct ConversionResult {
    bool success;
    QString outputPath;
    QString errorMessage;
    int pageCount;
};

class IConverter {
public:
    virtual ~IConverter() = default;
    virtual ConversionResult convert(const QString &inputPath,
                                     const QString &outputPath) = 0;
    virtual bool supports(const QString &filePath) const = 0;
    virtual Format sourceFormat() const = 0;
};

} // namespace DocumentConverter

#endif // CONVERTER_H
