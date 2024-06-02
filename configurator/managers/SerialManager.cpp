//
// Created by Isaak on 4/6/2024.
//

#include "SerialManager.h"
#include <QSerialPortInfo>
#include <QTimer>
#include <QSerialPort>
#include "../mainwindow.h"
#include <QScrollBar>
#include <QDebug>
#include "DebugBoxManager.h"


SerialManager::SerialManager(MainWindow *mw)
        : QObject(mw), mw(mw) {

    mSerial = new QSerialPort(this);
    updatePortList();

    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(2500);
    mSerialScanTimer->start();

    connect(mSerialScanTimer, &QTimer::timeout,
                  this, &SerialManager::updatePortList);

    connect(mSerial, &QSerialPort::readyRead,
                  this, &SerialManager::serialReadyRead);

    connect(mSerial, &QSerialPort::errorOccurred,
                  this, &SerialManager::handleSerialError);

    auto_connect_timer = new QTimer(this);
    auto_connect_timer->setInterval(1000);
    auto_connect_timer->start();

    connect(auto_connect_timer, &QTimer::timeout,
                  this, &SerialManager::autoConnect);
}

SerialManager::~SerialManager() {
}

void SerialManager::sendCommand(QString command) {
    if (!mSerial->isOpen()) {
        return;
    }

    command.append("\r\n");
    mSerial->write(command.toLocal8Bit());
}

void SerialManager::serialReadyRead() {
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
    QScrollBar *sb = mw->ui->tbMonitor->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void SerialManager::updatePortList() {
    mSerialPorts = QSerialPortInfo::availablePorts();

    mw->ui->cbPorts->clear();
    for (QSerialPortInfo port : mSerialPorts) {
        mw->ui->cbPorts->addItem(port.portName(), port.systemLocation());
    }

}

void SerialManager::connectSerial() {
    QString serialName =  mw->ui->cbPorts->currentText();
    QString serialLoc  =  mw->ui->cbPorts->currentData().toString();

    if (mSerial->isOpen()) { // if connected
        mw->db->log("Disconnecting from " + serialName);
        sendCommand("disconnect");
        mSerial->close();
        mw->ui->pbConnect->setText("Connect");
        mw->ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
//        disable_tabs_by_nr(1, false);
        return;
    }

    // else, if not connected, connectSerial
    mSerial->setPortName(serialLoc);
    mSerial->setBaudRate(QSerialPort::Baud115200);
    mSerial->setDataBits(QSerialPort::Data8);
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    if(mSerial->open(QIODevice::ReadWrite)) { // only change the button if the connection was successful
        mw->db->log("Connected to " + serialName);
        mw->ui->pbConnect->setText("Disconnect");
        mw->ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#7a0101"));

        if (mw->ui->optEnterDfuNext->isChecked()) {
            sendCommand("dfu");
            mw->ui->optEnterDfuNext->setChecked(false);
        }
        else {
            sendCommand("connect"); // send connection
        }
        //        disable_tabs_by_nr(1, true);
    } else {
        if (mSerial->error() == QSerialPort::PermissionError) {
            mw->db->log("Permission denied to " + serialName);
        } else {
            if (serialName == "") {
                mw->db->log("Failed to connect to serial port");
            } else {
                mw->db->log("Failed to connect to " + serialName);

            }
        }
    }
}

void SerialManager::handleSerialError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        qDebug() << "Serial error occurred, disconnecting!";
        mSerial->close();
        mw->ui->pbConnect->setText("Connect");
        mw->ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#004011"));
//        disable_tabs_by_nr(1, false);
    }
}

//this function is called every so often to check if a new device has been connected, and if so, connectSerial to it
void SerialManager::autoConnect() {

    if (mSerial->isOpen()) {
        return;
    }

    if (!mw->ui->optAutoConnect->isChecked()) {
        return;
    }

    for (QSerialPortInfo port: mSerialPorts) {
        mSerial->setPort(port);
        if (mSerial->open(QIODevice::ReadWrite)) {
            mw->db->logi("Auto connected to " + port.portName());

            mw->ui->pbConnect->setText("Disconnect");
            mw->ui->pbConnect->setStyleSheet(PBCONNECT_STYLE("#7a0101"));
            if (mw->ui->optEnterDfuNext->isChecked()) {
                sendCommand("dfu");
                mw->ui->optEnterDfuNext->setChecked(false);
            }
            else {
                sendCommand("connect"); // send connection
            }
        }
    }
}

void SerialManager::writeColoredMessage(const QString& logMessage) {
    QString color;
    QString logLevel;
    QString message;
    int padding = 8;

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
        color = "cyan"; // Set info messages to blue
    } else if (logLevel.contains("[WARN]")) {
        color = "orange"; // Set warn messages to orange
    } else if (logLevel.contains("[ERR]")) {
        color = "red"; // Set error messages to red
    } else if (logLevel.contains("[RSP]")) {
        color = "green"; // Set response messages to green
    } else {
        color = "white"; // Default color
        padding = 0;
    }
    padding = padding - logLevel.length();

    if (padding < 0) {
        padding = 0;
    }

    QString paddingSpaces;
    for (int i = 0; i < padding; ++i) {
        paddingSpaces += "&nbsp;";
    }

    // Append the message with HTML formatting to the QTextBrowser
    QString formattedMessage = "<font color=\"" + color + "\">" + logLevel + paddingSpaces  + "</font>" + message + "<br>";
    mw->ui->tbMonitor->insertHtml(formattedMessage);

    // Scroll to the bottom
    mw->ui->tbMonitor->verticalScrollBar()->setValue(mw->ui->tbMonitor->verticalScrollBar()->maximum());
}