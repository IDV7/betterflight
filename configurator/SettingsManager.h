//
// Created by Isaak on 4/6/2024.
//

#ifndef CONFIGURATOR_SETTINGSMANAGER_H
#define CONFIGURATOR_SETTINGSMANAGER_H


#include <QObject>
#include <QSettings>
#include "ui_mainwindow.h"


class SettingsManager : public QObject
{
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr, Ui::MainWindow *ui = nullptr);
    ~SettingsManager();
    
    void loadSettings();
    void saveSettings();

private:
    Ui::MainWindow *m_ui;

};

#endif //CONFIGURATOR_SETTINGSMANAGER_H
