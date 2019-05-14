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

    // Store all the objects of the main window's settings checkboxes in the QList of QCheckBoxes variable.
    checkboxeslist = {ui->showerrBox, ui->neBox, ui->nmBox, ui->multirunBox, ui->editorBox, ui->moviecamBox, ui->naBox, ui->aiBox, ui->spritesBox, ui->swBox, ui->maprwmBox};

    // Snippet by user 'Barracuda' from https://stackoverflow.com/a/23227915.
    // Set an image as the background of the main launcher window.
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

    // A new instance of the preferences window to the variable in the MainWindow class as a child of this main window.
    prefsdialog = new PreferencesDialog(this);

    // Snippet by user 'Barracuda' from https://stackoverflow.com/a/23227915.
    // Set an image as the background of the preferences editor window.
    QPixmap bgroundprefs(":/images/marbletexture.png");
    bgroundprefs = bgroundprefs.scaled(prefsdialog->size(), Qt::IgnoreAspectRatio);
    QPalette paletteprefs;
    paletteprefs.setBrush(QPalette::Background, bgroundprefs);
    prefsdialog->setPalette(paletteprefs);
    // End of snippet.
}

// Method for the initial loading of variables from the player data file and set-up of previously selected options.
// Returns an integer success/error code.
int MainWindow::startSetup()
{
    //Check if all the required files (and folders) are present.
    QString reqfilescheck = launcherReqFilesCheck();
    // If some of the required files and folders are not present (the return string is not 'y'), then show a critical
    // error message, and return a specific error code.
    if (reqfilescheck != "y")
    {
        // Create the QMessageBox object we will display to the user.
        QMessageBox reqserror;
        // Make it a critical error template, with the return string of the launcherReqFilesCheck method as the body,
        // since it returns the string describing the error(s), and a single 'close' button.
        reqserror.critical(this, "Error", reqfilescheck, QMessageBox::Close, QMessageBox::Close);
        // Set a fixed with, otherwise it will be a thousand pixels wide to show each line without a carriage return.
        reqserror.setFixedWidth(200);
        // Return an error code 404, very cleverly referencing the HTTP error for file not found.
        return 404;
    }

        // Read launcher data:
    // Integer to store the return code of the readLauncherData method and call the method with the reference of the
    // launcheroptionslist object to allow adding options to the list from within the readLauncherData method.
    int launcherdatacheck = readLauncherData(&launcheroptionslist);
    // If the return code is not 0 for 'all ok', return an error code to the caller.
    if (launcherdatacheck != 0)
        return launcherdatacheck;

    // Fill combobox of campaign options with the data from the launcher data file.
    // Find the QComboBox object for the campaign options combobox.
    QComboBox *campComboBox = this->findChild<QComboBox*>("campComboBox");
    // Call the setOptions method with the reference to our launcheroptionslist object containing the options from the
    // launcher data file, the QComboBox to be filled, the string of the option type used in the launcher.dat file, and
    // a string to override the name of the default option to be slightly more descriptive.
    setOptions(&launcheroptionslist, campComboBox, "camp", "RTR Main Campaign");

    // Fill combobox of unit roster (export_descr_unit.txt, EDU) options with the data from the launcher data file.
    QComboBox *eduComboBox = this->findChild<QComboBox*>("eduComboBox");
    // See previous setOptions call comment (without default name override).
    setOptions(&launcheroptionslist, eduComboBox, "edu");

    // Fill combobox of trees options with the data from the launcher data file.
    QComboBox *treesComboBox = this->findChild<QComboBox*>("treesComboBox");
    // See previous setOptions call comment.
    setOptions(&launcheroptionslist, treesComboBox, "trees");

    // Pre-emptively reset all the checkboxes to defaults before loading the saved player data, in case of an error.
    resetChecks();

    // Read player data.
    // Since failing to load the previous settings is not critical, we do not return a value here, as any errors that
    // come up are handled within the method.
    readPlayerData();

    // We can only get here if there were no errors (and therefore no returns), so return an all ok code 0.
    return 0;
}

// ==================
// Slots:

// When the launch button is pressed down:
void MainWindow::on_launchButton_pressed()
{
    // Change the icon of the button from the regular mod icon to the highlighted button.
    // Create a QIcon object, and add a QPixmap to it (an image object optimised by Qt for displaying on-screen).
    QIcon i;
    i.addPixmap(QPixmap(":/images/rtr_pressed.png"));
    // Set the icon of the launcher button to the QIcon object created with the appropriate image.
    ui->launchButton->setIcon(i);
}

// When the launch button is released:
void MainWindow::on_launchButton_released()
{
    // Change the icon of the button back to the regular one.
    QIcon i;
    i.addPixmap(QPixmap(":/images/rtr.png"));
    // See on_launchButton_pressed slot method.
    ui->launchButton->setIcon(i);
}

