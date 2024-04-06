#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QScrollBar>
#include <QFileDialog>
#include <QProcess>
#include <QTest>
#include "SerialManager.h"
#include "SettingsManager.h"
#include "FlashManager.h"


MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingsManager = new SettingsManager(this, ui);
    serialManager = new SerialManager(this, ui);
    flashManager = new FlashManager(this);


    this->setWindowTitle("BetterFlight Configurator");
    setStyleSheets();


    ui->pbConnect->setEnabled(true);


    connect(ui->leCLI, &QLineEdit::returnPressed,
            this, &MainWindow::onSendPushButtonClicked);

    connect(ui->pbQuickFlash, &QPushButton::clicked,
            this, &MainWindow::onPbQuickFlashClicked);
}


MainWindow::~MainWindow()
{
    delete ui;
    delete settingsManager;
    delete serialManager;
    delete flashManager;

}

// ------------------- Tool bar ------------------- //

void MainWindow::onConnectPushButtonClicked()
{
    serialManager->connectSerial();
}

void MainWindow::onPbQuickFlashClicked() {
    ui->lwTabs->setCurrentRow(2);
    flashManager->flashFW();
}

// ------------------- EOF Tool bar ------------------- //
// =====================================================//
// ------------------- CLI tab ------------------- //


void MainWindow::onSendPushButtonClicked()
{
    serialManager->sendCommand(ui->leCLI->text());
    ui->leCLI->clear();
}

// ------------------- EOF CLI tab ------------------- //
// =====================================================//
// ------------------- Firmware Flash tab ------------------- //

void MainWindow::onPbSelectBinClicked()
{
    flashManager->selectBinary();
}

void MainWindow::onPbFlashClicked() {
    flashManager->flashFW();
}




// ------------------- EOF Firmware Flash tab ------------------- //
// =====================================================//
// ------------------- Misc ------------------- //

void MainWindow::setStyleSheets() {

    //pbConnect button
//    ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
//    ui->leCLI->setStyleSheet(QLINEEDIT_STYLE);
//    ui->lwTabs->setStyleSheet(QLISTWIDGET_STYLE);
//    ui->swContent->setStyleSheet(QSTACKEDWIDGET_STYLE);
//    ui->tbMonitor->setStyleSheet(QTEXTBROWSER_STYLE);
//    ui->pbQuickFlash->setStyleSheet(PBCONNECT_STYLE("#948715"));

    qApp->setStyleSheet("QMenu::item:selected { background-color: #555; }");
};

// allows the disabling of tabs (QStackedWidget children) by name
void MainWindow::disable_tabs_by_nr(int tab_nr, bool enable) {
    QWidget* tabWidget = ui->swContent->widget(tab_nr);
    if(tabWidget) {
        QList<QWidget*> childWidgets = tabWidget->findChildren<QWidget*>();
        for(QWidget* childWidget : childWidgets) {
            if (!enable) {
                childWidget->setEnabled(false);
                childWidget->setStyleSheet("opacity: 0.25;");
                return;
            }
            childWidget->setEnabled(true);
            childWidget->setStyleSheet("opacity: 1;");
        }
    }
}