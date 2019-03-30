// Include class headers.
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Include Qt classes used in the code.
#include <QApplication>
#include <QDir>
#include <QProcess>
#include <QFileInfo>
#include <QMessageBox>
#include <QDateTime>
#include <QLocale>

// ==================
// Public methods:

// Constructor for the UI window element class (parent window intialised to none by default; *parent = nullptr).
MainWindow::MainWindow(QWidget *parent) :
    // Default Qt UI window class inheritance.
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Default Qt UI setup declaration.
    ui->setupUi(this);
    // Setting the window flags. The main window should be a window (Qt::Window), it should have a minimise and close
    // buttons, but maximise should be disabled (Qt::WindowMinimize... & Qt::WindowClose...), and, on Windows, since it
    // is a window of fixed size, it should look as such (Qt::MSWindowsFixedSize...).
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    // Set the locale of the main launcher window to be the system's locale.
    this->setLocale(QLocale::system());

    // Store all the checkboxes in the mainwindow in the QList of QCheckBoxes variable.
    checkboxeslist = {ui->showerrBox, ui->neBox, ui->nmBox, ui->multirunBox, ui->editorBox, ui->moviecamBox, ui->naBox, ui->aiBox, ui->spritesBox, ui->swBox, ui->maprwmBox};

    // Snippet by user 'Barracuda' from https://stackoverflow.com/a/23227915.
    // Set an image as the background of the launcher window.
    QPixmap bground(":/images/marbletexture.png");
    bground = bground.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bground);
    this->setPalette(palette);
    // End of snippet.

    // Snippet by user 'stackunderflow' and 'Basti Vagabond' from https://stackoverflow.com/a/12734881.
    // Make main button have an elliptical shape, fit to the button icon.
    QRect rect(23,22,155,156);
    QRegion region (rect,QRegion::Ellipse);
    ui->launchButton->setMask(region);
    // End of snippet.

    prefsdialog = new PreferencesDialog(this);

    QPixmap bground2(":/images/marbletexture.png");
    bground2 = bground2.scaled(prefsdialog->size(), Qt::IgnoreAspectRatio);
    QPalette palette2;
    palette2.setBrush(QPalette::Background, bground2);
    prefsdialog->setPalette(palette2);
}

int MainWindow::startSetup()
{
    //Functions f;
    //Check if all the RTR stuff is present
    QString rtrcheck = launcherReqFilesCheck();
    if (rtrcheck!="yes")
    {
        QMessageBox rtrerror;
        rtrerror.critical(this,"Error", rtrcheck, QMessageBox::Close);
        rtrerror.setFixedWidth(200);
        return 404;
    }

    // Read launcher data
    OptionData *l = &listoptions;
    if (readLauncherData(l)==500)
        return 500;

    // EDU options
    QComboBox *eduComboBox = findChild<QComboBox*>("eduComboBox");
    setOptions(l, eduComboBox,"edu");

    // Campaign options
    QComboBox *campComboBox = findChild<QComboBox*>("campComboBox");
    setOptions(l, campComboBox, "camp");

    // Campaign options
    QComboBox *treesComboBox = findChild<QComboBox*>("treesComboBox");
    setOptions(l, treesComboBox, "trees");

    resetChecks();

    // Read player data
    readPlayerData(&playeroptionlist,this);

    return 0;
}

