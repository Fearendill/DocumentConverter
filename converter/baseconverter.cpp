// baseconverter.cpp
#include "baseconverter.h"
#include <QStandardPaths>
#include <QFileInfo>
#include <QDebug>
#include <QDir>

namespace DocumentConverter {

BaseConverter::BaseConverter(QObject *parent)
    : QObject(parent)
{
    m_libreOfficePath = findLibreOfficePath();
    m_libreOfficeAvailable = !m_libreOfficePath.isEmpty();

    if (m_libreOfficeAvailable) {
        qDebug() << "LibreOffice trouvé:" << m_libreOfficePath;
    } else {
        qDebug() << "LibreOffice NON trouvé - Vérifiez l'installation";
    }
}

BaseConverter::~BaseConverter() = default;

QString BaseConverter::findLibreOfficePath()
{
    QStringList possiblePaths;

#ifdef Q_OS_WIN
    possiblePaths << "C:/Program Files/LibreOffice/program/soffice.exe"
                  << "C:/Program Files (x86)/LibreOffice/program/soffice.exe"
                  << "C:/Programmes/LibreOffice/program/soffice.exe";
#elif defined(Q_OS_MAC)
    possiblePaths << "/Applications/LibreOffice.app/Contents/MacOS/soffice"
                  << "/usr/local/bin/soffice"
                  << "/usr/bin/soffice";
#else // Linux
    possiblePaths << "/usr/bin/libreoffice"
                  << "/usr/bin/soffice"
                  << "/usr/lib/libreoffice/program/soffice"
                  << "/snap/bin/libreoffice";
#endif

    // Vérifier d'abord dans le PATH
    QProcess checkProcess;
    checkProcess.start("which", QStringList() << "libreoffice");
    if (checkProcess.waitForFinished(3000)) {
        QString path = QString::fromUtf8(checkProcess.readAllStandardOutput()).trimmed();
        if (!path.isEmpty() && QFile::exists(path)) {
            return path;
        }
    }

    // Vérifier les chemins spécifiques
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    // Recherche via where/which fallback
    checkProcess.start("where", QStringList() << "soffice");
    if (checkProcess.waitForFinished(3000)) {
        QString output = QString::fromUtf8(checkProcess.readAllStandardOutput()).trimmed();
        if (!output.isEmpty() && QFile::exists(output.split("\n").first())) {
            return output.split("\n").first();
        }
    }

    return QString();
}

bool BaseConverter::executeLibreOffice(const QString &inputPath,
                                      const QString &outputDir,
                                      const QString &format)
{
    if (m_libreOfficePath.isEmpty()) {
        return false;
    }

    QProcess process;
    QStringList arguments;
    arguments << "--headless"
              << "--convert-to" << format
              << "--outdir" << QDir::toNativeSeparators(outputDir)
              << QDir::toNativeSeparators(inputPath);

    qDebug() << "Exécution:" << m_libreOfficePath << arguments;

    process.start(m_libreOfficePath, arguments);

    if (!process.waitForFinished(180000)) { // 3 minutes timeout
        process.kill();
        qDebug() << "Timeout LibreOffice";
        return false;
    }

    QString errorOutput = QString::fromUtf8(process.readAllStandardError());
    QString stdOutput = QString::fromUtf8(process.readAllStandardOutput());

    if (!errorOutput.isEmpty()) {
        qDebug() << "LibreOffice stderr:" << errorOutput;
    }
    if (!stdOutput.isEmpty()) {
        qDebug() << "LibreOffice stdout:" << stdOutput;
    }

    return process.exitCode() == 0;
}

bool BaseConverter::executeLibreOfficeHeadless(const QString &inputPath,
                                               const QString &outputDir)
{
    return executeLibreOffice(inputPath, outputDir, "pdf");
}

bool BaseConverter::checkLibreOfficeAvailable()
{
    return m_libreOfficeAvailable;
}

} // namespace DocumentConverter
