//
// Created by Isaak on 4/6/2024.
//

#ifndef CONFIGURATOR_FLASHMANAGER_H
#define CONFIGURATOR_FLASHMANAGER_H

#include <QObject>
#include <QProcess>
class MainWindow;

class FlashManager : public QObject
{
    Q_OBJECT
public:
    explicit FlashManager(MainWindow *mw);
    //~FlashManager();

    void flashFW();
    void selectBinary();

    MainWindow *mw;

private:
    QProcess* m_p;

    void progressError();
    void progressFinished();
    void readProgressData();

};

#endif //CONFIGURATOR_FLASHMANAGER_H
