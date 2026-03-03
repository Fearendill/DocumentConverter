/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupInput;
    QHBoxLayout *horizontalLayout;
    QListWidget *listFiles;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnSelectInput;
    QLabel *lblInputCount;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupOutput;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lblOutputDir;
    QPushButton *btnSelectOutput;
    QProgressBar *progressBar;
    QPushButton *btnConvert;
    QGroupBox *groupLog;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *logTextEdit;
    QMenuBar *menubar;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName("actionAbout");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        groupInput = new QGroupBox(centralwidget);
        groupInput->setObjectName("groupInput");
        horizontalLayout = new QHBoxLayout(groupInput);
        horizontalLayout->setObjectName("horizontalLayout");
        listFiles = new QListWidget(groupInput);
        listFiles->setObjectName("listFiles");
        listFiles->setMaximumSize(QSize(16777215, 150));

        horizontalLayout->addWidget(listFiles);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        btnSelectInput = new QPushButton(groupInput);
        btnSelectInput->setObjectName("btnSelectInput");

        verticalLayout_2->addWidget(btnSelectInput);

        lblInputCount = new QLabel(groupInput);
        lblInputCount->setObjectName("lblInputCount");

        verticalLayout_2->addWidget(lblInputCount);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);


        verticalLayout->addWidget(groupInput);

        groupOutput = new QGroupBox(centralwidget);
        groupOutput->setObjectName("groupOutput");
        horizontalLayout_2 = new QHBoxLayout(groupOutput);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        lblOutputDir = new QLabel(groupOutput);
        lblOutputDir->setObjectName("lblOutputDir");
        lblOutputDir->setStyleSheet(QString::fromUtf8("color: gray;"));

        horizontalLayout_2->addWidget(lblOutputDir);

        btnSelectOutput = new QPushButton(groupOutput);
        btnSelectOutput->setObjectName("btnSelectOutput");

        horizontalLayout_2->addWidget(btnSelectOutput);


        verticalLayout->addWidget(groupOutput);

        progressBar = new QProgressBar(centralwidget);
        progressBar->setObjectName("progressBar");
        progressBar->setVisible(false);

        verticalLayout->addWidget(progressBar);

        btnConvert = new QPushButton(centralwidget);
        btnConvert->setObjectName("btnConvert");
        btnConvert->setEnabled(false);
        btnConvert->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"           background-color: #4CAF50;\n"
"           color: white;\n"
"           font-weight: bold;\n"
"           padding: 10px;\n"
"           font-size: 14px;\n"
"       }\n"
"       QPushButton:disabled {\n"
"           background-color: #cccccc;\n"
"       }"));

        verticalLayout->addWidget(btnConvert);

        groupLog = new QGroupBox(centralwidget);
        groupLog->setObjectName("groupLog");
        verticalLayout_3 = new QVBoxLayout(groupLog);
        verticalLayout_3->setObjectName("verticalLayout_3");
        logTextEdit = new QTextEdit(groupLog);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setReadOnly(true);

        verticalLayout_3->addWidget(logTextEdit);


        verticalLayout->addWidget(groupLog);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName("menuHelp");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuHelp->menuAction());
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Document Converter", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "\303\200 propos", nullptr));
        groupInput->setTitle(QCoreApplication::translate("MainWindow", "Fichiers source", nullptr));
        btnSelectInput->setText(QCoreApplication::translate("MainWindow", "Ajouter fichiers...", nullptr));
        lblInputCount->setText(QCoreApplication::translate("MainWindow", "0 fichier(s)", nullptr));
        groupOutput->setTitle(QCoreApplication::translate("MainWindow", "Dossier de destination", nullptr));
        lblOutputDir->setText(QCoreApplication::translate("MainWindow", "Non s\303\251lectionn\303\251", nullptr));
        btnSelectOutput->setText(QCoreApplication::translate("MainWindow", "Choisir...", nullptr));
        btnConvert->setText(QCoreApplication::translate("MainWindow", "Convertir en PDF", nullptr));
        groupLog->setTitle(QCoreApplication::translate("MainWindow", "Journal", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Aide", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
