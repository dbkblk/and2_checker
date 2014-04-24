#include "updater.h"
#include "QLabel"
#include "QCoreApplication"
#include <QApplication>
#include <QDesktopWidget>
#include "QProcess"
#include "QFile"
#include <QDir>
#include <QSettings>

updater::updater(QWidget *parent)
    : QMainWindow(parent)
{
    central = new QLabel(this);
    central->setText("Updating...");
    setCentralWidget(central);

    // Layout definition
    this->show();
    this->setFixedHeight(50);
    this->setFixedWidth(150);
    this->setContentsMargins(15,15,15,15);
    this->setWindowTitle("Updater");
    QDir::setCurrent("../");
    central->setGeometry(20,20,125,30);



    // Update process. Download :
    tasks = new QProcess(parent);
    tasks->start("taskkill /f /im and2_checker.exe");
    tasks->waitForFinished(-1);
    QString downloadlink = "https://dl.dropboxusercontent.com/u/369241/and2_checker.7z";
    QString downloadfile = "and2_checker.7z";
    QString command = "checker/wget.exe -c --no-check-certificate " + downloadlink;
    tasks->start(command);
    tasks->waitForFinished(-1);

    // Extraction
    command = "temp/7za.exe x -y " + downloadfile;
    tasks->start(command);
    tasks->waitForFinished(-1);

    // Cleaning
    QFile::remove(downloadfile);
    if(!QFile::exists("update_out.ini")) {
        QFile ch_conf;
        ch_conf.open(QIODevice::WriteOnly);
    }
    QSettings settings("update_out.ini", QSettings::IniFormat);
    settings.setValue("UPDATE_OUT",1);
    tasks->startDetached("and2_checker.exe");
    exit();
}

updater::~updater()
{
    qApp->exit();
}

void updater::exit()
{
    qApp->exit();
}