// ==================
// Slots:

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
    QString dataText = playerDataTextGen();
    bool goahead = true;

    if(this->prefsdialog->writePreferences(prefsdialog->preferencesText)==500)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::warning(this,"Continue launching?", "Your preferences could not be saved, and will be discarded if you launch the game.\n"\
                             "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }

    if(writePlayerData(dataText)==500)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::warning(this,"Continue launching?", "Your changes could not be saved, and will be discarded if you launch the game.\n"\
                             "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }

    QString errortext = "";
    // EDU switch
    QString comboboxtext = ui->eduComboBox->currentText();
    int eduswitch = filefolderSwitch(ui->eduComboBox->currentData().toString(), listoptions.getListOfEdus());
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
    int campswitch = filefolderSwitch(ui->campComboBox->currentData().toString(), listoptions.getListOfCamps());
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
    int treesswitch = filefolderSwitch(ui->treesComboBox->currentData().toString(), listoptions.getListOfTrees());
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
        confirm = QMessageBox::critical(this,"Error!:",errortext,QMessageBox::Yes|QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }
    //qDebug() << eduswitch;

    QStringList commands = this->setArguments();
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
            QMessageBox::critical(this,"Critical error", "The Rome Total War: Alexander executable was not found. Make sure you have installed RTR correctly,"\
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
        QMessageBox::information(this,"Success", "The map.rwm file was out of date and was successfully deleted.\nAn up-to-date file will be generated by the game at launch.", QMessageBox::Close);
        break;
    case 1:
        QMessageBox::critical(this,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR installation.", QMessageBox::Close);
        break;
    case 2:
        QMessageBox::information(this,"Success", "The map.rwm file was up-to-date.\nNo further action was needed.", QMessageBox::Close);
        break;
    case 3:
        QMessageBox::information(this,"Success", "The map.rwm file was not present.\nAn up-to-date file will be generated by the game at launch.", QMessageBox::Close);
        break;
    case 4:
        QMessageBox::critical(this,"Critical error", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                              "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close);
        break;
    }
}

void MainWindow::on_preferencesButton_clicked()
{
    readPreferences();
    prefsdialog->setChangesMadeFalse();
    prefsdialog->exec();
}

void MainWindow::on_saveButton_clicked()
{
    QString dataText = playerDataTextGen();
    if (writePlayerData(dataText)==0)
        QMessageBox::information(this,"Success", "Your settings have been successfully saved.", QMessageBox::Ok);
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

// ==================
// Private methods:

// Check file/folder existence methods:
// snippet taken from https://stackoverflow.com/a/26991243
bool MainWindow::fileExists(QString abspath)
{
    QFileInfo check_file(abspath);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

// snippet taken and adapted from https://stackoverflow.com/a/26991243
bool MainWindow::dirExists(QString abspath)
{
    QFileInfo check_file(abspath);
    // check if file exists and if yes: Is it really a directory and no file?
    return check_file.exists() && check_file.isDir();
}

// Assuming launcher will be in RTW/RTR/Launcher/
QString MainWindow::launcherReqFilesCheck()
{
    QString returnstring = "";
    if (!fileExists(QCoreApplication::applicationDirPath() + "/../../RomeTW-ALX.exe"))
        returnstring += "The Rome Total War: Alexander executable was not found. Make sure you have installed RTR correctly,"\
                    "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n";
    if (!dirExists(QCoreApplication::applicationDirPath() + "/../data"))
        returnstring += "The RTR data folder was not found. Make sure you have installed RTR correctly\n\n";
    if (!fileExists(QCoreApplication::applicationDirPath() + launcherDataFile))
        returnstring += "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed RTR correctly"\
                    " and not moved any files in the launcher folder.\n\n";
    //remove critical errors line
    //if (returnstring.isEmpty())
        returnstring = "yes";
    return returnstring;
}

// Read data methods:
int MainWindow::readLauncherData(OptionData *l)
{
    QFile launcherData(QCoreApplication::applicationDirPath() + launcherDataFile);
    if (launcherData.exists())
    {
        if (!launcherData.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this,"File not readable", "The data file could not be read. Please check your RTR installation and make sure the data"\
                                                         " is accessible.\n\nError: " + launcherData.errorString());
            return 500;
        }
        QTextStream in(&launcherData);
        in.setCodec("UTF-8");
        OptionObject o = {"","","","",""};
        QString warningtext="";
        uint linenumber=0;
        while (!in.atEnd())
        {
            linenumber++;
            QString line = in.readLine().simplified();
            if(line.split("//")[0]!="" && line!="" && line.split("//")[0].split(":").size()>1)
            {
                QStringList linedata = line.split("//")[0].split(":");
                if (linedata[1]=="" || linedata[1].split("|").size()!=4 || linedata[0]=="")
                    warningtext += QString("Error at line %1: argument number is not valid.\n").arg(linenumber);
                else
                {
                    if (linedata[0]!="dir" && linedata[0]!="file")
                        warningtext += QString("Error at line %1: the path type '%2' is invalid.\n").arg(linenumber).arg(linedata[0]);
                    else
                    {
                        QStringList objargs = linedata[1].split("|");
                        if (objargs[0]=="" || objargs[1]=="" || objargs[2]=="" || objargs[3]=="")
                            warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                        else
                        {

                            QString objpath = objargs[0].split("\\").join("/");
                            if (objpath.front()!="/")
                                objpath = QString("/" + objpath);
                            if (linedata[0]=="file" && objpath.split("/").size()<3)
                                warningtext += QString("Error at line %1: filepath is invalid (must be inside a folder inside /data/).\n").arg(linenumber);
                            else if (linedata[0]=="file" && objpath.split("/")[1]=="")
                                warningtext += QString("Error at line %1: filepath is invalid (must be inside a folder inside /data/).\n").arg(linenumber);
                            else if (objpath.split("//").size()>1)
                                warningtext += QString("Error at line %1: empty folder names are not allowed.\n").arg(linenumber);
                            else
                            {
                                if (objargs[3]!="edu" && objargs[3]!="camp" && objargs[3]!="trees")
                                    warningtext += QString("Error at line %1: invalid option type '%2'.\n").arg(linenumber).arg(objargs[3]);
                                else
                                {
                                    QDir objpathdir(QCoreApplication::applicationDirPath() + "/../data" + objpath);
                                    QDir datadir(QCoreApplication::applicationDirPath() + "/../data");
                                    if (objpathdir == datadir)
                                        warningtext += QString("Error at line %1: cannot use data folder as an option.\n").arg(linenumber);
                                    else
                                    {
                                        o = {linedata[0],objpathdir.absolutePath(),objargs[1],objargs[2],objargs[3]};
                                        //qDebug() << objpathdir.absolutePath();
                                        l->addObj(o);
                                    }
                                    //QMessageBox::information(0,"Debug info", objpathdir.absolutePath(), QMessageBox::Close);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (l->getListOfDirs().size()>0)
            foreach (OptionObject oo, l->getListOfDirs())
                if (!dirExists(oo.abspath))
                    warningtext += QString("Directory error: directory chosen for option '%1' cannot be found. Verify your RTR installation or your data file.\n").arg(oo.displayname);

        if (l->getListOfFiles().size()>0)
            foreach (OptionObject fo, l->getListOfFiles())
                if (!fileExists(fo.abspath))
                    warningtext += QString("File error: file chosen for option '%1' cannot be found. Verify your RTR installation or your data file.\n").arg(fo.displayname);

        if (!warningtext.isEmpty())
        {
            warningtext = "The following errors were found in the launcher data file:\n\n" + warningtext;
            QMessageBox::warning(this,"Launcher data error!", warningtext, QMessageBox::Close);
        }
        launcherData.close();

        return 0;
    }
    else
    {
        QMessageBox::critical(this,"Launcher data error!", "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed RTR correctly"\
                                                    " and not moved any files in the launcher folder.\n\n", QMessageBox::Close);
        return 500;
    }
}

void MainWindow::readPlayerData(QList<PlayerOption> *l, MainWindow *w)
{
    QString playerDataPath = QCoreApplication::applicationDirPath() + playerDataFile;
    QFile playerData(playerDataPath);
    if (fileExists(playerDataPath))
    {
        if (!playerData.open(QIODevice::ReadOnly))
            QMessageBox::critical(this,"File not readable", "The player data file could not be read. Please check your RTR installation and make sure the player data file"\
                                                         " is accessible.\n\nError: " + playerData.errorString());
        else
        {
            QTextStream in(&playerData);
            in.setCodec("UTF-8");
            PlayerOption o = {"",""};
            QString warningtext="";
            uint linenumber=0;
            while (!in.atEnd())
            {
                QString line = in.readLine().simplified();
                if(line.split("//")[0]!="" && line!="" && line.split("//")[0].split(":").size()>1)
                {
                    QStringList linedata = line.split("//")[0].split(":");
                    if (linedata[0]=="" || linedata[1]=="")
                        warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                    else
                    {
                        //qDebug() << linedata[0];
                        //qDebug() << w->findChild<QObject *>(linedata[0]);
                        if (w->findChild<QCheckBox *>(linedata[0]) || w->findChild<QComboBox *>(linedata[0]))
                        {
                            o = {linedata[0],linedata[1]};
                            l->append(o);
                            if (w->findChild<QCheckBox *>(linedata[0]))
                            {
                                QCheckBox *uiObject = w->findChild<QCheckBox *>(linedata[0]);
                                //QMessageBox::warning(0,"Preferences file errors!", uiObject->text(), QMessageBox::Close);
                                bool status = false;
                                if(linedata[1]=="1")
                                    status = true;
                                uiObject->setChecked(status);
                            }
                            else
                            {
                                QComboBox *uiObject = w->findChild<QComboBox *>(linedata[0]);
                                int indexOption = uiObject->findData(linedata[1]);
                                //qDebug() << indexOption;
                                if (linedata[1]!="default")
                                {
                                    if(indexOption == -1)
                                        warningtext += QString("Error at line %1: could not find option %2.\n").arg(linenumber).arg(linedata[1]);
                                    else
                                        uiObject->setCurrentIndex(indexOption);
                                }
                            }
                            //qDebug() << status;
                        }
                        else
                            warningtext += QString("Error at line %1: could not find option %2.\n").arg(linenumber).arg(linedata[0]);
                    }
                }
                linenumber++;
            }
            if (!warningtext.isEmpty())
            {
                warningtext = "The following errors were found in the player data file:\n\n" + warningtext;
                QMessageBox::warning(this,"Player data errors!", warningtext, QMessageBox::Close);
            }
            playerData.close();
        }
    }
    else
    {
        //qDebug() << "no player file";
        writePlayerData(defaultPlayerDat);
    }
}

QString MainWindow::playerDataTextGen()
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

void MainWindow::readPreferences()
{
    QVector<PlayerOption> maxresolutions;
    PlayerOption po = {"STRATEGY_MAX_RESOLUTION",maxres};
    maxresolutions.append(po);
    po.object = "BATTLE_MAX_RESOLUTION";
    maxresolutions.append(po);

    QString prefPath = QCoreApplication::applicationDirPath() + preferencesFile;
    QString prefFolder = QCoreApplication::applicationDirPath() + "/../preferences";
    QDir preferencesDir(prefFolder);
    if (!dirExists(prefFolder))
    {
        if(!preferencesDir.mkdir(prefFolder))
        {
            QMessageBox::critical(this,"Folder creation error", "The preferences folder is missing and could not be created. Please try again or create the folder '[RomeTW folder]/[RTR folder]/preferences/' manually.");
            prefsdialog->close();
            return;
        }
    }
    QFile prefData(prefPath);
    //qDebug() << prefPath;
    if (prefData.exists())
    {
        if (!prefData.open(QIODevice::ReadWrite))
            QMessageBox::critical(this,"File not readable", "The preferences file could not be read. Please check your RTR installation and make sure the preferences file"\
                                                         " is accessible.\n\nError: " + prefData.errorString());
        else
        {
            QString preferencesData = "";
            QTextStream in(&prefData);
            in.setCodec("UTF-8");
            QString warningtext="";
            uint linenumber=0;
            bool needsupdate = false;
            while (!in.atEnd())
            {
                QString line = in.readLine().simplified();
                if(line!="" && line.split(":").size()>1)
                {
                    QStringList linedata = line.split("//")[0].split(":");
                    if (linedata[0]=="" || linedata[1]=="")
                        warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                    else
                    {
                        //qDebug() << linedata[0];
                        //qDebug() << w->findChild<QObject *>(linedata[0]);
                        if (linedata[0]==maxresolutions.at(0).object || linedata[0]==maxresolutions.at(1).object)
                        {
                            preferencesData += linedata[0] + ":" + maxres + "\n";
                            if (linedata[1]!=maxres)
                                needsupdate = true;
                        }
                        else
                        {
                            //qDebug() << linedata[0];
                            if (prefsdialog->findChild<QCheckBox *>(linedata[0]))
                            {
                                QCheckBox *uiObject = prefsdialog->findChild<QCheckBox *>(linedata[0]);
                                QList<bool> modifiers = prefsdialog->getMods();
                                QList<QCheckBox *> listofboxes = prefsdialog->getListPrefs();
                                int i=0;
                                bool status = false;
                                if(linedata[1]=="TRUE")
                                    status = true;
                                bool moddedstatus=false;
                                foreach (QCheckBox *cb, listofboxes)
                                {
                                    if (cb->objectName()==uiObject->objectName())
                                        moddedstatus = status == modifiers.at(i);
                                        i++;
                                }
                                uiObject->setChecked(moddedstatus);
                            }
                            //qDebug() << status;
                            preferencesData += line + "\n";
                        }
                    }
                }
                else
                    preferencesData += line + "\n";
            }
            if (needsupdate)
            {
                prefData.resize(0);
                QTextStream out(&prefData);
                in.setCodec("UTF-8");
                out << preferencesData;
            }
            prefsdialog->preferencesText = preferencesData;
            //qDebug() << p->preferencesText;
            //qDebug() << preferencesData;
            if (!warningtext.isEmpty())
            {
                warningtext = "The following errors were found in the player data file:\n\n" + warningtext;
                QMessageBox::warning(this,"Preferences file errors!", warningtext, QMessageBox::Close);
            }
            prefData.close();
        }
    }
    else
    {
        //qDebug() << "no preferences file";
        prefsdialog->writePreferences(defaultPreferencesData);
    }
}

void MainWindow::setOptions(OptionData *l, QComboBox *comboBoxObject, QString optiontypelocal)
{
    QString defaultname = "Default";
    if (optiontypelocal=="camp")
        defaultname = "RTR Main Campaign";
    if (l->getListByOptionType(optiontypelocal).size()==0)
    {
        comboBoxObject->addItem(defaultname,"default");
        comboBoxObject->setEnabled(false);
    }
    else
    {
        foreach (OptionObject optionobj, l->getListByOptionType(optiontypelocal))
        {
            comboBoxObject->addItem(optionobj.displayname,optionobj.optioncode);
        }
    }
}

// Checking data methods:
bool MainWindow::checkDefaults(Ui::MainWindow *ui)
{
    bool checked = true;
    foreach (QCheckBox *c, checkboxeslist)
    {
        if (c->objectName()!="showerrBox")
                checked = checked && !c->checkState();
    }
    checked = checked && ui->showerrBox->checkState();
    return checked;
}

int MainWindow::checkMapRwm()
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

// Reset options methods:
void MainWindow::resetChecks()
{
    foreach (QCheckBox *c, checkboxeslist)
    {
        c->setChecked(false);
    }
    ui->showerrBox->setChecked(true);
}

// Write data and launch game methods.
int MainWindow::writePlayerData(QString dataToWrite)
{
    QFile playerData(QCoreApplication::applicationDirPath() + playerDataFile);
    if (!playerData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(this,"File not writeable", "The player data file could not be written. Please check your RTR installation and make sure the player data file"\
                                                     " is accessible.\n\nError: " + playerData.errorString());
        return 500;
    }
    else
    {
        QTextStream out(&playerData);
        out.setCodec("UTF-8");
        out << dataToWrite;
        playerData.close();
        return 0;
    }
}

int MainWindow::filefolderSwitch(QString currentoption, QList<OptionObject> options)
{
    if (currentoption=="default")
        return 0;
    else
    {
        int indexfolder = -1;
        int index = 0;
        foreach (OptionObject option, options)
        {
            if (option.optioncode==currentoption && indexfolder==-1)
            {
                indexfolder = index;
            }
            index++;
        }
        if (indexfolder!=-1)
        {
            OptionObject o = options.at(indexfolder);
            //qDebug() << o.pathtype;
            if (o.pathtype=="dir")
            {
                QDir data(QCoreApplication::applicationDirPath() + "/../data");
                //qDebug() << data.absolutePath();
                if (copyRecursively(o.abspath,data.absolutePath()))
                    return 0;
                else
                    return 1;
                    //messagetext += QString("The folder of the game type '%1' could not be copied.\n").arg(o.displayname);
            }
            else
            {
                if (fileExists(o.abspath))
                {
                    QFileInfo filecopyinfo(o.abspath);
                    QFile filecopy(o.abspath);
                    QFileInfo originalfile(QCoreApplication::applicationDirPath() + "/../data/" + filecopyinfo.fileName());
                    QString originalfilepath(originalfile.absoluteFilePath());
                    //qDebug() << edufileinfo.filePath();
                    QFileInfo file(originalfilepath);
                    if (file.exists())
                    {
                        QFile filetorm(originalfilepath);
                        filetorm.remove();
                    }
                    if (filecopy.copy(originalfilepath))
                        return 0;
                    else
                        //qDebug() << filecopy.errorString();
                        return 2;
                        //messagetext += QString("The file for the game type '%1' could not be copied.\n").arg(o.displayname);
                }
            }
        }
        return 3;
        //messagetext += QString("The files for the game type '%1' could not be found.\n").arg(ui->eduComboBox->currentText().toString());
    }


    //return messagetext;
}

// function taken from https://forum.qt.io/topic/59245/is-there-any-api-to-recursively-copy-a-directory-and-all-it-s-sub-dirs-and-files/3
bool MainWindow::copyRecursively(QString sourceFolder, QString destFolder)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if(!sourceDir.exists())
        return false;

    QDir destDir(destFolder);
    if(!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);
    for(int i = 0; i< files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        QFileInfo file(destName);
        if (file.exists())
        {
            QFile filetorm(destName);
            filetorm.remove();
        }
        success = QFile::copy(srcName, destName);
        if(!success)
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for(int i = 0; i< files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = copyRecursively(srcName, destName);
        if(!success)
            return false;
    }

    return true;
}

QStringList MainWindow::setArguments()
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
            QMessageBox::critical(this,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR installation.", QMessageBox::Close);
            QMessageBox::StandardButton confirm;
            confirm = QMessageBox::warning(this,"Continue launching?", "The map file could not be deleted and will be out of date.\n"\
                                 "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
            if (confirm == QMessageBox::No)
                argList[0] = "nope";
            break;
        case 4:
            QMessageBox::critical(this,"Critical error", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                                  "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close);
                argList[0] = "nope";
            break;
        }
    }

    return argList;
}
