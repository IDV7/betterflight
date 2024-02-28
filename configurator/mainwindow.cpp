#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Set dark mode stylesheet
    qApp->setStyleSheet("QMainWindow { background-color: #333; color: #fff; }"
                        "QMenuBar { background-color: #222; color: #fff; }"
                        "QMenu { background-color: #222; color: #fff; }"
                        "QMenu::item:selected { background-color: #555; }"
                        "QListWidget { background-color: #444; color: #fff; }"
                        "QTextEdit, QLineEdit { background-color: #555; color: #fff; }"
                        "QPushButton { background-color: #555; color: #fff; border-radius: 5px; }");

    // Set rounded corners for QPushButton
    ui->pbConnect->setStyleSheet("QPushButton { border-radius: 5px; }");

    // Set rounded corners for QTextEdit and QLineEdit
    ui->teMonitor->setStyleSheet("QTextEdit, QLineEdit { border-radius: 5px; }");
    ui->leCLI->setStyleSheet("QTextEdit, QLineEdit { border-radius: 5px; }");
}

MainWindow::~MainWindow()
{
    delete ui;
}
