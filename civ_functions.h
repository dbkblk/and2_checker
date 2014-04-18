#ifndef civ_function
#define civ_function

bool dirExists(const std::string &dirName_in);
bool clearCache();
bool setStartup();
bool restoreBackup();
void replaceAll(std::string& str, const std::string& from, const std::string& to);
bool currentVersion();
bool installMod();
bool cleanUp();
bool rollBack();
bool checkUpdate();
const char* readColors();
int readColorsCounter();
bool setColors(const char* color);
const char* readXML(const char* file, const char* tag);
bool writeXML(const char* file, const char* tag, const char* newValue);

#endif