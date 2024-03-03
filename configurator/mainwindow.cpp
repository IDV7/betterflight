#include "mainwindow.h"
#include "./../build-configurator-Desktop_Qt_6_6_2_MinGW_64_bit-Debug/configurator_autogen/include/ui_mainwindow.h"
#include <QDebug>
#include <QScrollBar>

#define PBCONNECT_STYLE(color) ("QPushButton { background-color: " color "; color: #fff; border-radius: 37px; }")
#define QLISTWIDGET_STYLE "QListWidget { background-color: #444; border: 1px solid #666; color: #fff; }"
#define QSTACKEDWIDGET_STYLE "QStackedWidget { background-color: #333; }"
#define QLINEEDIT_STYLE "QLineEdit { background-color: #555; border: 1px solid #666; color: #fff; }"
#define QTEXTBROWSER_STYLE "QTextBrowser { background-color: #444; color: #fff; }"



void set_style_sheets();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("BeterFlight Configurator");
    setStyleSheets();

    mSerial = new QSerialPort(this);



    updateSerialPorts();
    ui->pbConnect->setEnabled(true);

    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(5000);
    mSerialScanTimer->start();

    connect(mSerialScanTimer, &QTimer::timeout,
            this, &MainWindow::updateSerialPorts);

    connect(ui->leCLI, &QLineEdit::returnPressed,
            this, &MainWindow::onSendPushButtonClicked);

    connect(mSerial, &QSerialPort::readyRead,
            this, &MainWindow::serialReadyRead);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateSerialPorts()
{
    mSerialPorts = QSerialPortInfo::availablePorts();

    ui->cbPorts->clear();
    for (QSerialPortInfo port : mSerialPorts) {
        ui->cbPorts->addItem(port.portName(), port.systemLocation());
    }
}

void MainWindow::onConnectPushButtonClicked()
{
    //ui->pbConnect->setEnabled(false);
    QString serialName =  ui->cbPorts->currentText();
    QString serialLoc  =  ui->cbPorts->currentData().toString();

    if (mSerial->isOpen()) {
        qDebug() << "Serial already connected, disconnecting!";
        mSerial->close();
        ui->pbConnect->setText("Connect");
        ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
        return;
    }
    else {
        qDebug() << "Serial not connected, connecting!";
        ui->pbConnect->setText("Disconnect");
        ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#7a0101"));
    }


    mSerial->setPortName(serialLoc);
    mSerial->setBaudRate(QSerialPort::Baud9600);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    if(mSerial->open(QIODevice::ReadWrite)) {
        qDebug() << "SERIAL: OK!";
    } else {
        qDebug() << "SERIAL: ERROR!";
    }
    //ui->pbConnect->setEnabled(true);
}

void MainWindow::onSendPushButtonClicked()
{
    qDebug("send button pressed");
    QString str= ui->leCLI->text();
    ui->leCLI->clear();
    str.append("\r\n");
    mSerial->write(str.toLocal8Bit());
}

void MainWindow::serialReadyRead()
{
    qDebug("serial ready read");
    QByteArray data = mSerial->readAll();
    QString str = QString(data);
    ui->tbMonitor->insertPlainText(str);
    QScrollBar *sb = ui->tbMonitor->verticalScrollBar();
    sb->setValue(sb->maximum());
}


void MainWindow::setStyleSheets() {

    //pbConnect button
    ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
    ui->leCLI->setStyleSheet(QLINEEDIT_STYLE);
    ui->lwTabs->setStyleSheet(QLISTWIDGET_STYLE);
    ui->swContent->setStyleSheet(QSTACKEDWIDGET_STYLE);
    ui->tbMonitor->setStyleSheet(QTEXTBROWSER_STYLE);

    qApp->setStyleSheet("QMainWindow { background-color: #333; color: #fff; }"
                        "QMenuBar { background-color: #222; color: #fff; }"
                        "QMenu { background-color: #222; color: #fff; }"
                        "QMenu::item:selected { background-color: #555; }");
};