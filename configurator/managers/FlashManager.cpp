//
// Created by Isaak on 4/6/2024.
//

#include "FlashManager.h"
#include <QTest>
#include <QProcess>
#include <QObject>
#include <QTimer>
#include <QFileDialog>
#include "../mainwindow.h"
#include "../ui_mainwindow.h"
#include "SerialManager.h"
#include "SettingsManager.h"
#include "DebugBoxManager.h"

FlashManager::FlashManager(MainWindow *mw)
        : QObject((QObject *)mw), mw(mw) {
    m_p = nullptr;
}

//FlashManager::~FlashManager() = default;

void FlashManager::flashFW() {

    mw->ui->tbFlashLog->clear();
    mw->ui->tbFlashLog->append("Starting firmware flash...");

    //first check if a dfu device is connected
    mw->ui->tbFlashLog->append("Checking for DFU device...");
    QString deviceIndex = getDfuDevice();
    if (deviceIndex.isEmpty()) {
        mw->ui->tbFlashLog->append("No DFU device found, attempting to command mcu to enter DFU mode...");
        if (!mw->serialManager->mSerial->isOpen()) {
            mw->ui->tbFlashLog->append("Unable to conact mcu, Serial port is not open, please connect first\nExiting...");
            mw->db->loge("Flash failed");
            return;
        }
        mw->ui->tbFlashLog->append("Entering DFU mode...");
        mw->serialManager->sendCommand("dfu");

        mw->ui->tbFlashLog->append("Checking for DFU device again...");

        //check for 10 seconds if the device entered DFU mode
        for (int i = 0; i < 10; i++) {
            QTest::qWait(1000);
            deviceIndex = getDfuDevice();
            if (!deviceIndex.isEmpty()) {
                break;
            }
        }


        if (deviceIndex.isEmpty()) {
            mw->ui->tbFlashLog->append("No DFU device found, exiting...");
            mw->db->loge("Flash failed");
            return;
        }

        mw->ui->tbFlashLog->append("DFU device found: " + deviceIndex);

    }
    QTest::qWait(750); // Wait for the device to enter DFU mode

    QString binaryPath = mw->ui->leBinaryPath->text();
    if (binaryPath.isEmpty()) {
        return;
    }

    QString path = mw->ui->leBinaryPath->text();
    if (path.isEmpty()) {
        mw->db->loge("Flashing failed, no firmware file selected");
        return;
    }

    mw->ui->tbFlashLog->append("Flashing firmware from: " + path);

    // assure user that flashing will start soon
    mw->ui->prFlashing->setValue(1);

    deviceIndex = getDfuDevice();
    flashFromFile(path, deviceIndex);

    while (m_p->state() == QProcess::Running) {
        QTest::qWait(1000);
    }

    mw->ui->tbFlashLog->append("Exiting DFU mode...");
    exitDFU(deviceIndex);

    while (m_p->state() == QProcess::Running) {
        QTest::qWait(100);
    }
    mw->ui->tbFlashLog->append("All done, sending you back to the monitor tab.");
    mw->db->logs("Firmware flashed successfully");
    //reset progress bar
    mw->ui->prFlashing->setValue(0);

    //go to monitor tab
    mw->ui->tbMonitor->clear();

    if (mw->ui->optClearAfterFlash->isChecked()) {
        mw->ui->tbMonitor->clear();
    }
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

QString FlashManager::getDfuDevice() {
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

    return deviceIndex;
}

void FlashManager::flashFromFile(QString pathToBinary, QString deviceIndex) {
    //writing the firware file to the microcontroller
    m_p = new QProcess(this);

    //if pathToBineary contains any " remove it
    pathToBinary.replace("\"", "");

    QString args = "-c port=" + deviceIndex + " --write \"" + pathToBinary + "\" --verify";

    connect(m_p, &QProcess::readyReadStandardOutput, this, &FlashManager::readProgressData);
    connect(m_p, &QProcess::finished, this, &FlashManager::readProgressData);
    connect(m_p, &QProcess::errorOccurred, this, &FlashManager::progressError);

    m_p->start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", args.split(" "));

}

void FlashManager::exitDFU(QString deviceIndex) {
    //send command to leave dfu
    QString leave_dfu_args = "-c port=" + deviceIndex + " -s DFU_DNLOAD";
    m_p->start("C:/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI.exe", leave_dfu_args.split(" "));

}