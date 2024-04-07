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
#include "DebugBoxManager.h"

#define VERSION "0.1.3"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    settingsManager = new SettingsManager(this);
    serialManager = new SerialManager(this);
    flashManager = new FlashManager(this);
    db = new DebugBoxManager(this);

    this->setWindowTitle("BetterFlight Configurator - v" VERSION);
    setStyleSheets();

    ui->pbConnect->setEnabled(true);

    ui->swContent->setCurrentIndex(1);

    connect(ui->leCLI, &QLineEdit::returnPressed,
            this, &MainWindow::onSendPushButtonClicked);

    connect(ui->pbQuickFlash, &QPushButton::clicked,
            this, &MainWindow::onPbQuickFlashClicked);

    connect(ui->actionFlash, &QAction::triggered, this, &MainWindow::changeStackedWidgetIndex);
    connect(ui->actionMonitor, &QAction::triggered, this, &MainWindow::changeStackedWidgetIndex);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::changeStackedWidgetIndex);

}


MainWindow::~MainWindow()
{
    delete ui;
    delete settingsManager;
    delete serialManager;
    delete flashManager;
    delete db;
}


void MainWindow::onConnectPushButtonClicked()
{
    serialManager->connectSerial();
}

void MainWindow::onPbQuickFlashClicked() {
    ui->swContent->setCurrentIndex(2);
    onPbFlashClicked();
}

void MainWindow::onSendPushButtonClicked()
{
    serialManager->sendCommand(ui->leCLI->text());
    ui->leCLI->clear();
}

void MainWindow::onPbSelectBinClicked()
{
    flashManager->selectBinary();
}

void MainWindow::onPbFlashClicked() {
    flashManager->flashFW();
}

void MainWindow::changeStackedWidgetIndex() {
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        QString actionName = action->text();
        int index = -1;
        if (actionName == "Monitor") {
            index = 1;
        } else if (actionName == "Flash") {
            index = 2;
        } else if (actionName == "Settings") {
            index = 3;
        }
        ui->swContent->setCurrentIndex(index);
    }
}




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