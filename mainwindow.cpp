#include "mainwindow.h"
#include "civ_functions.h"
#include "optionbox.h"
#include "updatebox.h"
#include "updatemanager.h"
#include "ui_mainwindow.h"
#include "ui_installBox.h"
#include "ui_optionBox.h"
#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QtGui>
#include <QtWidgets>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Checker version
    setCheckerParam("Main/CheckerVersion",QString::number(constants::GLOBAL_CHECKER_VERSION));

    // Creation of widgets
    ubox = new updatebox(this);
    optbox = new optionBox(this);
    updateGUI = new QWidget (this);
    update_manager = new updateManager(this);

    // Main window shape

    this->setWindowTitle("Civilization IV: A New Dawn");
    setStyleSheet("MainWindow { background-image: url(checker/and2_background.jpg); background-position: bottom }");

    /*  Thread code, imported from https://github.com/fabienpn/simple-qt-thread-example */
    thread = new QThread();
    worker = new Worker();

    worker->moveToThread(thread);
    connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
    connect(thread, SIGNAL(started()), worker, SLOT(UMCheckUpdate()));
    connect(worker, SIGNAL(finished(bool)), thread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(finished(bool)), this, SLOT(UpdateWindowInfos()), Qt::DirectConnection);
    connect(worker, SIGNAL(finished(bool)), this, SLOT(UpdateAvailable(bool)));
    connect(worker, SIGNAL(finished(bool)), update_manager, SLOT(updateDistantInfos()));


    // Check launcher update in background (to avoid having two threads running simultaneously, the previous thread is aborted).
    worker->abort();
    thread->wait(); // If the thread is not running, this will immediately return.
    worker->requestWork();

    // Update labels and buttons
    UpdateWindowInfos();

    // Check for addons
    check_addon_mcp();
    check_addon_more_handicaps();
    check_addon_more_music();
}

MainWindow::~MainWindow()
{
    QProcess::execute("taskkill /f /im curl.exe");
    // Abort threads and close the ui
    worker->abort();
    thread->wait();
    qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
    delete thread;
    delete worker;
    delete ui;

}

void MainWindow::UpdateWindowInfos()
{
    // Versions label on the main Window

    QString vers = "Launcher rev. " + readCheckerParam("Main/CheckerVersion") + "\nMod rev. " + readCheckerParam("Main/LocalRev");
    QPalette lb_palette;
    lb_palette.setColor(QPalette::WindowText, Qt::black);
    ui->lb_versions->setPalette(lb_palette);
    ui->lb_versions->setText(vers);
}

void MainWindow::RestoreButtonState()
{
    ui->bt_update->setStyleSheet("");
    ui->bt_update->setText("Check for update");
    return;
}

void MainWindow::UpdateAvailable(bool update)
{
    // Detect if it's a launcher update or another type

    if(update)
    {
        if(readCheckerParam("Main/CheckerVersion") < readCheckerParam("Update/DistantCheckerVersion"))
        {
            if(readCheckerParam("Main/CheckerAutoUpdate") == "1")
            {
                launcherUpdate();
            }
            else
            {
                askUpdate.setWindowTitle("Launcher update available");
                askUpdate.setText("An update of the launcher is available.");
                askUpdate.setInformativeText("Do you want to update ?");
                askUpdate.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                int ret = askUpdate.exec();
                switch (ret) {
                    case QMessageBox::Ok :
                        launcherUpdate();
                        break;

                    case QMessageBox::Cancel :
                        return;
                        break;
                }
            }
        }

        else
        {
            ui->bt_update->setStyleSheet("background-color: yellow");
            ui->bt_update->setText("Update available !");
            return;
        }
    }

    return;
}

// Menu actions

void MainWindow::on_actionForum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.civfanatics.com/forumdisplay.php?f=369"));
}

void MainWindow::on_actionAddon_Mega_Civ_Pack_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.civfanatics.com/showthread.php?t=521289"));
}

void MainWindow::on_actionBugreport_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.civfanatics.com/showthread.php?t=474185"));
}

void MainWindow::on_actionAbout_AND_Resurrection_team_forum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.civfanatics.com/showthread.php?t=471460"));
}

void MainWindow::on_actionAddon_More_music_forum_triggered()
{
    QDesktopServices::openUrl(QUrl("http://forums.civfanatics.com/showthread.php?t=523763"));
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionOpen_mod_folder_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(QDir::currentPath()));
}

// Menu buttons

void MainWindow::on_bt_update_clicked()
{
    // Calculate changelog difference

    int chglog_diff = readCheckerParam("Update/DistantRev").toInt() - readCheckerParam("Main/LocalRev").toInt();
    qDebug() << "The changelog diff is equal to " << chglog_diff;

    // If there are update, show the changelog in a window

    if(chglog_diff == 0) {
        QMessageBox::information(this, "Information", "There is no update at the moment.");
    }
    else if(chglog_diff >= 1) {
        bool value = true;
        char command[30];
        sprintf(command,"checker/svn.exe log -l %d -r HEAD:BASE",chglog_diff);
        ubox->show();
        ubox->setWindowTitle("Update tool");
        ubox->execute(command,value);
        connect(ubox,SIGNAL(finished()),this,SLOT(UpdateWindowInfos()));
    }
    else
        QMessageBox::critical(this, "Error", "An error has occured while checking for updates.");
}

void MainWindow::on_bt_launch_clicked()
{
    // Check if the game path is known

    if(readCheckerParam("Main/ExecutablePath") == "error") {
        QMessageBox::information(0, "Information", "To be able to launch the game from the launcher, you need to set the game path in the options window. (Options > Select game path)");
        return;
    }
    else {
        launchGame();
    }

    // Check if the launcher should quit

    if(readCheckerParam("Main/QuitLauncher") == "1") {
        qApp->exit();
    }
    else {
        this->setWindowState(Qt::WindowMinimized);
    }
}

void MainWindow::on_bt_option_clicked()
{
    // Invoke the option window

    optbox->show();
}

// Installation process

installBox::installBox(QDialog *parent) :
  QDialog(parent),
  ui(new Ui::installBox)
{
    // Installation window objects

    ui->setupUi(this);
    inst_view = new updatebox(this);
}

void installBox::on_buttonBox_accepted()
{
    // Setup the initial window and launch checkout command in a box

    inst_view->show();
    inst_view->installMode();
    bool cursor = false;
    inst_view->execute("checker/svn.exe checkout \"svn://svn.code.sf.net/p/anewdawn/code/Trunk/Rise of Mankind - A New Dawn\" .", cursor);
    connect(inst_view->bt_chglog_close,SIGNAL(clicked()),inst_view,SLOT(close()));
}

void installBox::on_buttonBox_rejected()
{
    qApp->exit();
}

void MainWindow::on_bt_addons_clicked()
{
    window_installer = new Addons(this);
    window_installer->show();
}

void MainWindow::on_actionPack_binaries_for_git_triggered()
{
    system("checker\\PackBaseFile.bat");
}

void MainWindow::on_pushButton_clicked()
{
    update_manager->show();
}
