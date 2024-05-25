//
// Created by Isaak on 4/7/2024.
//

#include "DebugBoxManager.h"
#include <QObject>
#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include "SerialManager.h"
#include "SettingsManager.h"
#include <QDateTime>
#include "QQueue"
#include <QScrollBar>

DebugBoxManager::DebugBoxManager(MainWindow *mw)
        : QObject(mw), mw(mw) {
    mw->ui->tbDebug->setReadOnly(true);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DebugBoxManager::removeDebugMsg);

    timer->start(1000); // Check for message removal every x
}

DebugBoxManager::~DebugBoxManager() {
    delete timer;
}

//log success
void DebugBoxManager::logs(const QString &logMessage) {
    colorLog(logMessage, "#2f6b24");
}
//log error
void DebugBoxManager::loge(const QString &logMessage) {
    colorLog(logMessage, "#852828");
}
//log warning
void DebugBoxManager::logw(const QString &logMessage) {
    colorLog(logMessage, "#856119");
}
//log info
void DebugBoxManager::logi(const QString &logMessage) {
    colorLog(logMessage, "#244991");
}
//log debug
void DebugBoxManager::logd(const QString &logMessage) {
    if (!mw->ui->optDebugMode->isChecked()) {
        return;
    }
    colorLog(logMessage, "#545454");
}

void DebugBoxManager::colorLog(const QString &logMessage, const QString &color) {
    QString logMsg = "<font color=\"" + color + "\">" + logMessage + "</font>";
    log(logMsg);
}

void DebugBoxManager::log(const QString &logMessage) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString messageWithTimestamp = timestamp + " -> " + logMessage;

    mw->ui->tbDebug->append(messageWithTimestamp);
    mw->ui->tbDebug->verticalScrollBar()->setValue(mw->ui->tbDebug->verticalScrollBar()->maximum());

    MessageInfo info;
    info.message = logMessage;
    info.timestamp = QDateTime::currentMSecsSinceEpoch();
    messageQueue.enqueue(info);
}

void DebugBoxManager::removeDebugMsg() {
    if (!messageQueue.isEmpty()) {
        qint64 oldestTimestamp = messageQueue.head().timestamp;
        qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
        qint64 messageAge = currentTime - oldestTimestamp;

        if (messageAge >= 60000) {
            mw->ui->tbDebug->moveCursor(QTextCursor::Start);
            mw->ui->tbDebug->moveCursor(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
            mw->ui->tbDebug->textCursor().removeSelectedText();
            messageQueue.dequeue();
            if (messageQueue.isEmpty()) {
                mw->ui->tbDebug->clear();
            }
        }
    }
}

void DebugBoxManager::clearLog() {
    mw->ui->tbDebug->clear();
    messageQueue.clear();
}