// When the launch button is clicked (engaged, which is different from pressed, as the button can be pressed down and
// held, but not 'activated') (does not necessarily have to be clicked, but can also be activated with the keyboard):
void MainWindow::on_launchButton_clicked()
{
    // Disable the launcher button as soon as it is clicked to prevent accidentally launching the game multiple times.
    ui->launchButton->setEnabled(false);

    // Bool variable to let the launcher know it should go ahead with the launching of the game.
    // Defaulted to true, made false in case there is an error on the way.
    bool goahead = true;

    // Saving preferences from the main window doesn't seem like the best idea. Best left in the preferences window.
    /*if(prefsdialog->writePreferences(prefsdialog->preferencesText)==400)
    {
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::warning(this,"Continue launching?", "Your preferences could not be saved, and will be discarded if you launch the game.\n"\
                             "Would you like to continue launching the game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (confirm == QMessageBox::No)
            goahead = false;
    }*/

    // Generate the player data file contents with the user-selected settings in the launcher.
    QString dataText = playerDataTextGen();

    // Try to save the player data file with the QString we generated above and store the returned error string.
    QString errstring = writePlayerData(dataText);

    // If writing the player data file was not successful (did not return an empty error string), then:
    if(errstring != "")
    {
        // Make a QMessageBox StandardButton object that will store the button the user pressed.
        QMessageBox::StandardButton confirm;
        // Create a message box asking the player to confirm if they want to continue launching the game even though
        // their settings could not be saved, with the default button (when pressing enter) being no.
        confirm = QMessageBox::warning(this,"Continue launching?", errstring + "\n\nYour configuration could not be saved, and will be discarded if you launch the game.\n"\
                             "Do you wish to continue launching the game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        // If the button pressed was the 'No' button:
        if (confirm == QMessageBox::No)
            // Do not go ahead with launching the game.
            goahead = false;
    }

    // String to store the error text if any of the switching operations for changing the options failed.
    QString errortext = "";

    // TODO: Refactor the Player Option into its own class, with its own getter methods and make a comparison here between selected option and previous option.
    // Append the string returned by the file/folder switch method (error text or nothing) to the errortext variable,
    // which we call by sending the desired combobox and OptionData object with the desired option type.
    // EDU switch:
    errortext += filefolderSwitch(ui->eduComboBox, launcheroptionslist.getListOfEdus());
    // Campaign switch
    errortext += filefolderSwitch(ui->campComboBox, launcheroptionslist.getListOfCamps());
    // Trees switch
    errortext += filefolderSwitch(ui->treesComboBox, launcheroptionslist.getListOfTrees());

    // If the errortext variable is not empty (i.e. there was an error along the way), then warn the player.
    if (!errortext.isEmpty())
    {
        // Add a final piece of information to the error text.
        errortext += "\nAlthough the game may run normally, it is likely your desired configuration will\nnot be in effect, and the game might not even start correctly\n\n"\
                "Do you wish to continue launching the game?";
        // See above.
        QMessageBox::StandardButton confirm;
        confirm = QMessageBox::critical(this,"Error!", errortext, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        // If the button pressed was the 'No' button:
        if (confirm == QMessageBox::No)
            // Do not go ahead with launching the game.
            goahead = false;
    }

    // List of strings storing all the command line arguments that will be sent when launched, and filling the
    // QStringList object with the selected options by calling the setArguments method.
    QStringList commands = setArguments();

    // If we have arrived all the way to here without any errors (goahead == true), and if the returned list of
    // commands is not just 'n' (do not proceed) then proceed to take-off.
    if (goahead == true && commands[0]!="n")
    {
        // QFileInfo object for the RTW executable to launch, assuming the user has correctly installed the launcher in
        // [RTW/MODFOLDER/Launcher]. Path has to be absolute, otherwise it does not work.
        QFileInfo rtwexec(QCoreApplication::applicationDirPath() + "/../../RomeTW-ALX.exe");
        // Check if the game executable can be found, is actually a file, and is executable, just to be sure.
        if (rtwexec.exists() && rtwexec.isFile() && rtwexec.isExecutable())
        {
            // Store the absolute file path of the executable file.
            QString execabspath = rtwexec.absoluteFilePath();

            // Previous failed attempts at launching the game with the command line arguments.
            /* file += commandlineswitches;
            process->setNativeArguments("-mod:X"); */

            // Snippet by user 'tomvodi' from https://stackoverflow.com/a/19442886.
            // Launch the game with the command line arguments requested by the user, as well as the mod name, making
            // sure the game is not closed when the launcher is closed.
            QProcess *process = new QProcess(this);
            if (process->startDetached(execabspath,commands,QCoreApplication::applicationDirPath() + "/../.."))
                // Close the launcher by quitting the application
                // (though it would be better if it returned 0 to the main, but alas).
                QApplication::quit();
            else
                // If the executable could not be launched, alert the user of the problem with a message box.
                QMessageBox::critical(this,"Critical error!", "The Rome: Total War - Alexander executable could not be launched. Make sure you have installed Rome: Total War correctly,"\
                                                             "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.",\
                                      QMessageBox::Close, QMessageBox::Close);
        }
        else
        {
            // If the executable was not found, alert the user of the problem with a message box.
            QMessageBox::critical(this,"Critical error!", "The Rome: Total War - Alexander executable was not found. Make sure you have installed RTR correctly,"\
                                                         "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.",\
                                  QMessageBox::Close, QMessageBox::Close);
        }
    }

    // Finally, re-enable the launch button to let the user try again if there was an issue (launcher did not close).
    ui->launchButton->setEnabled(true);
}

// Slot for the map.rwm checker button.
void MainWindow::on_mapButton_clicked()
{
    // Check if the map.rwm file is up-to-date, and delete it to have the game generate an updated one if necessary.
    checkMapRwm();
}

// Slot for the button to show the editor of the mod's RTW preferences.txt file.
void MainWindow::on_preferencesButton_clicked()
{
    // Read the mod's RTW preferences file every time the dialog is to be displayed, so it gets updated every time.
    readPreferences();
    // Since we are reading the data from the preferences file every time it is started, no changes have been made.
    prefsdialog->setChangesMadeFalse();

    // Show the preferences editor dialog as a modal window.
    prefsdialog->exec();
}

// Slot for the save settings button.
void MainWindow::on_saveButton_clicked()
{
    // Generate the player data file contents with the user-selected settings in the launcher.
    QString dataText = playerDataTextGen();

    // Try to save the player data file with the QString we generated above and store the returned error string.
    QString errstring = writePlayerData(dataText);

    // If writing the player data file was successful (returned an empty error string), inform the player.
    if (errstring == "")
        QMessageBox::information(this,"Success", "Your settings have been successfully saved.", QMessageBox::Ok, QMessageBox::Ok);
    // If it failed, send a warning with the error details as the message dialog body.
    else
        QMessageBox::warning(this,"File not writeable", errstring, QMessageBox::Ok, QMessageBox::Ok);
}

// Slot for the discard settings button.
void MainWindow::on_discardButton_clicked()
{
    // Create a QMessageBox StandardButton object that will store the button the user pressed.
    QMessageBox::StandardButton confirm;
    // TODO: Check for changes for prompting.
    // Create a message box asking the player to confirm if they want to discard their changes and exit.
    confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to exit?\nAll your unsaved changes will be lost.",
                                   QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    // If the user chose yes, then exit the application.
    if (confirm == QMessageBox::Yes)
        QApplication::quit();
}

// Slot for the restore defaults button.
void MainWindow::on_defaultsButton_clicked()
{
    // If the checkboxes are not in their default states.
    if (!checkDefaults())
    {
        // Create a QMessageBox StandardButton object that will store the button the user pressed.
        QMessageBox::StandardButton confirm;
        // Create a message box asking the player to confirm if they want to discard their changes and restore defaults.
        confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to reset to the default settings?\nYour previous settings will still be stored in your\ndata file unless you launch the game or manually save.",
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        // If the user chose yes, then reset the checkboxes to defaults.
        if (confirm == QMessageBox::Yes)
            resetChecks();
    }
}

// ==================
// Private methods:

// Check file/folder existence methods:

// Method to check if a file with the specified absolute path exists and is indeed a file.
bool MainWindow::fileExists(QString abspath)
{
    // Snippet by user 'mozzbozz' from https://stackoverflow.com/a/26991243
    // A QFileInfo object needs an absolute path, and will not work with a relative path.
    QFileInfo checkfile(abspath);
    // Return the boolean result of whether the file exists and whether it is really a file.
    return checkfile.exists() && checkfile.isFile();
    // End snippet.
}

// Method to check if a directory with the specified absolute path exists and is indeed a directory.
bool MainWindow::dirExists(QString abspath)
{
    // A QFileInfo object needs an absolute path, and will not work with a relative path.
    QFileInfo checkdir(abspath);
    // Return the boolean result of whether the file exists and whether it is really a directory.
    return checkdir.exists() && checkdir.isDir();
}

// Method to check if all the requirements for running the mod successfully are present.
QString MainWindow::launcherReqFilesCheck()
{
    // An empty string that will store any error descriptions if any come up.
    QString returnstring = "";

    // Assuming launcher will be located in [RTW/MODFOLDER/Launcher/]:
    // If no game executable is not found, add an error message to the string. Note that fileExists() and dirExists()
    // both require the file path parameter to be an absolute path.
    if (!fileExists(QCoreApplication::applicationDirPath() + "/../../RomeTW-ALX.exe"))
        returnstring += "The Rome: Total War - Alexander executable was not found. Make sure you have installed RTR correctly,"\
                    "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n";

    // If the mod data folder was not found, add an error message to the string.
    if (!dirExists(QCoreApplication::applicationDirPath() + "/../data"))
        returnstring += "The RTR data folder was not found. Make sure you have installed RTR correctly\n\n";

    // If the launcher data file was not found, add an error message to the string.
    if (!fileExists(QCoreApplication::applicationDirPath() + launcherDataFilePath))
        returnstring += "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed RTR correctly"\
                    " and not moved any files in the launcher folder.\n\n";

    // If no error was recorded, then set the string to 'y' to signal that all the required files and dirs were found.
    // NOTE: Comment out next line to avoid critical error dialog blocking the launcher when testing.
    if (returnstring.isEmpty())
        returnstring = "y";

    // Return whatever was in the return string.
    return returnstring;
}

// Read data methods:
// Method to read the launcher data file.
// Require an OptionData class where all the launcher options will be stored
int MainWindow::readLauncherData(OptionData *l)
{
    // Generate the absolute file path for the launcher data file using the relative path defined in optiondata.h.
    QString launcherdatafilepath = QCoreApplication::applicationDirPath() + launcherDataFilePath;

    // Check if the data file exists.
    if (fileExists(launcherdatafilepath))
    {
        // If it does, create a QFile object pointing to the absolute path of the file.
        QFile launcherdata(launcherdatafilepath);

        // Try to open it, and if it doesn't, send a critical error and return an error code.
        if (!launcherdata.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this,"File not readable", "The data file could not be read. Please check your RTR installation and make sure the data"\
                                                         " is accessible.\n\nError: " + launcherdata.errorString());
            // Very cleverly referencing the HTTP error code for Bad Request.
            return 400;
        }

        // Otherwise, create a QTextStream interface 'in' with a reference to the launcher data file.
        QTextStream in(&launcherdata);
        // Set the encoding to UTF-8 for less limited script support.
        in.setCodec("UTF-8");

        // Initialise an OptionObject struct to use for appending options to the list of options.
        OptionObject o = {"","","","",""};

        // Create a QString to store any possible errors and warnings that come up when reading the file.
        QString warningtext="";
        // And an unsigned line number counter to keep track of the line in which the errors and warnings were found.
        uint linenumber=0;

            // Variables used in the file reading loop:
        // A QString to store the contents of each line.
        QString line = "";
        // A list of QStrings to store the different data elements of each option in the file, the type of path (file
        // or folder) and then the rest.
        QStringList linedata = {""};
        // A list of QStrings to store the elements of each option (file/folder path, code, name, type).
        QStringList objargs = {""};
        // QString to store the file/folder path of each option.
        QString objpath = "";

        // While we are still not at the end of the file:
        while (!in.atEnd())
        {
            // Read the next line and store it in 'line', using simplified() to collapse any whitespace (such as tabs)
            // to a single space, and then add one to the linenumber counter.
            line = in.readLine().simplified();
            linenumber++;

            // Check if the line is not empty, and, if commented with '//', there is something behind the comment, and,
            // when the line behind the comment (if any) has at least one ':' character, then:
            if(line != "" && line.split("//")[0] != "" && line.split("//")[0].split(":").size() > 1)
            {
                // Split the data behind any comment character ('//') by the character ':'.
                linedata = line.split("//")[0].split(":");

                // Check if there is nothing after the ':', or if what follows is not split in four elements
                // (path|code|name|type) by the '|' divider, or if there is nothing behind the ':' character:
                if (linedata[1] == "" || linedata[1].split("|").size() != 4 || linedata[0] == "")
                    // If so, add a warning to the warning string.
                    warningtext += QString("Error at line %1: argument number is not valid.\n").arg(linenumber);
                else
                {
                    // If not, then check if what's behind the ':' is not any of the two allowed options:
                    if (linedata[0] != "dir" && linedata[0] != "file")
                        // If so, add a warning to the warning string.
                        warningtext += QString("Error at line %1: the path type '%2' is invalid.\n").arg(linenumber).arg(linedata[0]);
                    else
                    {
                        // If not, split the data after the ':' by the '|' dividing character.
                        objargs = linedata[1].split("|");

                        // Check if any of the four elements is not empty:
                        if (objargs[0]=="" || objargs[1]=="" || objargs[2]=="" || objargs[3]=="")
                            // If so, add a warning to the warning string.
                            warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                        else
                        {
                            // If not, get the object path, and convert any backslash (has to be escaped with another
                            // one here) to a forward slash, to allow for the use of either type.
                            objpath = objargs[0].split("\\").join("/");

                            // If there is no forward slash at the front of the path in the file, add one, as it is
                            // required in order to create the path for the Qt file/folder objects.
                            if (objpath.front() != "/")
                                objpath = QString("/" + objpath);

                            // Check if, for files, there are fewer than two forward slashes in the path (/x/file.y).
                            if (linedata[0] == "file" && objpath.split("/").size() < 3 )
                                // If so, add a warning to the warning string, since this would be invalid.
                                warningtext += QString("Error at line %1: file path is invalid (must be inside a folder inside /data/).\n").arg(linenumber);
                            // Else, check if the file name at the end of the path is an empty string.
                            else if (linedata[0] == "file" && objpath.split("/").last() == "")
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: file name is invalid.\n").arg(linenumber);
                            // Else, check if there are any empty folder sequences ('//') for either files or folders.
                            else if (objpath.split("//").size()>1)
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: empty folder names are not allowed. If you used '//' as a divider, please change it to '/' or '\'.\n")\
                                        .arg(linenumber);
                            // Else, check if the user tried to go up a level in the folder tree with '..'.
                            else if (objpath.split("..").size()>1)
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: you may not go a level higher in the folder tree.\n").arg(linenumber);
                            else
                            {
                                // TODO: Refactor and make expansible.
                                // If none of these issues were detected, check if the option type is not any of the
                                // allowed options.
                                if (objargs[3] != "edu" && objargs[3] != "camp" && objargs[3] != "trees")
                                    // If so, add a warning to the warning string.
                                    warningtext += QString("Error at line %1: invalid option type '%2'.\n").arg(linenumber).arg(objargs[3]);
                                else
                                {
                                    // If no errors have been detected so far, begin properly parsing the option data.
                                    // Create a QDir object to generate the absolute path for the option file/folder.
                                    QDir objpathdir(QCoreApplication::applicationDirPath() + "/../data" + objpath);

                                    /*QDir datadir(QCoreApplication::applicationDirPath() + "/../data");
                                    if (objpathdir == datadir)
                                        warningtext += QString("Error at line %1: cannot use data folder as an option.\n").arg(linenumber);
                                    else
                                    {*/

                                    // Add the data into an OptionObject struct.
                                    o = {linedata[0],objpathdir.absolutePath(),objargs[1],objargs[2],objargs[3]};
                                    // Add the OptionObject into the OptionData list.
                                    l->addObj(o);
                                }
                            }
                        }
                    }
                }
            }
        }

        // If we have any options using directories, go through them and check if the directories chosen exist:
        if (l->getListOfDirs().size()>0)
            foreach (OptionObject oo, l->getListOfDirs())
                if (!dirExists(oo.abspath))
                    // If a directory cannot be found, add a warning to the QString.
                    warningtext += QString("Directory error: directory chosen for option '%1' cannot be found. Verify your RTR installation or your launcher data file.\n").arg(oo.displayname);

        // If we have any options using files, go through them and check if the files chosen exist:
        if (l->getListOfFiles().size()>0)
            foreach (OptionObject fo, l->getListOfFiles())
                if (!fileExists(fo.abspath))
                    // If a file cannot be found, add a warning to the QString.
                    warningtext += QString("File error: file chosen for option '%1' cannot be found. Verify your RTR installation or your launcher data file.\n").arg(fo.displayname);

        // Check if the warning string is not empty:
        if (!warningtext.isEmpty())
        {
            // If so, add a small introduction behind the list of errors, and display a warning to the user.
            warningtext = "The following errors were found in the launcher data file:\n\n" + warningtext;
            QMessageBox::warning(this,"Launcher data error!", warningtext, QMessageBox::Close, QMessageBox::Close);
        }

        // Close the QFile since we finished using it.
        launcherdata.close();

        // Since any errors detected so far are not application-breaking, return an all ok '0'.
        return 0;
    }

    // If the file was not found, send a critical error.
    else
    {
        QMessageBox::critical(this,"Launcher data error!", "Could not find the data file for the launcher (launcher.dat) in the launcher folder."\
                                                           " Check that you have installed RTR correctly and not moved any files in the launcher folder.\n\n",\
                              QMessageBox::Close, QMessageBox::Close);
        // Return an error code 404, very cleverly referencing the HTTP error for file not found.
        return 404;
    }
}

// Method to read the player data file.
void MainWindow::readPlayerData()
{
    // Generate the absolute file path for the player data file using the relative path defined in optiondata.h.
    QString playerdatapath = QCoreApplication::applicationDirPath() + playerDataFilePath;

    // Check if the data file exists.
    if (fileExists(playerdatapath))
    {
        // If it does, create a QFile object pointing to the absolute path of the file.
        QFile playerdatafile(playerdatapath);

        // Try to open it, and if it doesn't, send a critical error.
        if (!playerdatafile.open(QIODevice::ReadOnly))
            QMessageBox::critical(this,"File not readable", "The player data file could not be read. Please check your RTR installation and make sure the player data file"\
                                                         " is accessible.\n\nError: " + playerdatafile.errorString(), QMessageBox::Close, QMessageBox::Close);

        // Otherwise, create a QTextStream interface 'in' with a reference to the launcher data file.
        QTextStream in(&playerdatafile);
        // Set the encoding to UTF-8 for less limited script support.
        in.setCodec("UTF-8");

        // Initialise an PlayerOption struct to use for appending options to the list of options.
        PlayerOption o = {"",""};

        // Create a QString to store any possible errors and warnings that come up when reading the file.
        QString warningtext="";
        // And an unsigned line number counter to keep track of the line in which the errors and warnings were found.
        uint linenumber=0;

            // Variables used in the file reading loop:
        // A QString to store the contents of each line.
        QString line = "";
        // A list of QStrings to store the different data elements of each option in the file, the type of path (file
        // or folder) and then the rest.
        QStringList linedata = {""};

        // While we are still not at the end of the file:
        while (!in.atEnd())
        {
            // Read the next line and store it in 'line', using simplified() to collapse any whitespace (such as tabs)
            // to a single space, and then add one to the linenumber counter.
            line = in.readLine().simplified();
            linenumber++;

            // Check if the line is not empty, and, if commented with '//', there is something behind the comment, and,
            // when the line behind the comment (if any) has one ':' character, then:
            if(line != "" && line.split("//")[0] != "" &&  line.split("//")[0].split(":").size() == 2)
            {
                // Split the data behind any comment character ('//') by the character ':'.
                linedata = line.split("//")[0].split(":");

                // Check if there is no text either behind or in front of the ':' separator:
                if (linedata[0] == "" || linedata[1] == "")
                    // If so, add a warning to the warning string.
                    warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                else
                {
                    // If not, check if the option code behind the ':' matches any checkbox or combobox element.
                    if (this->findChild<QCheckBox *>(linedata[0]) || this->findChild<QComboBox *>(linedata[0]))
                    {
                        // If so, add the data into an OptionObject struct.
                        o = {linedata[0],linedata[1]};
                        // Add the PlayerOption into the list of PlayerOption structs.
                        playeroptionslist.append(o);

                        // If the current player option is a checkbox, then:
                        if (this->findChild<QCheckBox *>(o.option))
                        {
                            // Store the UI element into its own object (this can only be done at this point,
                            // otherwise an exception is thrown if it turned out the option was not of this type.
                            QCheckBox *uiObject = this->findChild<QCheckBox *>(o.option);

                            // Make a bool to store whether the status of the checkbox should be checked or not.
                            bool status = false;

                            // If the setting for the option is '1', change status to reflect this, else keep it False.
                            if(o.setting == "1")
                                status = true;

                            // Set the checked status of the checkbox whatever the 'status' variable turned out to be.
                            uiObject->setChecked(status);
                        }
                        // If the current player option was not a checkbox (hence is a combobox), then:
                        else
                        {
                            // Same implementation as above, only the type of child object to find changed.
                            QComboBox *uiObject = this->findChild<QComboBox *>(linedata[0]);

                            // If the selected option is not the default (which means we don't need to do anything).
                            if (linedata[1] != "default")
                            {
                                // Try to find the index of the selected option in the respective combobox.
                                int indexOption = uiObject->findData(linedata[1]);

                                // If it cannot find the option in the combobox (it returns -1), add a warning.
                                if(indexOption == -1)
                                    warningtext += QString("Error at line %1: could not find option %2.\n").arg(linenumber).arg(linedata[1]);
                                // Otherwise, if we did find it, set the selected combobox option appropriately.
                                else
                                    uiObject->setCurrentIndex(indexOption);
                            }
                        }
                    }

                    // If, above, we could not find the option name in any checkbox or combobox, add a warning.
                    else
                        warningtext += QString("Error at line %1: could not find option %2.\n").arg(linenumber).arg(linedata[0]);
                }
            }
        }

        // Check if the warning string is not empty:
        if (!warningtext.isEmpty())
        {
            // If so, add a small introduction behind the list of errors, and display a warning to the user.
            warningtext = "The following errors were found in the player data file:\n\n" + warningtext;
            QMessageBox::warning(this,"Player data errors!", warningtext, QMessageBox::Close, QMessageBox::Close);
        }

        // Close the QFile since we finished using it.
        playerdatafile.close();
    }

    // If the file was not found, then:
    else
    {
        // Generate the player data file contents with the default settings.
        QString datatext = playerDataTextGen();

        // Try to save the player data file with the QString we generated above and store the returned error string.
        QString errstring = writePlayerData(datatext);

        // If writing the player data file was not successful (did not return an empty error string), warn the player.
        if (errstring != "")
            QMessageBox::warning(this,"File not writeable", errstring, QMessageBox::Ok, QMessageBox::Ok);
    }
}

// Method to generate the text of the player data file.
QString MainWindow::playerDataTextGen()
{
    // Self-explanatory. QString to store the data as we generate it.
    QString dataText = "// ------------------------------------------------------------------------------------------------------------\n"\
            "// Start options checkboxes\n";
    // To pass the state of the checkbox as checked (1) or unchecked (0), we form a QString with a %1 placerholder for
    // the parameter we pass with arg(). The syntax (Bool ? "1" : "0") checks a boolean and assigns a value for the
    // true or false check inline. If the checkbox is checked (hence == true), write '1'; if false, write '0'.
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

// Method to read the mod's RTW preferences.txt file.
int MainWindow::readPreferences()
{
    // NOTE: Maybe use QPairs here?
    // A QVector of PlayerOption structs to store the max resolutions.
    QVector<PlayerOption> maxresolutions;

    // A PlayerOption struct to append to the QVector above, initialised to the strategy map max resolution.
    PlayerOption po = {"STRATEGY_MAX_RESOLUTION", maxres};
    // Append the PlayerOption struct to the QVector.
    maxresolutions.append(po);

    // Change the PlayerOption struct to the battle map max resolution and append.
    po.option = "BATTLE_MAX_RESOLUTION";
    maxresolutions.append(po);

    // Generate the absolute file path for the RTW preferences file using the relative path defined in optiondata.h.
    QString prefpath = QCoreApplication::applicationDirPath() + preferencesFilePath;
    // Generate the absolute path for the RTW preferences directory.
    QString prefFolder = QCoreApplication::applicationDirPath() + "/../preferences";

    // Check if the preferences directory does not exist yet.
    if (!dirExists(prefFolder))
    {
        // If so, make a QDir object pointing to the preferences folder, create the directory, and check if it failed.
        QDir preferencesDir(prefFolder);
        if(!preferencesDir.mkdir(prefFolder))
        {
            // If so, alert the player.
            QMessageBox::critical(this, "Folder creation error", "The preferences folder is missing and could not be created. Please try again or create the folder '[RomeTW folder]/[RTR folder]/preferences/' manually.",\
                                 QMessageBox::Close, QMessageBox::Close);

            // Since we cannot do anything with the preferences dialog if we cannot even create the preferences
            // folder, return an error code 403, very cleverly referencing the HTTP error code for Forbidden.
            return 403;
        }
    }

    // Check if the preferences file does not exists.
    if (!fileExists(prefpath))
    {
        // If so, try to write the default preferences file, and if it is not successful, alert the player.
        if (prefsdialog->writePreferences(defaultPreferencesData) != 0)
        {
            QMessageBox::critical(this, "Folder creation error", "The preferences file could not be created. Please try again.",\
                                 QMessageBox::Close, QMessageBox::Close);
            // Since we cannot do anything with the preferences dialog if we cannot even create the preferences file,
            // return an error code 403, very cleverly referencing the HTTP error code for Forbidden.
            return 403;
        }
    }

    // Make a QFile object pointing to the preferences file.
    QFile preffile(prefpath);

    // Check if the file cannot be opened for reading and writing.
    if (!preffile.open(QIODevice::ReadWrite))
    {
        // If so, alert the player.
        QMessageBox::critical(this,"File not readable", "The preferences file could not be read. Please check your RTR installation and make sure the preferences file"\
                                                     " is accessible.\n\nError: " + preffile.errorString());
        // Since we cannot do anything with the preferences dialog if we cannot even open the preferences file, return
        // an error code 403, very cleverly referencing the HTTP error code for Forbidden.
        return 403;
    }

    // Otherwise, create a QTextStream interface 'in' with a reference to the preferences data file QFile object.
    QTextStream in(&preffile);
    // Set the encoding to UTF-8 for less limited script support.
    in.setCodec("UTF-8");

    // A QString to store the contents of the file line-by-line as we go through it.
    QString prefcontents = "";

    // Create a QString to store any possible errors and warnings that come up when reading the file.
    QString warningtext="";
    // And an unsigned line number counter to keep track of the line in which the errors and warnings were found.
    uint linenumber=0;

        // Variables used in the file reading loop:
    // A QString to store the contents of each line.
    QString line = "";
    // A list of QStrings to store the different data elements of each option in the file, the type of path (file
    // or folder) and then the rest.
    QStringList linedata = {""};
    // A bool to signal whether the preferences.txt file needs to be updated, or can remain as is.
    bool needsupdate = false;

    // While we are still not at the end of the file:
    while (!in.atEnd())
    {
        // Read the next line and store it in 'line', using simplified() to collapse any whitespace (such as tabs)
        // to a single space, and then add one to the linenumber counter.
        line = in.readLine().simplified();
        linenumber++;

        // Check if the line is not empty, and has at least one ':' character, then:
        if(line != "" && line.split(":").size() > 1)
        {
            // Split the data by the character ':'.
            linedata = line.split(":");

            // Check if there is no text either behind or in front of the ':' separator:
            if (linedata[0] == "" || linedata[1] == "")
                // If so, add a warning to the warning string.
                warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
            else
            {
                // If not, check if the text behind the ':' matches either the maximum strategic map or battle map
                // resolution option code (as stored in the maxresolutions object).
                if (linedata[0] == maxresolutions.at(0).option || linedata[0] == maxresolutions.at(1).option)
                {
                    // If so, check if the settings in the file match the maximum resolution we have defined.
                    if (linedata[1] != maxres)
                    {
                        // If so, modify the line in question to set the maximum resolution, and signal to update the file.
                        prefcontents += linedata[0] + ":" + maxres + "\n";
                        needsupdate = true;
                    }
                }

                // If the line did not have anything to do with the strategy map or battle map resolution, then:
                else
                {
                    // Add the line to the rest of the lines already stored, since we won't edit it at this point.
                    prefcontents += line + "\n";

                    // If there is a checkbox in the preferences dialog that matches the option in the line, then:
                    if (prefsdialog->findChild<QCheckBox *>(linedata[0]))
                    {
                        // Get the QCheckBox object of the option in question
                        QCheckBox *optioncheckbox = prefsdialog->findChild<QCheckBox *>(linedata[0]);
                        // TODO: Refactor modifier capture.
                        // Get a QList of bools of the modifiers for the checkbox statuses (for more information, see
                        // preferencesDialog class and the getMods method).
                        QList<bool> modifiers = prefsdialog->getMods();

                        // Store the 'real' status of the option in the preferences file as a bool. The syntax
                        // (Bool ? "1" : "0") checks a boolean and assigns a value for thetrue or false check inline.
                        // If the data in this line after the ':' reads 'TRUE', assign a boolean true, and viceversa.
                        bool realstatus =  linedata[1] == "TRUE" ? true : false;

                        // Convoluted way to mask the true status with the modifier.
                        // Get a QList of all the QCheckBox objects in the preferences dialog.
                        QList<QCheckBox *> listofboxes = prefsdialog->getListPrefs();

                        // For each QCheckBox object in the list:
                        foreach (QCheckBox *cb, listofboxes)
                        {
                            // If the name of the object is identical to the option in the line, then:
                            if (cb->objectName() == linedata[0])
                            {
                                // The value of modstatus is an XNOR (==) between the real status and the modifier.
                                bool modstatus = realstatus == modifiers.at(listofboxes.indexOf(cb));
                                // The checked status of the option checkbox for the option in the current line is set
                                // according to the value of modstatus.
                                optioncheckbox->setChecked(modstatus);
                                // No need to keep checking the remaining QCheckBox objects anymore, so break.
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Else, if the line is empty or has not ':' character, simply add the line to the rest of the file contents.
        else
            prefcontents += line + "\n";
    }
    // Reached the end of the preferences data file.

    // If we need to update the preferences file, then:
    if (needsupdate)
    {
        // Empty the contents of the preferences file.
        preffile.resize(0);

        // Create a QTextStream interface 'out' with a reference to the preferences data file QFile object.
        QTextStream out(&preffile);

        // Set the encoding to UTF-8 for wider script support.
        out.setCodec("UTF-8");

        // Put the modified contents to store into the QTextStream (and therefore, the preferences file).
        out << prefcontents;
    }

    // Save the modified contents of the preferences file into the respective variable of the preferencesDialog class.
    prefsdialog->preferencesText = prefcontents;

    // Check if the warning string is not empty:
    if (!warningtext.isEmpty())
    {
        // If so, add a small introduction behind the list of errors, and display a warning to the user.
        warningtext = "The following errors were found in the preferences file:\n\n" + warningtext;
        QMessageBox::warning(this,"Preferences file errors!", warningtext, QMessageBox::Close, QMessageBox::Close);
    }

    // Close the QFile since we finished using it.
    preffile.close();

    // Since any errors detected so far are not application-breaking, return an all ok '0'.
    return 0;
}

// Method to add the available options of the launcher data file to their respective comboboxes.
// Takes as parameters a pointer to an OptionData class with all the data for the combobox, the pointer to the combobox
// UI element, a QString with the code of the option type, and a QString for the name of the default option.
void MainWindow::setOptions(OptionData *l, QComboBox *combobox, QString optiontypestring, QString defaultname)
{
    // Get the list of launcher options for the current option type.
    QList<OptionObject> optionslist = l->getListByOptionType(optiontypestring);

    // If there are no options for the current option type (the size of the list is 0), then:
    if (optionslist.size() == 0)
    {
        // Add a default option to the combobox with the text of the default name initialised in the header, or, the
        // one passed to the method as an argument, and 'default' for the hidden data for that option.
        combobox->addItem(defaultname, "default");
        combobox->setEnabled(false);
    }
    // Else, if there is at least one option for the current option type, then:
    else
    {
        // Loop through each OptionObject struct, and:
        foreach (OptionObject o, optionslist)
        {
            // Add a new item to the combobox with the option code as the hidden data for that combobox option.
            combobox->addItem(o.displayname, o.optioncode);
        }
    }
}

// Checking data methods:
// Method to check whether the user's configuration is different from the defaults.
bool MainWindow::checkDefaults()
{
    // TODO: Expand to include comboboxes?
    // Bool variable to return whether the current settings in the launcher are the same as the defaults.
    bool defaultsset = true;

    // Loop through each checkbox UI element in the launcher by using a private QList of QCheckBoxes, and:
    foreach (QCheckBox *c, checkboxeslist)
    {
        // If the checkbox is not the show_err checkbox (found by checking the object name), then:
        if (c->objectName() != "showerrBox")
            // Set the checked variable to the result of defaultsset AND the negation of the checked status of the
            // checkbox. This makes it so that all the checkboxes except the show_err one are unchecked by default.
            defaultsset = defaultsset && !c->checkState();
        // Else, if the checkbox is the show_err checkbox, then:
        else
            // See above, but the checked status is not negated; meaning the show_err box should be checked by default.
            defaultsset = defaultsset && c->checkState();
    }

    // Return the boolean variable.
    return defaultsset;
}

// Method to check whether the map.rwm file is up to date to the rest of the contents of the other map files.
int MainWindow::checkMapRwm()
{
    // Create a QDir object pointing to the campaign map's base directory.
    QDir basedir(QCoreApplication::applicationDirPath() + "/../data/world/maps/base");

    // Check if the base directory exists.
    if (basedir.exists())
    {

        // If it does, generate the absolute file path pointing to the absolute path of the map.RWM file, the
        // RTW-generated map file that must be deleted to generate a new one in case of any recent changes.
        QString maprwmpath = basedir.absolutePath() + "/map.rwm";

        // Check if the map.RWM file exists.
        if (fileExists(maprwmpath))
        {
            // If it does, create a QFileInfo object pointing to the absolute path of the file.
            QFileInfo maprwm(maprwmpath);

            // Store the timestamp of the last modification of the map.RWM file to compare with the raw map files.
            QDateTime mapmod = maprwm.lastModified();

            //basedir.setFilter(QDir::Files);
            // Store the list of files in the base folder into a QFileInfoList object to check if the map is up-to-date.
            QFileInfoList filelist = basedir.entryInfoList(QDir::Files);

            // Create a bool to signal whether the map.RWM needs to be deleted or not, initialise it as false.
            bool needsdel = false;

            // For each file (as a QFileInfo object) in the list of files:
            foreach (QFileInfo fileinfo, filelist)
            {
                // If so, store the extension of the current file we are processing.
                QString ext = fileinfo.suffix();

                // Check if the extension is either .TGA or .TXT, since those are the only relevant ones.
                if (ext=="tga" || ext=="txt")
                {
                    // If it is, then store the timestamp of the last modification.
                    QDateTime filemod = fileinfo.lastModified();

                    // Check if the current file was modified after the map file by comparing the timestamps.
                    if (mapmod < filemod)
                    {
                        // If it was, signal that we need to delete the map.RWM file.
                        needsdel = true;
                    }
                }
            }

            // Check if we need to delete the map.RWM file.
            if (needsdel)
            {
                // If we do, create a QFile object pointing to the absolute path of the map.RWM file.
                QFile maprwmfile(maprwmpath);

                // Try to remove it, and, if not successful, send a critical error
                if (!maprwmfile.remove())
                {
                    QMessageBox::critical(this,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR"\
                                                         " installation.", QMessageBox::Close, QMessageBox::Close);
                    // Return an error code 404, very cleverly referencing the HTTP error for forbidden.
                    return 403;
                }

                // Otherwise, inform the user of our success and the generation process, and return an all ok '0'.
                QMessageBox::information(this,"Success", "The map.rwm file was out of date and was successfully deleted.\nAn up-to-date file will be generated by the game"\
                                                         " at launch.", QMessageBox::Close, QMessageBox::Close);
                return 0;
            }
            // If we did not need to delete the map.RWM file, inform the user, and return an all ok '0'.
            else
            {
                QMessageBox::information(this,"Success", "The map.rwm file was up-to-date.\nNo further action was needed.", QMessageBox::Close, QMessageBox::Close);
                return 0;
            }
        }

        // If the file was not found, then inform the player, and since the file will be generated by the game, and it
        // will handle any other issues with the raw map files, if any, our job is done, so we return an all ok '0'.
        else
        {
            QMessageBox::information(this,"Success", "The map.rwm file was not present.\nAn up-to-date file will be generated by the game at launch.", QMessageBox::Close,\
                                     QMessageBox::Close);
            return 0;
        }
    }

    // If the campaign map's base directory was not found, then send a critical error.
    else
    {
        QMessageBox::critical(this,"Critical error!", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                              "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]"\
                              "/RTRLauncher.exe.", QMessageBox::Close, QMessageBox::Close);
        // Return an error code 404, very cleverly referencing the HTTP error for file not found.
        return 404;
    }
}

// Reset options methods:
void MainWindow::resetChecks()
{
    // To reset the checkboxes to default, we go through each QCheckBox in the chechboxes list:
    foreach (QCheckBox *c, checkboxeslist)
    {
        // Set the check status of each to false,
        c->setChecked(false);
    }
    // but set the showerrBox to true, since it's very useful for debugging, and there is no reason to leave it off.
    ui->showerrBox->setChecked(true);
}

// Write data and launch game methods.
QString MainWindow::writePlayerData(QString dataToWrite)
{
    // Create a QFile object pointing to the the absolute file path for the player data file generated using the
    // relative path defined in optiondata.h (since we'll overwrite it, there's no reason to check if it exists).
    QFile playerData(QCoreApplication::applicationDirPath() + playerDataFilePath);

    // If the file cannot be opened (or created) in write-only mode, and its contents discarded (with Truncate):
    if (!playerData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // Return a string informing the user of the error.
        return QString("The player data file could not be written. Please check your RTR installation and make sure the player data file"\
                                                     " is accessible.\nError: " + playerData.errorString());
    }

    // Otherwise, create a QTextStream interface 'out' with a reference to the player data file QFile object.
    QTextStream out(&playerData);

    // Set the encoding to UTF-8 for wider script support.
    out.setCodec("UTF-8");

    // Put the data received as a method parameter into the QTextStream (and therefore, the player data file).
    out << dataToWrite;

    // Close the QFile since we finished using it.
    playerData.close();

    // Return an empty string, signalling there were no errors.
    return "";
}

QString MainWindow::filefolderSwitch(QComboBox *combobox, QList<OptionObject> options)
{
    QString currentoption = combobox->currentData().toString();
    if (currentoption == "default")
        return "";
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
                    return "";
                else
                    return QString("The folder for the game type '%1' could not be copied.\n").arg(combobox->currentText());
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
                        return "";
                    else
                        //qDebug() << filecopy.errorString();
                        return QString("The file for the game type '%1' could not be copied.\n").arg(combobox->currentText());;
                        //messagetext += QString("The file for the game type '%1' could not be copied.\n").arg(o.displayname);
                }
            }
        }
        return QString("The files for the game type '%1' could not be found.\n").arg(combobox->currentText());
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
        case 403:
            QMessageBox::critical(this,"Error!", "The map.rwm file was out of date but could not be deleted.\nPlease try again. If this error persists, check your RTR installation.", QMessageBox::Close, QMessageBox::Close);
            QMessageBox::StandardButton confirm;
            confirm = QMessageBox::warning(this,"Continue launching?", "The map file could not be deleted and will be out of date.\n"\
                                 "Would you like to continue launching the game?",QMessageBox::Yes|QMessageBox::No);
            if (confirm == QMessageBox::No)
                argList[0] = "n";
            break;
        case 404:
            QMessageBox::critical(this,"Critical error!", "The base campaign folder was not found. Make sure you have installed RTR correctly,"\
                                                  "and that the launcher is in the location \n[RTW install folder]/[RTR mod folder]/[launcher folder]/RTRLauncher.exe.\n\n", QMessageBox::Close, QMessageBox::Close);
                argList[0] = "n";
            break;
        }
    }

    return argList;
}
