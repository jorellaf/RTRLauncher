#include "functions.h"

//#ifndef DATAHEADER_H
//#include "dataheader.h"
//#endif

#include <QApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <QDir>
#include <QComboBox>
#include <QCheckBox>
#include <QDebug>

// snippet taken from https://stackoverflow.com/a/26991243
bool Functions::fileExists(QString path)
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

// snippet taken and adapted from https://stackoverflow.com/a/26991243
bool Functions::dirExists(QString path)
{
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a directory and no file?
    return check_file.exists() && check_file.isDir();
}

// Assuming launcher will be in RTW/RTR/Launcher/
QString Functions::rtrPresent()
{
    QString rtrcheck = "";
    if (!fileExists(QCoreApplication::applicationDirPath() + "/../../RomeTW-ALX.exe"))
        rtrcheck += "The Rome Total War: Alexander executable was not found. Make sure you have installed RTR correctly,"\
                    "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n";
    if (!dirExists(QCoreApplication::applicationDirPath() + "/../data"))
        rtrcheck += "The RTR data folder was not found. Make sure you have installed RTR correctly\n\n";
    if (!fileExists(QCoreApplication::applicationDirPath() + launcherDataFile))
        rtrcheck += "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed RTR correctly"\
                    " and not moved any files in the launcher folder.\n\n";
    if (rtrcheck.isEmpty())
        rtrcheck = "yes";
    return rtrcheck;
}

int Functions::readLauncherData(ListOptionObjects *l)
{
    QFile launcherData(QCoreApplication::applicationDirPath() + launcherDataFile);
    if (launcherData.exists())
    {
        if (!launcherData.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(0,"File not readable", "The data file could not be read. Please check your RTR installation and make sure the data"\
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
            QMessageBox::warning(0,"Launcher data error!", warningtext, QMessageBox::Close);
        }
        launcherData.close();

        return 0;
    }
    else
    {
        QMessageBox::critical(0,"Launcher data error!", "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed RTR correctly"\
                                                    " and not moved any files in the launcher folder.\n\n", QMessageBox::Close);
        return 500;
    }
}

void Functions::setOptions(ListOptionObjects *l, QComboBox *comboBoxObject, QString optiontypelocal)
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

//void Functions::setCampOptions(ListOptionObjects *l, QComboBox *campComboBox)
//{
//    if (l->getListOfCamps().size()==0)
//    {
//        campComboBox->addItem("RTR Main Campaign","default");
//        campComboBox->setEnabled(false);
//    }
//    else
//    {
//        foreach (OptionObject optionobj, l->getListOfCamps())
//            campComboBox->addItem(optionobj.displayname,optionobj.optioncode);
//    }
//}

int Functions::writePlayerData(QString dataToWrite)
{
    QFile playerData(QCoreApplication::applicationDirPath() + playerDataFile);
    if (!playerData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(0,"File not writeable", "The player data file could not be written. Please check your RTR installation and make sure the player data file"\
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

void Functions::readPlayerData(QList<PlayerOption> *l, MainWindow *w)
{
    QString playerDataPath = QCoreApplication::applicationDirPath() + playerDataFile;
    QFile playerData(playerDataPath);
    if (fileExists(playerDataPath))
    {
        if (!playerData.open(QIODevice::ReadOnly))
            QMessageBox::critical(0,"File not readable", "The player data file could not be read. Please check your RTR installation and make sure the player data file"\
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
                QMessageBox::warning(0,"Player data errors!", warningtext, QMessageBox::Close);
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

// function taken from https://forum.qt.io/topic/59245/is-there-any-api-to-recursively-copy-a-directory-and-all-it-s-sub-dirs-and-files/3
bool Functions::copyRecursively(QString sourceFolder, QString destFolder)
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

int Functions::filefolderSwitch(QString currentoption, QList<OptionObject> options)
{
    Functions f;
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
                if (f.copyRecursively(o.abspath,data.absolutePath()))
                    return 0;
                else
                    return 1;
                    //messagetext += QString("The folder of the game type '%1' could not be copied.\n").arg(o.displayname);
            }
            else
            {
                if (f.fileExists(o.abspath))
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

int Functions::writePreferences(QString dataToWrite)
{
    QFile preferencesData(QCoreApplication::applicationDirPath() + preferencesFile);
    if (!preferencesData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(0,"File not writeable", "The preferences file could not be written. Please check your RTR installation and make sure the preferences file"\
                                                     " is accessible.\n\nError: " + preferencesData.errorString());
        return 500;
    }
    else
    {
        QTextStream out(&preferencesData);
        out.setCodec("UTF-8");
        out << dataToWrite;
        preferencesData.close();
        return 0;
    }
}

void Functions::readPreferences(PreferencesDialog *p)
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
            QMessageBox::critical(0,"Folder creation error", "The preferences folder is missing and could not be created. Please try again or create the folder '[RomeTW folder]/[RTR folder]/preferences/' manually.");
            p->close();
            return;
        }
    }
    QFile prefData(prefPath);
    //qDebug() << prefPath;
    if (prefData.exists())
    {
        if (!prefData.open(QIODevice::ReadWrite))
            QMessageBox::critical(0,"File not readable", "The preferences file could not be read. Please check your RTR installation and make sure the preferences file"\
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
                            if (p->findChild<QCheckBox *>(linedata[0]))
                            {
                                QCheckBox *uiObject = p->findChild<QCheckBox *>(linedata[0]);
                                QList<bool> modifiers = p->getMods();
                                QList<QCheckBox *> listofboxes = p->getListPrefs();
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
            p->preferencesText = preferencesData;
            //qDebug() << p->preferencesText;
            //qDebug() << preferencesData;
            if (!warningtext.isEmpty())
            {
                warningtext = "The following errors were found in the player data file:\n\n" + warningtext;
                QMessageBox::warning(0,"Preferences file errors!", warningtext, QMessageBox::Close);
            }
            prefData.close();
        }
    }
    else
    {
        //qDebug() << "no preferences file";
        writePreferences(defaultPreferencesData);
    }
}
