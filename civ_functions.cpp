#include <civ_functions.h>
#include <mainwindow.h>
#include <lib\tinyxml2.h>
#include <QtCore>
#include <QtNetwork>
#include <QtGui>
#include <QtWidgets>


using namespace std;



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
        qDebug() << "The file couldn't be read : " << read.ErrorID();
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
        qDebug() << "The file couldn't be read : " << read.ErrorID();
        return 0;
    }

    // Go to color level
    tinyxml2::XMLElement* value_el = read.FirstChildElement("Civ4ModularLoadControls")->FirstChildElement("ConfigurationInfos")->FirstChildElement("ConfigurationInfo")->FirstChildElement("Modules")->FirstChildElement("Module")->ToElement();

    // Loop
    int counter = -1;
    for(;; value_el=value_el->NextSiblingElement() ) {

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
        qDebug() << "The file couldn't be read : " << read.ErrorID();
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

QString check_addon_mcp()
{
    QFile addon_MCP_file("Assets/Addon_MCP.ini");
    QFile addon_MCP_file2("Assets/MCP0.FPK");
    if(addon_MCP_file2.exists() == true)
    {
        if(addon_MCP_file.exists() == true)
        {
            qDebug() << "Civ Mega Pack detected";
            QSettings addon_MCP_settings("Assets/Addon_MCP.ini", QSettings::IniFormat);
            QString MCP_version = addon_MCP_settings.value("MAIN/Version").toString();

            setCheckerParam("Addons/MCPVersion",MCP_version);
            return MCP_version;
        }
        else
        {
            setCheckerParam("Addons/MCPVersion","Unknown");
            return "Unknown";
        }

    }
    else
    {
        qDebug() << "Civ Mega Pack not detected";
        setCheckerParam("Addons/MCPVersion","Not installed");
        return "Not installed";
    }
}

QString check_addon_more_music()
{
    QFile addon_audio_file("Assets/Addon_audio.ini");
    QDir addon_audio_new("Assets/Sounds/Addon_audio");
    QDir addon_audio_old("Assets/Sounds/NEW");

    if (addon_audio_new.exists() == true || addon_audio_old.exists() == true)
    {
        qDebug() << "More music detected";
        if(addon_audio_file.exists() == true)
        {
            QSettings addon_audio_settings("Assets/Addon_audio.ini", QSettings::IniFormat);
            QString audio_version = addon_audio_settings.value("MAIN/Version").toString();

            setCheckerParam("Addons/MoreMusicVersion",audio_version);
            return audio_version;
        }
        else
        {
            setCheckerParam("Addons/MoreMusicVersion","Unknown");
            return "Unknown";
        }

    }
    else
    {
        qDebug() << "More music not detected";
        setCheckerParam("Addons/MoreMusicVersion","Not installed");
        return "Not installed";
    }
}

QString check_addon_more_handicaps()
{
    QFile addon_handicaps_file("Assets/Addon_handicap.ini");
    if(addon_handicaps_file.exists() == true)
    {
        qDebug() << "More handicaps detected";
        QSettings addon_handicaps_settings("Assets/Addon_handicap.ini", QSettings::IniFormat);
        QString handicaps_version = addon_handicaps_settings.value("MAIN/Version").toString();

        setCheckerParam("Addons/MoreHandicapsVersion",handicaps_version);

        return handicaps_version;
    }
    else
    {
        qDebug() << "More handicaps not detected";
        setCheckerParam("Addons/MoreHandicapsVersion","Not installed");
        return "Not installed";
    }
}

