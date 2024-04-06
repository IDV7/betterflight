//
// Created by Isaak on 4/6/2024.
//

#include "FlashManager.h"
#include <QTest>
#include <QProcess>
#include <QObject>
#include <QTimer>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "SerialManager.h"
#include "SettingsManager.h"

FlashManager::FlashManager(MainWindow *mw)
        : QObject((QObject *)mw), mw(mw) {
    m_p = nullptr;
}

//FlashManager::~FlashManager() = default;

void FlashManager::flashFW() {

    mw->serialManager->sendCommand("dfu");
    QTest::qWait(750); // Wait for the device to enter DFU mode

    QString binaryPath = mw->ui->leBinaryPath->text();
    if (binaryPath.isEmpty()) {
        return;
    }

    QString path = mw->ui->leBinaryPath->text();
    if (path.isEmpty()) {
        return;
    }

    QProcess pDeviceList;
    QString args_pDeviceList = "-l";
    pDeviceList.start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", args_pDeviceList.split(" "));
    pDeviceList.waitForFinished();

    QString output = pDeviceList.readAllStandardOutput();
    QStringList lines = output.split("\n");

    QString deviceIndex;
    for (const QString &line : lines) {
        if (line.contains("Device Index")) {
            QStringList parts = line.split(":");
            if (parts.size() > 1) {
                deviceIndex = parts[1].trimmed();
                break;
            }
        }
    }

    // assure user that flashing will start soon
    mw->ui->prFlashing->setValue(1);

    //writing the firware file to the microcontroller
    m_p = new QProcess(this);
    QString args = "-c port=" + deviceIndex + " --write \"" + path + "\" --verify";
    connect(m_p, &QProcess::readyReadStandardOutput, this, &FlashManager::readProgressData);
    connect(m_p, &QProcess::finished, this, &FlashManager::readProgressData);
    connect(m_p, &QProcess::errorOccurred, this, &FlashManager::progressError);

    m_p->start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", args.split(" "));

    QTest::qWait(10000);


    //send command to leave dfu
    QString leave_dfu_args = "-c port=" + deviceIndex + " -s DFU_DNLOAD";
    m_p->start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", leave_dfu_args.split(" "));

    QTest::qWait(500);

    //reset progress bar
    mw->ui->prFlashing->setValue(0);

    //go to monitor tab
    mw->ui->tbMonitor->clear();
    mw->ui->swContent->setCurrentIndex(1);
}

void FlashManager::readProgressData() {
    QString progData = m_p->readAllStandardOutput();

    //make copy of the buffer and clean it up for display in the flash log text browser
    QString cleanData = progData;
    cleanData.replace("\xdb\x20", "");
    cleanData.replace("\xdb", "");
    mw->ui->tbFlashLog->append(cleanData);

    if(progData.contains("%")){
        int index = progData.lastIndexOf("\xdb\x20");
        if(index >=0){
            index = index+2;
            int lastindex = progData.lastIndexOf("%");
            int value = progData.mid(index, lastindex-index).toInt();
            mw->ui->prFlashing->setValue(value);
        }
    }
}

void FlashManager::progressFinished() {
    mw->ui->prFlashing->setValue(100);
    //ui->plainTextEdit->appendPlainText();
    m_p->deleteLater();
}

void FlashManager::progressError() {
    mw->ui->prFlashing->setFormat("Error :");
}

void FlashManager::selectBinary() {
    QString fileName = QFileDialog::getOpenFileName(mw, tr("Open Firmware File"), "", tr("Firmware Files (*.hex)"));
    if (fileName.isEmpty()) {
        return;
    }
    mw->ui->leBinaryPath->setText(fileName);
    mw->settingsManager->saveSettings();
}