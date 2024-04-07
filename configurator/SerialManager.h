//
// Created by Isaak on 4/6/2024.
//

#ifndef CONFIGURATOR_SERIALMONITORMANAGER_H
#define CONFIGURATOR_SERIALMONITORMANAGER_H

#include "ui_mainwindow.h"
#include <QObject>
#include <QSerialPortInfo>
#include <QTimer>
#include <QSerialPort>
class MainWindow;

class SerialManager : public QObject
{
    Q_OBJECT
public:
    explicit SerialManager(MainWindow *mw);
    ~SerialManager();

    void sendCommand(QString command);
    void updatePortList();
    void connectSerial();
    void autoConnect();

    QSerialPort *mSerial;
private:
    MainWindow *mw;

    QList<QSerialPortInfo> mSerialPorts;
    QTimer *mSerialScanTimer;
    QTimer *auto_connect_timer;



    void serialReadyRead();
    void handleSerialError(QSerialPort::SerialPortError error);
    void writeColoredMessage(const QString &logMessage);

};

#endif //CONFIGURATOR_SERIALMONITORMANAGER_H
