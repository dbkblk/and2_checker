#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <shlobj.h>
#include <wchar.h>
#include <civ_functions.h>
#include <mainwindow.h>
#include <lib\tinyxml2.h>
#include <QDir>
#include <QDebug>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QEventLoop>
#include <QObject>
#include <QSettings>

using namespace std;

// Clear the cache folder

bool clearCache()
{
	// Getting the cache path
    string cacheDir;
    string delCmd = "DEL /Q ";
    string quote = "\"";
    string finalDir = "\\My Games\\Beyond the Sword\\cache\\";
    string dat = "*";
    char* Appdata = getenv("LOCALAPPDATA");
    cacheDir = delCmd + quote + Appdata + finalDir + dat + quote;
    // cout << cacheDir << endl;

    return 0;
}

// Set the mod to start by default
bool setConfigParam(QString param, QString newValue)
{
    // Make a backup
    QFile::remove("../../CivilizationIV.bak");
    QFile::copy("../../CivilizationIV.ini", "../../CivilizationIV.bak");

    // Set value
    QSettings settings("../../CivilizationIV.ini", QSettings::IniFormat);
    settings.setValue(param, newValue);

    qDebug() << "Parameter set to " << settings.value(param);
    return 0;
}

QString readConfigParam(QString param)
{
    QSettings settings("../../CivilizationIV.ini", QSettings::IniFormat);
    QString value = settings.value(param).toString();
    qDebug() << value;
    return value;
}

QString readCheckerParam(QString param)
{
    QSettings settings("checker/checker_config.ini", QSettings::IniFormat);
    qDebug() << settings.status();
    if(!settings.contains(param)) {
        return "error";
    }
    QString value = settings.value(param).toString();
    qDebug() << "Checker parameter set to " << settings.value(param);
    return value;
}

bool setCheckerParam(QString param, QString newValue)
{
    if(!QFile::exists("checker/checker_config.ini")) {
        QFile ch_conf;
        ch_conf.open(QIODevice::WriteOnly);
    }
    QSettings settings("checker/checker_config.ini", QSettings::IniFormat);
    qDebug() << settings.status();
    settings.setValue(param, newValue);
    qDebug() << "Checker parameter set to" <<       settings.value(param);
    return 0;
}

bool launcherCheck()
{
    Downloader d;
    d.download("https://dl.dropboxusercontent.com/u/369241/update.ini","checker/update.ini");
    QSettings upd_ini("checker/update.ini", QSettings::IniFormat);
    QString loc_version = readCheckerParam("MAIN/CheckerVersion");
    QString dist_version = upd_ini.value("VERSION/CheckerVersion").toString();
    if(loc_version == dist_version) {
        qDebug() << "No update is required";
        QFile::remove("checker/update.ini");
        return 0;
    }
    else
        qDebug() << "An update is available";
        return 1;
}

bool launcherUpdate()
{
    QSettings upd_ini("checker/update.ini", QSettings::IniFormat);
    QString downloadlink = upd_ini.value("VERSION/DownloadLink").toString();
    QString downloadfile = upd_ini.value("VERSION/DownloadFile").toString();
    qDebug() << "Link : " << downloadlink << endl << "File : " << downloadfile;
    char cmd[512];
    QFile::copy("checker/7za.exe","7za.exe");
    sprintf(cmd, "echo Downloading launcher update... && TIMEOUT 3 && checker\\wget.exe -c --no-check-certificate %s && taskkill /f /im and2_checker.exe >NUL 2>NUL && echo Extracting update... && 7za.exe x -y %s && echo Update done && del 7za.exe && TIMEOUT 3 && start and2_checker.exe", downloadlink.toStdString().c_str(), downloadfile.toStdString().c_str());
    qDebug() << "Update command : " << cmd;
    system((char *)cmd);
}

bool restoreBackup()
{
    QFile::remove("../../CivilizationIV.ini");
    QFile::copy("../../CivilizationIV.bak", "../../CivilizationIV.ini");
	return 0;
}

bool checkUpdate()
{
    qDebug() << "Checking for update..." << endl;
    system("checker\\svn.exe update && echo The cache will now be cleared && TIMEOUT 3");
    clearCache();
    return 0;
    /*QProcess pr_update;
    pr_update.startDetached("checker/svn.exe update");
    qDebug() << pr_update.pid();*/

}

bool rollBack()
{
    system("checker\\svn.exe update -r PREV --accept theirs-full && echo The cache will now be cleared && TIMEOUT 3");
    clearCache();
    return 0;
}

