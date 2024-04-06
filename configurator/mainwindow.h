#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QProcess>
class SettingsManager;
class SerialManager;
class FlashManager;



#define PBCONNECT_STYLE(color) ("QPushButton { background-color: " color "; color: #fff; border-radius: 37px; }")
#define QLISTWIDGET_STYLE "QListWidget { background-color: #444; border: 1px solid #666; color: #fff; }"
#define QSTACKEDWIDGET_STYLE "QStackedWidget { background-color: #333; }"
#define QLINEEDIT_STYLE "QLineEdit { background-color: #555; border: 1px solid #666; color: #fff; }"
#define QTEXTBROWSER_STYLE "QTextBrowser { background-color: #444; color: #fff }"


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

    SettingsManager *settingsManager;
    SerialManager *serialManager;
    FlashManager *flashManager;

    Ui::MainWindow *ui;
private slots:
    void onConnectPushButtonClicked();
    void onSendPushButtonClicked();
    void onPbSelectBinClicked();
    void onPbFlashClicked();
    void onPbQuickFlashClicked();
    void changeStackedWidgetIndex();




private:


    void setStyleSheets();
    void disable_tabs_by_nr(int tab_nr, bool enable);

};
#endif // MAINWINDOW_H
