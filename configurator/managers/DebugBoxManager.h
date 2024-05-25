//
// Created by Isaak on 4/7/2024.
//

#ifndef CONFIGURATOR_DEBUGBOXMANAGER_H
#define CONFIGURATOR_DEBUGBOXMANAGER_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QPair>
#include <QQueue>
class MainWindow;

class DebugBoxManager : public QObject {
    Q_OBJECT
public:
    explicit DebugBoxManager(MainWindow *mw);
    ~DebugBoxManager();

    void log(const QString &logMessage);
    void colorLog(const QString &logMessage, const QString &color);
    void logs(const QString &logMessage);
    void loge(const QString &logMessage);
    void logw(const QString &logMessage);
    void logi(const QString &logMessage);
    void logd(const QString &logMessage);

    void clearLog();


    MainWindow *mw;

private slots:
    void removeDebugMsg();

private:
    QTimer *timer;
    struct MessageInfo {
        QString message;
        qint64 timestamp;
    };
    QQueue<MessageInfo> messageQueue;

};


#endif //CONFIGURATOR_DEBUGBOXMANAGER_H
