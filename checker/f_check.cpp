#include "f_civ.h"
#include "w_main.h"
#include "f_svn.h"

#include "QObject"
#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include <QFile>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include <QDir>

f_check::f_check(QObject *parent) :
    QObject(parent)
{
    _working =false;
    _abort = false;
}

void f_check::requestWork()
{
    mutex.lock();
    _working = true;
    _abort = false;
    //qDebug() << "Request worker start in Thread "<<thread()->currentThreadId();
    qDebug("Starting update checking...");
    mutex.unlock();

    emit workRequested();
}

void f_check::abort()
{
    mutex.lock();
    if (_working) {
        _abort = true;
        qDebug()<<  "Request worker aborting in Thread " <<   thread()->currentThreadId();
    }
    mutex.unlock();
}

// TO REVIEW - Reason : SVN -> RSYNC
void f_check::UMCheckUpdate()
{
    /*
    // Wait 3s before to check for update
    QEventLoop loop;
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();

    // Begin processing
    qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();
    bool update;
    QProcess download;
    download.start(constants::GLOBAL_UPDATE_URL);

    if (download.waitForFinished(60000))
    {
        QSettings upd_ini("checker/update.ini", QSettings::IniFormat);

        // Reading update info
        setCheckerParam("Update/Changelog",upd_ini.value("VERSION/Changelog").toString());
        setCheckerParam("Update/DistantCheckerMajorVersion",upd_ini.value("VERSION/CheckerMajorVersion").toString());
        setCheckerParam("Update/DistantCheckerMinorVersion",upd_ini.value("VERSION/CheckerMinorVersion").toString());

        if(LauncherVersionCalculation() == true || svnLocalInfo() < svnDistantInfo()){
            update = true;
            qDebug() << "Update is " << update;
        }

        else
        {
            qDebug() << "No update is required";
            update = false;
        }
    }

    // Set _working to false, meaning the process can't be aborted anymore.
    mutex.lock();
    _working = false;
    mutex.unlock();

    //qDebug()<<"Worker process finished in Thread "<<thread()->currentThreadId();
    qDebug("Update checking ended...");
    // Finished signal
    emit finished(update);
    */
}

bool f_check::ActionLauncherUpdate()
{
    QSettings upd_ini("checker/update.ini", QSettings::IniFormat);
    QStringList downloadlink;
    downloadlink << upd_ini.value("VERSION/DownloadLink").toString();
    QFile::copy("checker/upd_proc.exe","upd_proc.exe");
    QProcess update;
    update.startDetached("upd_proc.exe",downloadlink);

    return 0;
}

bool f_check::LauncherVersionCalculation()
{
    if (readCheckerParam("Main/CheckerMajorVersion").toInt() < readCheckerParam("Update/DistantCheckerMajorVersion").toInt())
    {
        return true;
    }
    else if (readCheckerParam("Main/CheckerMajorVersion").toInt() == readCheckerParam("Update/DistantCheckerMajorVersion").toInt() && readCheckerParam("Main/CheckerMinorVersion").toInt() < readCheckerParam("Update/DistantCheckerMinorVersion").toInt())
    {
        return true;
    }
    else
    {
        return false;
    }
}
