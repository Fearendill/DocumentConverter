#ifndef BASECONVERTER_H
#define BASECONVERTER_H

#include "converter.h"
#include <QProcess>
#include <QTemporaryDir>

namespace DocumentConverter {

class BaseConverter : public QObject, public IConverter {
    Q_OBJECT
public:
    explicit BaseConverter(QObject *parent = nullptr);
    virtual ~BaseConverter();

protected:
    bool executeLibreOffice(const QString &inputPath,
                            const QString &outputDir,
                            const QString &format = "pdf");
    bool executeLibreOfficeHeadless(const QString &inputPath,
                                    const QString &outputDir);
    QString getLibreOfficePath() const { return m_libreOfficePath; }
    bool checkLibreOfficeAvailable();
    QString findLibreOfficePath();

    QTemporaryDir m_tempDir;
    bool m_libreOfficeAvailable = false;
    QString m_libreOfficePath;
};

} // namespace DocumentConverter

#endif // BASECONVERTER_H
