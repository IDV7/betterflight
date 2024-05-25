//
// Created by Isaak on 4/6/2024.
//

#include "SettingsManager.h"
#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include <QSettings>
#include <QTimer>
#include "DebugBoxManager.h"


SettingsManager::SettingsManager(MainWindow *mw)
        : QObject(mw), mw(mw) {
    settings = new QSettings("BetterFlight", "Configurator");
    loadSettings();

    //every so often check if the settings have been saved
    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setInterval(1500);
    autoSaveTimer->start();
    connect(autoSaveTimer, &QTimer::timeout, this, &SettingsManager::checkForUnsavedChanges);
}

SettingsManager::~SettingsManager() {
    saveSettings();
    delete settings;
    delete autoSaveTimer;

}

void SettingsManager::loadSettings() {
    if (!settings->contains("initialized")) {
        saveSettings(); // Save the default settings if the app is ran for the first time to prevent crash.
    }

    //Load the last binary path
    mw->ui->leBinaryPath->setText(settings->value("lastBinPath").toString());

    //Load the optAutoConnect

    mw->ui->optAutoConnect->setChecked(settings->value("optAutoConnect").toBool() ? Qt::Checked : Qt::Unchecked);
    mw->ui->optClearAfterFlash->setChecked(settings->value("optClearAfterFlash").toBool() ? Qt::Checked : Qt::Unchecked);
    mw->ui->optDebugMode->setChecked(settings->value("optDebugMode").toBool() ? Qt::Checked : Qt::Unchecked);

}

void SettingsManager::saveSettings() {
    settings->setValue("initialized", true);

    //Save the last binary path
    settings->setValue("lastBinPath", mw->ui->leBinaryPath->text());
}

//saves every x seconds
void SettingsManager::checkForUnsavedChanges() {
    settings->setValue("optAutoConnect", mw->ui->optAutoConnect->isChecked());
    settings->setValue("optClearAfterFlash", mw->ui->optClearAfterFlash->isChecked());
    settings->setValue("optDebugMode", mw->ui->optDebugMode->isChecked());
}