#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QScrollBar>
#include <QFileDialog>
#include <QProcess>
#include <QSettings>

#define PBCONNECT_STYLE(color) ("QPushButton { background-color: " color "; color: #fff; border-radius: 37px; }")
#define QLISTWIDGET_STYLE "QListWidget { background-color: #444; border: 1px solid #666; color: #fff; }"
#define QSTACKEDWIDGET_STYLE "QStackedWidget { background-color: #333; }"
#define QLINEEDIT_STYLE "QLineEdit { background-color: #555; border: 1px solid #666; color: #fff; }"
#define QTEXTBROWSER_STYLE "QTextBrowser { background-color: #444; color: #fff }"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("BeterFlight Configurator");
    loadSettings();
    setStyleSheets();

    mSerial = new QSerialPort(this);



    updateSerialPorts();
    ui->pbConnect->setEnabled(true);


    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(2500);
    mSerialScanTimer->start();

    connect(mSerialScanTimer, &QTimer::timeout,
            this, &MainWindow::updateSerialPorts);

    connect(ui->leCLI, &QLineEdit::returnPressed,
            this, &MainWindow::onSendPushButtonClicked);

    connect(mSerial, &QSerialPort::readyRead,
            this, &MainWindow::serialReadyRead);

    connect(mSerial, &QSerialPort::errorOccurred,
            this, &MainWindow::handleSerialError);
}


MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

// ------------------- Tool bar ------------------- //

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

void MainWindow::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        qDebug() << "Serial error occurred, disconnecting!";
        mSerial->close();
        ui->pbConnect->setText("Connect");
        ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
    }
}

// ------------------- EOF Tool bar ------------------- //

// ------------------- CLI tab ------------------- //

void MainWindow::writeColoredMessage(const QString& logMessage) {
    QString color;
    QString logLevel;
    QString message;

    // Extract log level and message
    int levelStart = logMessage.indexOf('[');
    int levelEnd = logMessage.indexOf(']');
    if (levelStart != -1 && levelEnd != -1) {
        logLevel = logMessage.mid(levelStart, levelEnd - levelStart + 1);
        message = logMessage.mid(levelEnd + 1).trimmed();
    } else {
        logLevel = "";
        message = logMessage;
    }

    // Set color based on log level
    if (logLevel.contains("[DEBUG]")) {
        color = "gray"; // Set debug messages to gray
    } else if (logLevel.contains("[INFO]")) {
        color = "blue"; // Set info messages to blue
    } else if (logLevel.contains("[WARN]")) {
        color = "orange"; // Set warn messages to orange
    } else if (logLevel.contains("[ERR]")) {
        color = "red"; // Set error messages to red
    } else if (logLevel.contains("[RSP]")) {
        color = "green"; // Set response messages to purple
    } else {
        color = "white"; // Default color
    }

    // Append the message with HTML formatting to the QTextBrowser
    QString formattedMessage = "<font color=\"" + color + "\">" + logLevel + "</font>" + message + "<br>";
    ui->tbMonitor->insertHtml(formattedMessage);

    // Scroll to the bottom
    ui->tbMonitor->verticalScrollBar()->setValue(ui->tbMonitor->verticalScrollBar()->maximum());
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
    static QString incompleteMessage; // Static variable to store incomplete message between function calls
    qDebug("serial ready read");
    QByteArray data = mSerial->readAll();
    QString str = QString(data);

    // Check if there's an incomplete message from previous reads
    if (!incompleteMessage.isEmpty()) {
        str.prepend(incompleteMessage); // Prepend incomplete message to new data
        incompleteMessage.clear(); // Clear the incomplete message buffer
    }

    // Process each complete message
    QStringList messages = str.split('\n');
    for (const QString& message : messages) {
        if (!message.trimmed().isEmpty()) { // Check if the message is not empty after trimming
            writeColoredMessage(message);
        }
    }

    // If the last character is not a newline, it indicates an incomplete message, so we store it for the next read
    if (!str.endsWith('\n')) {
        incompleteMessage = messages.last(); // Store the incomplete message for the next read
    }

    // Scroll to the bottom
    QScrollBar *sb = ui->tbMonitor->verticalScrollBar();
    sb->setValue(sb->maximum());
}

// ------------------- EOF CLI tab ------------------- //

// ------------------- Firmware Flash tab ------------------- //

void MainWindow::onPbSelectBinClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Firmware File"), "", tr("Firmware Files (*.hex)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->leBinaryPath->setText(fileName);
    saveSettings();
}

void MainWindow::onPbFlashClicked() {
    //put mcu in dfu
    mSerial->write("dfu");

    QString path = ui->leBinaryPath->text();
    qDebug() << "Path: " << path;
    if (path.isEmpty()) {
        qDebug("No file selected");
        return;
    }
    // check and parse the device list
    QProcess pDeviceList;
    QString args_pDeviceList = "-l";
    pDeviceList.start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", args_pDeviceList.split(" "));
    pDeviceList.waitForFinished();

    QString output = pDeviceList.readAllStandardOutput();
    QStringList lines = output.split("\n");

    QString deviceIndex;
    for (const QString &line : lines) {
        if (line.contains("Device Index")) {
            QStringList parts = line.split(":");
            if (parts.size() > 1) {
                deviceIndex = parts[1].trimmed();
                break;
            }
        }
    }

    //writing the firware file to the microcontroller
    m_p = new QProcess(this);
    QString args = "-c port=" + deviceIndex + " --write \"" + path + "\" --verify -rst";
    connect(m_p, &QProcess::readyReadStandardOutput, this, &MainWindow::readProgressData);
    connect(m_p, &QProcess::finished, this, &MainWindow::readProgressData);
    connect(m_p, &QProcess::errorOccurred, this, &MainWindow::progressError);

    m_p->start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", args.split(" "));
}

void MainWindow::readProgressData() {
    QString progData = m_p->readAllStandardOutput();
    //qDebug() << progData << progData.toHex(',');
    if(progData.contains("%")){
        int index = progData.lastIndexOf("\xdb\x20");
        if(index >=0){
            index = index+2;
            int lastindex = progData.lastIndexOf("%");
            int value = progData.mid(index, lastindex-index).toInt();
            ui->prFlashing->setValue(value);
        }
    }
}

void MainWindow::progressFinished() {
    ui->prFlashing->setValue(100);
    //ui->plainTextEdit->appendPlainText();
    m_p->deleteLater();
}

void MainWindow::progressError() {
    ui->prFlashing->setFormat("Error :");
}

// ------------------- EOF Firmware Flash tab ------------------- //

// ------------------- Q Settings ------------------- //

void MainWindow::loadSettings() {
    QSettings settings("BetterFlight", "Configurator");

    //Load the last binary path
    ui->leBinaryPath->setText(settings.value("lastBinPath").toString());
}

void MainWindow::saveSettings() {
    QSettings settings("BetterFlight", "Configurator");

    //Save the last binary path
    settings.setValue("lastBinPath", ui->leBinaryPath->text());
}

// ------------------- EOF Q Settings ------------------- //

// ------------------- Misc ------------------- //

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

