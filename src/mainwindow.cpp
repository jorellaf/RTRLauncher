#include "mainwindow.h"
#include "ui_mainwindow.h"

#ifndef DATAHEADER_H
#include "dataheader.h"
#endif

#ifndef FUNCTIONS_H
#include "functions.h"
#endif

#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>

void MainWindow::resetChecks()
{
    foreach (QCheckBox *c, listofprefsmainwindow)
    {
        c->setChecked(false);
    }
    ui->showerrBox->setChecked(true);
}

bool MainWindow::checkDefaults(Ui::MainWindow *ui)
{
    bool checked = true;
    foreach (QCheckBox *c, listofprefsmainwindow)
    {
        if (c->objectName()!="showerrBox")
                checked = checked && !c->checkState();
    }
    checked = checked && ui->showerrBox->checkState();
    return checked;
}

QString playerDataTextGen(Ui::MainWindow *ui)
{
    QString dataText = "// ------------------------------------------------------------------------------------------------------------\n"\
            "// Start options checkboxes\n";
    dataText += QString("showerrBox:%1").arg(ui->showerrBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("neBox:%1").arg(ui->neBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("nmBox:%1").arg(ui->nmBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("multirunBox:%1").arg(ui->multirunBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("editorBox:%1").arg(ui->editorBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("moviecamBox:%1").arg(ui->moviecamBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("naBox:%1").arg(ui->naBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("aiBox:%1").arg(ui->aiBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("spritesBox:%1").arg(ui->spritesBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("swBox:%1").arg(ui->swBox->isChecked() ? "1" : "0") + "\n";
    dataText += QString("maprwmBox:%1").arg(ui->maprwmBox->isChecked() ? "1" : "0") + "\n";
    dataText += "\n// ------------------------------------------------------------------------------------------------------------\n"\
            "// EDU select\n";
    dataText += QString("eduComboBox:%1").arg(ui->eduComboBox->currentData().toString()) + "\n";
    dataText += "\n// ------------------------------------------------------------------------------------------------------------\n"\
            "// Trees select\n";
    dataText += QString("treesComboBox:%1").arg(ui->treesComboBox->currentData().toString()) + "\n";
    dataText += "\n// ------------------------------------------------------------------------------------------------------------\n"\
            "// Campaign select\n";
    dataText += QString("campComboBox:%1").arg(ui->campComboBox->currentData().toString()) + "\n";
    dataText += "\n// ------------------------------------------------------------------------------------------------------------\n\n"\
            "\n// ------------------------------------------------------------------------------------------------------------\n"\
            "// ------------------------------------------------------------------------------------------------------------\n"\
            "// This is an example of a comment\n"\
            "Lines without a colon will get ignored without displaying a warning too.\n"\
            "//So will empty lines.\n\n"\
            "// This file lists all the player-customised options chosen for launching the game, including start options and chosen campaign and game type.\n"\
            "// Syntax: [option code]:[setting code]\n"\
            "// File encoding must be in UTF-8!\n"\
            "// ------------------------------------------------------------------------------------------------------------\n"\
            "// ------------------------------------------------------------------------------------------------------------\n";
    return dataText;
}

int checkMapRwm()
{
    QDir basefolder(QCoreApplication::applicationDirPath() + "/../data/world/maps/base");
    QFileInfo map(basefolder.absolutePath() + "/map.rwm");
    QDateTime mapmod = map.lastModified();
    basefolder.setFilter(QDir::Files);
    if (basefolder.exists())
    {
        if (map.exists() && map.isFile())
        {
            QFileInfoList filelist = basefolder.entryInfoList(QDir::Files);
            foreach (QFileInfo fileinfo, filelist)
            {
                if (map.exists())
                {
                    QString ext = fileinfo.suffix();
                    if (ext=="tga" || ext=="txt")
                    {
                        QDateTime filemod = fileinfo.lastModified();
                        if (mapmod < filemod)
                        {
                            QFile mapfile(map.absoluteFilePath());
                            if (mapfile.remove())
                                return 0;
                            else
                                return 1;
                        }
                    }
                }
            }
            return 2;
        }
        else
            return 3;
    }
    else
        return 4;
}

QStringList setArguments(Ui::MainWindow *ui)
{
    QStringList argList;
    argList.append("-mod:RTR");
    if (ui->showerrBox->isChecked())
        argList.append("-show_err");
    if (ui->neBox->isChecked())
        argList.append("-ne");
    if (ui->nmBox->isChecked())
        argList.append("-nm");
    if (ui->multirunBox->isChecked())
        argList.append("-multirun");
    if (ui->editorBox->isChecked())
        argList.append("-enable_editor");
    if (ui->moviecamBox->isChecked())
        argList.append("-movie_cam");
    if (ui->naBox->isChecked())
        argList.append("-na");
    if (ui->aiBox->isChecked())
        argList.append("-ai");
    if (ui->spritesBox->isChecked())
        argList.append("-sprite_script");
    if (ui->swBox->isChecked())
        argList.append("-sw");

    if (ui->maprwmBox->isChecked())
    {
        int result = checkMapRwm();
        switch (result)
        {
        case 1:
            QMessageBox::critical(0,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR installation.", QMessageBox::Close);
            QMessageBox::StandardButton confirm;
            confirm = QMessageBox::warning(0,"Continue launching?", "The map file could not be deleted and will be out of date.\n"\
                                 "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
            if (confirm == QMessageBox::No)
                argList[0] = "nope";
            break;
        case 4:
            QMessageBox::critical(0,"Critical error", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                                  "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close);
                argList[0] = "nope";
            break;
        }
    }

    return argList;
}

int MainWindow::startUp()
{
    Functions f;
    //Check if all the RTR stuff is present
    QString rtrcheck = f.rtrPresent();
    if (rtrcheck!="yes")
    {
        QMessageBox rtrerror;
        rtrerror.critical(0,"Error", rtrcheck, QMessageBox::Close);
        rtrerror.setFixedWidth(200);
        return 404;
    }

    // Read launcher data
    ListOptionObjects *l = &listoptions;
    if (f.readLauncherData(l)==500)
        return 500;

    // EDU options
    QComboBox *eduComboBox = findChild<QComboBox*>("eduComboBox");
    f.setOptions(l, eduComboBox,"edu");

    // Campaign options
    QComboBox *campComboBox = findChild<QComboBox*>("campComboBox");
    f.setOptions(l, campComboBox, "camp");

    // Campaign options
    QComboBox *treesComboBox = findChild<QComboBox*>("treesComboBox");
    f.setOptions(l, treesComboBox, "trees");

    resetChecks();

    // Read player data
    f.readPlayerData(&playeroptionlist,this);

    return 0;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);

    this->listofprefsmainwindow = {ui->showerrBox, ui->neBox, ui->nmBox, ui->multirunBox, ui->editorBox, ui->moviecamBox, ui->naBox, ui->aiBox, ui->spritesBox, ui->swBox, ui->maprwmBox};

    // snippet taken from https://stackoverflow.com/a/23227915
    QPixmap bground(":/images/marbletexture.png");
    bground = bground.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bground);
    this->setPalette(palette);
    //

    // snippet taken from https://stackoverflow.com/a/12734881
    QRect *rect = new QRect(23,22,155,156);
    QRegion* region = new QRegion(*rect,QRegion::Ellipse);
    ui->launchButton->setMask(*region);
    //
    preferencesd = new PreferencesDialog;

    Functions f;
    f.readPreferences(preferencesd);
    preferencesd->setChangesMadeFalse();

    QPixmap bground2(":/images/marbletexture.png");
    bground2 = bground2.scaled(preferencesd->size(), Qt::IgnoreAspectRatio);
    QPalette palette2;
    palette2.setBrush(QPalette::Background, bground2);
    preferencesd->setPalette(palette2);
}

MainWindow::~MainWindow()
{
    delete preferencesd;
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::quit();
}

void MainWindow::on_launchButton_pressed()
{
    QIcon i;
    i.addPixmap(QPixmap(":/images/rtr_pressed.png"));
    ui->launchButton->setIcon(i);
}

void MainWindow::on_launchButton_released()
{
    QIcon i;
    i.addPixmap(QPixmap(":/images/rtr.png"));
    ui->launchButton->setIcon(i);
}

void MainWindow::on_launchButton_clicked()
{
    ui->launchButton->setEnabled(false);
    //Save settings
    QString dataText = playerDataTextGen(ui);
    bool goahead = true;

    Functions f;

    if(f.writePreferences(preferencesd->preferencesText)==500)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::warning(0,"Continue launching?", "Your preferences could not be saved, and will be discarded if you launch the game.\n"\
                             "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }

    if(f.writePlayerData(dataText)==500)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::warning(0,"Continue launching?", "Your changes could not be saved, and will be discarded if you launch the game.\n"\
                             "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }

    QString errortext = "";
    // EDU switch
    QString comboboxtext = ui->eduComboBox->currentText();
    int eduswitch = f.filefolderSwitch(ui->eduComboBox->currentData().toString(), listoptions.getListOfEdus());
    switch (eduswitch)
    {
    case 1:
        errortext = QString("The folder for the game type '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 2:
        errortext = QString("The file for the game type '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 3:
        errortext = QString("The files for the game type '%1' could not be found.\n").arg(comboboxtext);
        break;
    default:
        break;
    }

    // Campaign switch
    comboboxtext = ui->campComboBox->currentText();
    int campswitch = f.filefolderSwitch(ui->campComboBox->currentData().toString(), listoptions.getListOfCamps());
    switch (campswitch)
    {
    case 1:
        errortext += QString("The folder for the campaign '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 2:
        errortext += QString("The file for the campaign '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 3:
        errortext += QString("The files for the campaign '%1' could not be found.\n").arg(comboboxtext);
        break;
    default:
        break;
    }

    // Trees switch
    comboboxtext = ui->treesComboBox->currentText();
    int treesswitch = f.filefolderSwitch(ui->treesComboBox->currentData().toString(), listoptions.getListOfTrees());
    switch (treesswitch)
    {
    case 1:
        errortext += QString("The folder for the tree types '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 2:
        errortext += QString("The file for the tree types '%1' could not be copied.\n").arg(comboboxtext);
        break;
    case 3:
        errortext += QString("The files for the tree types '%1' could not be found.\n").arg(comboboxtext);
        break;
    default:
        break;
    }


    if (!errortext.isEmpty())
    {
        errortext += "\nAlthough the game may run normally, it is likely your desired configuration will\nnot be in effect, and the game might not even start correctly\n\nDo you wish to continue launching the game?";
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::critical(0,"Error!:",errortext,QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }
    //qDebug() << eduswitch;

    QStringList commands = setArguments(ui);
    if (goahead == true && commands[0]!="nope")
    {

        // snippet taken from https://stackoverflow.com/a/19442886
        QFileInfo check_file(QCoreApplication::applicationDirPath() + "/../../RomeTW-ALX.exe");
        //qDebug() << check_file.absoluteFilePath();
        QProcess *process = new QProcess(this);
        if (check_file.exists() && check_file.isFile())
        {
            QString file = check_file.absoluteFilePath();
            //file += commandlineswitches;
            //QMessageBox::information(0,"Debug info", commands[commands.length()-1], QMessageBox::Close);
            //process->setNativeArguments("-mod:RTR");
            process->startDetached(file,commands,QCoreApplication::applicationDirPath() + "/../..");
            process->waitForStarted();
            QApplication::quit();
        }
        else
            QMessageBox::critical(0,"Critical error", "The Rome Total War: Alexander executable was not found. Make sure you have installed RTR correctly,"\
                                                  "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close);
    }
    ui->launchButton->setEnabled(true);
}

void MainWindow::on_mapButton_clicked()
{
    int result = checkMapRwm();
    switch (result)
    {
    case 0:
        QMessageBox::information(0,"Success", "The map.rwm file was out of date and was successfully deleted.\nAn up-to-date file will be generated by the game at launch.", QMessageBox::Close);
        break;
    case 1:
        QMessageBox::critical(0,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR installation.", QMessageBox::Close);
        break;
    case 2:
        QMessageBox::information(0,"Success", "The map.rwm file was up-to-date.\nNo further action was needed.", QMessageBox::Close);
        break;
    case 3:
        QMessageBox::information(0,"Success", "The map.rwm file was not present.\nAn up-to-date file will be generated by the game at launch.", QMessageBox::Close);
        break;
    case 4:
        QMessageBox::critical(0,"Critical error", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                              "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close);
        break;
    }
}

void MainWindow::on_preferencesButton_clicked()
{
    preferencesd->show();
}

void MainWindow::on_saveButton_clicked()
{
    QString dataText = playerDataTextGen(ui);
    Functions f;
    if (f.writePlayerData(dataText)==0)
        QMessageBox::information(0,"Success", "Your settings have been successfully saved.", QMessageBox::Ok);
}

void MainWindow::on_discardButton_clicked()
{
    QMessageBox::StandardButton confirm;
      confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to exit?\nAll your unsaved changes will be lost.",
                                    QMessageBox::Yes|QMessageBox::No);
      if (confirm == QMessageBox::Yes)
        QApplication::quit();
}

void MainWindow::on_defaultsButton_clicked()
{
    if (!checkDefaults(ui))
    {
        QMessageBox::StandardButton confirm;
          confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to reset to the default settings?\nYour previous settings will still be stored in your\ndata file unless you launch the game or manually save.",
                                        QMessageBox::Yes|QMessageBox::No);
          if (confirm == QMessageBox::Yes)
            resetChecks();
    }
}
