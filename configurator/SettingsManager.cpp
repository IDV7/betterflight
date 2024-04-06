//
// Created by Isaak on 4/6/2024.
//

#include "SettingsManager.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QSettings>


SettingsManager::SettingsManager(QObject *parent, Ui::MainWindow *ui)
        : QObject(parent), m_ui(ui) {
    loadSettings();
}

SettingsManager::~SettingsManager() {
    saveSettings();
}

void SettingsManager::loadSettings() {
    QSettings settings("BetterFlight", "Configurator");

    if (!settings.contains("initialized")) {
        saveSettings(); // Save the default settings if the app is ran for the first time to prevent crash.
    }

    //Load the last binary path
    m_ui->leBinaryPath->setText(settings.value("lastBinPath").toString());

    //Load the auto connectSerial checkbox
    m_ui->cbOptAutoConnect->setChecked(settings.value("autoConnect").toBool() ? Qt::Checked : Qt::Unchecked);
}

void SettingsManager::saveSettings() {
    QSettings settings("BetterFlight", "Configurator");

    settings.setValue("initialized", true);

    //Save the last binary path
    settings.setValue("lastBinPath", m_ui->leBinaryPath->text());

    //Save the auto connectSerial checkbox
    settings.setValue("autoConnect", m_ui->cbOptAutoConnect->isChecked() ? true : false);
}
