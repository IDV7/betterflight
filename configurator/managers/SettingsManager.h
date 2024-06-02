//
// Created by Isaak on 4/6/2024.
//

#ifndef CONFIGURATOR_SETTINGSMANAGER_H
#define CONFIGURATOR_SETTINGSMANAGER_H


#include <QObject>
#include <QSettings>
#include <QTimer>
class MainWindow;

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(MainWindow *mw = nullptr);
    ~SettingsManager();
    
    void loadSettings();
    void saveSettings();

private:
    MainWindow *mw;
    QSettings *settings;
    QTimer *autoSaveTimer;

    void checkForUnsavedChanges();
};

#endif //CONFIGURATOR_SETTINGSMANAGER_H
