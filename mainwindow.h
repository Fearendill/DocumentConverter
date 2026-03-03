#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace DocumentConverter {
class IConverter;
}

class QProgressDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSelectInputFiles();
    void onSelectOutputDir();
    void onConvert();
    void onAbout();

private:
    void setupConverters();
    void updateStatus(const QString &message);
    bool validateInputs();
    void processFile(const QString &inputPath);

    Ui::MainWindow *ui;
    QMap<QString, std::shared_ptr<DocumentConverter::IConverter>> m_converters;
    QStringList m_inputFiles;
    QString m_outputDir;
    int m_successCount = 0;
    int m_failCount = 0;
};

#endif // MAINWINDOW_H
