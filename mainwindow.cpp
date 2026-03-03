#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "converter/txtconverter.h"
#include "converter/docxconverter.h"
#include "converter/odtconverter.h"
#include "converter/epubconverter.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupConverters();

    connect(ui->btnSelectInput, &QPushButton::clicked,
            this, &MainWindow::onSelectInputFiles);
    connect(ui->btnSelectOutput, &QPushButton::clicked,
            this, &MainWindow::onSelectOutputDir);
    connect(ui->btnConvert, &QPushButton::clicked,
            this, &MainWindow::onConvert);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::onAbout);

    // Configuration initiale
    ui->progressBar->setVisible(false);
    ui->btnConvert->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConverters()
{
    using namespace DocumentConverter;

    auto txtConv = std::make_shared<TxtConverter>(this);
    auto docxConv = std::make_shared<DocxConverter>(this);
    auto odtConv = std::make_shared<OdtConverter>(this);
    auto epubConv = std::make_shared<EpubConverter>(this);

    // Enregistrement par extension
    m_converters["txt"] = txtConv;
    m_converters["text"] = txtConv;
    m_converters["docx"] = docxConv;
    m_converters["doc"] = docxConv;
    m_converters["odt"] = odtConv;
    m_converters["ods"] = odtConv;
    m_converters["odp"] = odtConv;
    m_converters["epub"] = epubConv;
}

void MainWindow::onSelectInputFiles()
{
    QStringList filters;
    filters << "Documents texte (*.txt *.text)"
            << "Documents Word (*.docx *.doc)"
            << "Documents LibreOffice (*.odt *.ods *.odp)"
            << "E-books (*.epub)"
            << "Tous les fichiers supportés (*.txt *.text *.docx *.doc *.odt *.ods *.odp *.epub)"
            << "Tous les fichiers (*)";

    QStringList files = QFileDialog::getOpenFileNames(
        this,
        tr("Sélectionner les fichiers à convertir"),
        QDir::homePath(),
        filters.join(";;")
        );

    if (!files.isEmpty()) {
        m_inputFiles = files;
        ui->listFiles->clear();
        for (const QString &file : files) {
            ui->listFiles->addItem(QFileInfo(file).fileName());
        }
        ui->lblInputCount->setText(tr("%1 fichier(s) sélectionné(s)").arg(files.count()));
        validateInputs();
    }
}

void MainWindow::onSelectOutputDir()
{
    QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Sélectionner le dossier de destination"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
        );

    if (!dir.isEmpty()) {
        m_outputDir = dir;
        ui->lblOutputDir->setText(dir);
        validateInputs();
    }
}

bool MainWindow::validateInputs()
{
    bool valid = !m_inputFiles.isEmpty() && !m_outputDir.isEmpty();
    ui->btnConvert->setEnabled(valid);
    return valid;
}

void MainWindow::onConvert()
{
    if (!validateInputs()) return;

    m_successCount = 0;
    m_failCount = 0;

    QProgressDialog progress(tr("Conversion en cours..."), tr("Annuler"),
                             0, m_inputFiles.count(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    ui->progressBar->setVisible(true);
    ui->progressBar->setMaximum(m_inputFiles.count());
    ui->progressBar->setValue(0);

    for (int i = 0; i < m_inputFiles.count(); ++i) {
        if (progress.wasCanceled()) break;

        progress.setValue(i);
        ui->progressBar->setValue(i);
        progress.setLabelText(tr("Conversion de %1...").arg(
            QFileInfo(m_inputFiles[i]).fileName()));

        processFile(m_inputFiles[i]);
        QApplication::processEvents();
    }

    progress.setValue(m_inputFiles.count());
    ui->progressBar->setVisible(false);

    QString resultMsg = tr("Conversion terminée!\n"
                           "Succès: %1\n"
                           "Échecs: %2").arg(m_successCount).arg(m_failCount);

    QMessageBox::information(this, tr("Résultat"), resultMsg);
}

void MainWindow::processFile(const QString &inputPath)
{
    QFileInfo info(inputPath);
    QString ext = info.suffix().toLower();
    QString outputPath = m_outputDir + "/" + info.completeBaseName() + ".pdf";

    // Éviter l'écrasement
    int counter = 1;
    while (QFile::exists(outputPath)) {
        outputPath = m_outputDir + "/" + info.completeBaseName() +
                     QString("_%1.pdf").arg(counter++);
    }

    auto converter = m_converters.value(ext);
    if (!converter) {
        updateStatus(tr("Format non supporté: %1").arg(ext));
        m_failCount++;
        return;
    }

    auto result = converter->convert(inputPath, outputPath);

    if (result.success) {
        m_successCount++;
        updateStatus(tr("✓ Converti: %1").arg(info.fileName()));
    } else {
        m_failCount++;
        updateStatus(tr("✗ Échec: %1 - %2").arg(info.fileName(), result.errorMessage));
    }
}

void MainWindow::updateStatus(const QString &message)
{
    ui->statusbar->showMessage(message, 5000);
    ui->logTextEdit->append(message);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("À propos"),
                       tr("<h2>Document Converter</h2>"
                          "<p>Version 1.0</p>"
                          "<p>Convertisseur de documents vers PDF utilisant Qt 6.10 "
                          "et LibreOffice.</p>"
                          "<p>Formats supportés: TXT, DOCX, DOC, ODT, ODS, ODP</p>"));
}
