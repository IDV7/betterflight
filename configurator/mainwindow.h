#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QProcess>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onConnectPushButtonClicked();
    void onSendPushButtonClicked();
    void serialReadyRead();
    void onPbSelectBinClicked();
    void onPbFlashClicked();
    void onPbQuickFlashClicked();
    void readProgressData();
    void progressFinished();
    void progressError();
    void updateSerialPorts();



private:
    Ui::MainWindow *ui;
    QProcess * m_p;

    QSerialPort *mSerial;
    QList<QSerialPortInfo> mSerialPorts;
    QTimer *mSerialScanTimer;
    QTimer *auto_connect_timer;

    bool cb_auto_connect = false;
    bool cb_clear_after_flash = false;


    void setStyleSheets();
    void writeColoredMessage(const QString &logMessage);
    void loadSettings();
    void saveSettings();
    void handleSerialError(QSerialPort::SerialPortError error);
    void disable_tabs_by_nr(int tab_nr, bool enable);
    void auto_connect();

    void handleAutoConnectCB(int state);
};
#endif // MAINWINDOW_H