bool cleanUp()
{
    system("checker\\svn.exe cleanup");
    clearCache();
    cout << "The mod is reverted to the last working version." << endl;
    return 0;
}

/*const char* readXML(const char* file, const char* tag)
{
    tinyxml2::XMLDocument read;
    read.LoadFile(file);
    const char* value = read.FirstChildElement("versions")->FirstChildElement(tag)->GetText();
    qDebug() << "readXML : " << value;
    return value;
}

bool writeXML(const char* file, const char* tag, const char* newValue)
{
    tinyxml2::XMLDocument read;
    read.LoadFile(file);
    read.FirstChildElement("versions")->FirstChildElement(tag)->SetText(newValue);
    read.SaveFile(file);
    return 0;
}*/

const char* readColors()
{
    // Open the file
    tinyxml2::XMLDocument read;
    const char* file = "Assets/Modules/Interface Colors/MLF_CIV4ModularLoadingControls.xml";
    read.LoadFile(file);
    if (!read.ErrorID() == 0){
        cout << "The file couldn't be read : " << read.ErrorID()<< endl;
        return 0;
    }

    // Go to color level
    tinyxml2::XMLElement* value_el = read.FirstChildElement("Civ4ModularLoadControls")->FirstChildElement("ConfigurationInfos")->FirstChildElement("ConfigurationInfo")->FirstChildElement("Modules")->FirstChildElement("Module")->ToElement();

    // Loop
    for(;; value_el=value_el->NextSiblingElement() ) {
        const char* value = value_el->FirstChildElement("Directory")->GetText();
        const char* bLoad = value_el->FirstChildElement("bLoad")->GetText();

        if (!strcmp(bLoad, "1")) {
            return value;
        }

    }
    return 0;
}

int readColorsCounter()
{
    // Open the file
    tinyxml2::XMLDocument read;
    const char* file = "Assets/Modules/Interface Colors/MLF_CIV4ModularLoadingControls.xml";
    read.LoadFile(file);
    if (!read.ErrorID() == 0){
        cout << "The file couldn't be read : " << read.ErrorID()<< endl;
        return 0;
    }

    // Go to color level
    tinyxml2::XMLElement* value_el = read.FirstChildElement("Civ4ModularLoadControls")->FirstChildElement("ConfigurationInfos")->FirstChildElement("ConfigurationInfo")->FirstChildElement("Modules")->FirstChildElement("Module")->ToElement();

    // Loop
    int counter = -1;
    for(;; value_el=value_el->NextSiblingElement() ) {

        const char* value = value_el->FirstChildElement("Directory")->GetText();
        const char* bLoad = value_el->FirstChildElement("bLoad")->GetText();
        counter++;
        if (!strcmp(bLoad, "1")) {
            return counter;
        }

    }
    return 0;
}

bool setColors(const char* color)
{
    // Open the file
    tinyxml2::XMLDocument read;
    const char* file = "Assets/Modules/Interface Colors/MLF_CIV4ModularLoadingControls.xml";
    read.LoadFile(file);
    const char* resetValue = "0";
    if (!read.ErrorID() == 0){
        cout << "The file couldn't be read : " << read.ErrorID()<< endl;
        return 1;
    }


    // Go to color level
    tinyxml2::XMLElement* value_el = read.FirstChildElement("Civ4ModularLoadControls")->FirstChildElement("ConfigurationInfos")->FirstChildElement("ConfigurationInfo")->FirstChildElement("Modules")->FirstChildElement("Module")->ToElement();

    // Reset all values
    for(value_el; value_el; value_el=value_el->NextSiblingElement() ) {
        value_el->FirstChildElement("bLoad")->SetText(resetValue);
    }

    tinyxml2::XMLElement* valueSet_el = read.FirstChildElement("Civ4ModularLoadControls")->FirstChildElement("ConfigurationInfos")->FirstChildElement("ConfigurationInfo")->FirstChildElement("Modules")->FirstChildElement("Module")->ToElement();

    for(valueSet_el; valueSet_el; valueSet_el=valueSet_el->NextSiblingElement() ) {
        const char* txtValue = valueSet_el->FirstChildElement("Directory")->GetText();
        if (strcmp(txtValue ,color) == 0) {
            valueSet_el->FirstChildElement("bLoad")->SetText("1");
        }
    }
    read.SaveFile(file);
    return 0;
}

void launchGame(){
    QString bt_exe = readCheckerParam("MAIN/ExecutablePath");
    QString exec = QDir::toNativeSeparators(bt_exe);
    QUrl u = QUrl::fromLocalFile(exec);
    QDesktopServices::openUrl(QUrl(u));
}
