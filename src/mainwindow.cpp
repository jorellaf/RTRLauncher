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
#include <QCryptographicHash>

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
    // buttons, but no maximise (Qt::WindowMinimize... & Qt::WindowClose...), and, on Windows, since it is a window of
    // fixed size, it should look as such (Qt::MSWindowsFixedSize...).
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
        // Return an error code 404, very cleverly referencing the HTTP error for File Not Found.
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

    // Saving preferences from the main window is not the best idea. Best force save/discard in the preferences dialog.
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

    // If the checkbox to always check and update the map.RWM file is checked:
    if (ui->maprwmBox->isChecked())
    {
        // Make the check and store the returned integer in 'result'.
        int result = checkMapRwm();

        // Check the value of result and:
        switch (result)
        {
        // If the case is 403 (map.RWM deletion not successful):
        case 403:
            // Make a QMessageBox StandardButton object that will store the button the user pressed.
            QMessageBox::StandardButton confirm;

            // TODO: De-deprecate functions.
            // Create a message box asking the player to confirm if they want to continue launching the game even though
            // the map.RWM file could not be deleted, with the default button (when pressing enter) being no.
            confirm = QMessageBox::warning(this,"Continue launching?", "The map file could not be deleted and will be out of date.\n"\
                                                                       "Would you like to continue launching the game?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

            // If the button pressed was the 'No' button:
            if (confirm == QMessageBox::No)
                // Set the first item of the arguments list to 'n' to indicate the launch should not go on.
                commands[0] = "n";
            break;

        // If the case is 404 (campaign map base folder not found):
        case 404:
            // Set the first item of the arguments list to 'n' to indicate the launch should not go on.
            commands[0] = "n";
            break;
        }
    }

    // If we have arrived all the way to here without any errors (goahead == true), and if the returned list of
    // commands is not just 'n' (do not proceed) then proceed to take-off.
    if (goahead == true && commands[0]!="n")
    {
        // QFileInfo object for the RTW executable to launch, assuming the user has correctly installed the launcher in
        // [RomeTW/MODFOLDER/Launcher]. Path has to be absolute, otherwise it does not work.
        QFileInfo rtwexec(QDir::cleanPath(QDir::currentPath() + "/../../RomeTW-ALX.exe"));
        // If the current installation is a Steam installation, we need to use the testappa.exe file instead, so check if
        // it exists (to check if it's a Steam install) and set that as our rtwexec file.
        if (fileExists(QDir::cleanPath(QDir::currentPath() + "/../../testappa.exe")))
        {
            QFileInfo rtwexec(QDir::cleanPath(QDir::currentPath() + "/../../testappa.exe"));
        }
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
            if (process -> startDetached(execabspath, commands, QDir::cleanPath(QDir::currentPath() + "/../..")))
                // Close the launcher by quitting the application
                // (though it would be better if it returned 0 to the main, but alas).
                QApplication::quit();
            else
                // TODO: Implement engine selection.
                // If the executable could not be launched, alert the user of the problem with a message box.
                QMessageBox::critical(this,"Critical error!", "The Rome: Total War - Alexander executable could not be launched. Make sure you have installed Rome: Total War correctly,"\
                                                              "and that the launcher is in the location \n[RomeTW install folder]/[mod folder]/[launcher folder]/Launcher.exe.",\
                                      QMessageBox::Close, QMessageBox::Close);
        }
        else
        {
            // If the executable was not found, alert the user of the problem with a message box.
            QMessageBox::critical(this,"Critical error!", "The Rome: Total War - Alexander executable was not found. Make sure you have installed the mod correctly,"\
                                                          "and that the launcher is in the location \n[RomeTW install folder]/[mod folder]/[launcher folder]/Launcher.exe.",\
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
    //prefsdialog->setChangesMadeFalse();

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

    // TODO: Refactor the Player Option into its own class, with its own getter methods and make a comparison here between selected option and previous option.
    // Append the string returned by the file/folder switch method (error text or nothing) to the errortext variable,
    // which we call by sending the desired combobox and OptionData object with the desired option type.
    // EDU switch:
    errstring += filefolderSwitch(ui->eduComboBox, launcheroptionslist.getListOfEdus());
    // Campaign switch
    errstring += filefolderSwitch(ui->campComboBox, launcheroptionslist.getListOfCamps());
    // Trees switch
    errstring += filefolderSwitch(ui->treesComboBox, launcheroptionslist.getListOfTrees());

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
        // TODO: De-deprecate functions.
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
// TODO: Refactor into optiondata
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
    if (!fileExists(QDir::cleanPath(QDir::currentPath() + "/../../RomeTW-ALX.exe")))
        returnstring += "The Rome: Total War - Alexander executable was not found. Make sure you have installed the mod correctly,"\
                        "and that the launcher is in the location \n[RomeTW install folder]/[mod folder]/[launcher folder]/Launcher.exe.\n\n";

    // If the mod data folder was not found, add an error message to the string.
    if (!dirExists(QDir::cleanPath(QDir::currentPath() + "/../data")))
        returnstring += "The mod data folder was not found. Make sure you have installed the mod correctly\n\n";

    // If the launcher data file was not found, add an error message to the string.
    if (!fileExists(QDir::cleanPath(QDir::currentPath() + launcherDataFilePath)))
        returnstring += "Could not find the data file for the launcher (launcher.dat) in the launcher folder. Check that you have installed the mod correctly"\
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
    QString launcherdatafilepath = QDir::currentPath() + launcherDataFilePath;

    // Check if the data file exists.
    if (fileExists(launcherdatafilepath))
    {
        // If it does, create a QFile object pointing to the absolute path of the file.
        QFile launcherdata(launcherdatafilepath);

        // Try to open it, and if it doesn't, send a critical error and return an error code.
        if (!launcherdata.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::critical(this,"File not readable", "The data file could not be read. Please check your installation and make sure the data"\
                                                            " is accessible.\n\nError: " + launcherdata.errorString());
            // Very cleverly referencing the HTTP error code for Bad Request.
            return 400;
        }

        // Otherwise, create a QTextStream interface 'in' with a reference to the launcher data file.
        QTextStream in(&launcherdata);
        // Set the encoding to UTF-8 for less limited script support.
        in.setCodec("UTF-8");

        // Initialise an OptionObject struct to use for appending options to the list of options.
        OptionObject o = {"","","","","",""};

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
        // QString to store the source file/folder path of each option.
        QString objsrcpath = "";
        // QString to store the destination folder path of each option. The default destination is the RTR data folder.
        QString objdstpath = "/RTR/data/";

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

                // Check if there is nothing after the ':', or if what follows is not split in five elements
                // (srcpath|dstpath|code|name|type) by the '|' divider, or if there is nothing behind the ':' character:
                if (linedata[1] == "" || linedata[1].split("|").size() != 5 || linedata[0] == "")
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

                        // Check if any of the elements (besides dstpath, since default is /RTR/Data) is not empty:
                        if (objargs[0]=="" || objargs[2]=="" || objargs[3]=="" || objargs[4]=="")
                            // If so, add a warning to the warning string.
                            warningtext += QString("Error at line %1: invalid empty argument.\n").arg(linenumber);
                        else
                        {
                            // If not, get the object path, and convert any backslash (has to be escaped with another
                            // one here) to a forward slash, to allow for the use of either type.
                            objsrcpath = objargs[0].split("\\").join("/");

                            // If there is no forward slash at the front of the path in the file, add one, as it is
                            // required in order to create the path for the Qt file/folder objects.
                            if (objsrcpath.front() != "/")
                                objsrcpath = QString("/" + objsrcpath);

                            // Check if, for files, there are fewer than two forward slashes in the path (/x/file.y).
                            if (linedata[0] == "file" && objsrcpath.split("/").size() < 3 )
                                // If so, add a warning to the warning string, since this would be invalid.
                                warningtext += QString("Error at line %1: file path is invalid (must be inside its own folder folder inside /RTR/).\n").arg(linenumber);
                            // Else, check if the file name at the end of the path is an empty string.
                            else if (linedata[0] == "file" && objsrcpath.split("/").last() == "")
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: file name is invalid.\n").arg(linenumber);
                            // Else, check if there are any empty folder sequences ('//') for either files or folders.
                            else if (objsrcpath.split("//").size()>1)
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: empty folder names are not allowed. If you used '//' as a divider, please change it to '/' or '\'.\n")\
                                        .arg(linenumber);
                            // Else, check if the user tried to go up a level in the folder tree with '..'.
                            else if (objsrcpath.split("..").size()>1)
                                // If so, add a warning to the warning string.
                                warningtext += QString("Error at line %1: you may not go a level higher in the folder tree.\n").arg(linenumber);
                            else
                            {
                                // TODO: Refactor and make expansible.
                                // If none of these issues were detected, check if we should parse the destination path
                                if (objargs[1]!="")
                                {
                                    objdstpath = objargs[1].split("\\").join("/");

                                    // If there is no forward slash at the front of the path in the file, add one, as it is
                                    // required in order to create the path for the Qt file/folder objects.
                                    if (objdstpath.front() != "/")
                                        objdstpath = QString("/" + objdstpath);
                                    // If there is no forward slash at the end of the path in the file, add one, as it should be a folder.
                                    if (objdstpath.back() != "/")
                                        objdstpath = QString("/" + objdstpath);

                                    // Else, check if there are any empty folder sequences ('//') for either files or folders.
                                    else if (objdstpath.split("//").size()>1)
                                        // If so, add a warning to the warning string.
                                        warningtext += QString("Error at line %1: empty folder names are not allowed. If you used '//' as a divider, please change it to '/' or '\'.\n")\
                                                .arg(linenumber);
                                    // Else, check if the user tried to go up a level in the folder tree with '..'.
                                    else if (objdstpath.split("..").size()>1)
                                        // If so, add a warning to the warning string.
                                        warningtext += QString("Error at line %1: you may not go a level higher in the folder tree.\n").arg(linenumber);
                                }
                                // If none of these issues were detected, check if the option type is not any of the
                                // allowed options.
                                if (objargs[4] != "edu" && objargs[4] != "camp" && objargs[4] != "trees")
                                    // If so, add a warning to the warning string.
                                    warningtext += QString("Error at line %1: invalid option type '%2'.\n").arg(linenumber).arg(objargs[4]);
                                else
                                {
                                    // If no errors have been detected so far, begin properly parsing the option data.
                                    // Create a QDir object to generate the absolute path for the option file/folder.
                                    QDir objsrcpathdir(QDir::cleanPath(QDir::currentPath() + "/../" + objsrcpath));
                                    // If no errors have been detected so far, begin properly parsing the option data.
                                    // Create a QDir object to generate the absolute path for the option file/folder, based on the RTW folder.
                                    QDir objdstpathdir(QDir::cleanPath(QDir::currentPath() + "/../../" + objdstpath));

                                    // Add the data into an OptionObject struct.
                                    o = {linedata[0],objsrcpathdir.absolutePath(),objdstpathdir.absolutePath(),objargs[2],objargs[3],objargs[4]};
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
                if (!dirExists(oo.abssrcpath))
                    // If a directory cannot be found, add a warning to the QString.
                    warningtext += QString("Directory error: directory chosen for option '%1' cannot be found. Verify your installation or your launcher data file.\n").arg(oo.displayname);

        // If we have any options using files, go through them and check if the files chosen exist:
        if (l->getListOfFiles().size()>0)
            foreach (OptionObject fo, l->getListOfFiles())
                if (!fileExists(fo.abssrcpath))
                    // If a file cannot be found, add a warning to the QString.
                    warningtext += QString("File error: file chosen for option '%1' cannot be found. Verify your installation or your launcher data file.\n").arg(fo.displayname);

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
                                                           " Check that you have installed the mod correctly and not moved any files in the launcher folder.\n\n",\
                              QMessageBox::Close, QMessageBox::Close);
        // Return an error code 404, very cleverly referencing the HTTP error for File Not Found.
        return 404;
    }
}

// Method to read the player data file.
void MainWindow::readPlayerData()
{
    // Generate the absolute file path for the player data file using the relative path defined in optiondata.h.
    QString playerdatapath = QDir::currentPath() + playerDataFilePath;

    // Check if the data file exists.
    if (fileExists(playerdatapath))
    {
        // If it does, create a QFile object pointing to the absolute path of the file.
        QFile playerdatafile(playerdatapath);

        // Try to open it, and if it doesn't, send a critical error.
        if (!playerdatafile.open(QIODevice::ReadOnly))
            QMessageBox::critical(this,"File not readable", "The player data file could not be read. Please check your installation and make sure the player data file"\
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

// TODO: Refactor into Preferences Dialog.
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
    QString prefpath = QDir::currentPath() + preferencesFilePath;
    // Generate the absolute path for the RTW preferences directory.
    QString prefFolder = QDir::cleanPath(QDir::currentPath() + "/../preferences");

    // Check if the preferences directory does not exist yet.
    if (!dirExists(prefFolder))
    {
        // If so, make a QDir object pointing to the preferences folder, create the directory, and check if it failed.
        QDir preferencesDir(prefFolder);
        if(!preferencesDir.mkdir(prefFolder))
        {
            // If so, alert the player.
            QMessageBox::critical(this, "Folder creation error", "The preferences folder is missing and could not be created. Please try again or create the folder"\
                                                                 "'[RomeTW folder]/[mod folder]/preferences/' manually.", QMessageBox::Close, QMessageBox::Close);

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
        QMessageBox::critical(this,"File not readable", "The preferences file could not be read. Please check your installation and make sure the preferences file"\
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
                        // preferencesDialog class and the getMods method implementation).
                        QList<bool> modifiers = prefsdialog->getMods();

                        // Store the 'real' status of the option in the preferences file as a bool. The syntax
                        // (Bool ? X : Y) checks a boolean and assigns a value for the true or false check inline. If
                        // the data in this line after the ':' reads 'TRUE', assign the value of X (in this case a
                        // boolean true), and, if not, assign Y (in this case, a boolean false).
                        bool realstatus =  (linedata[1] == "TRUE") ? true : false;

                        // Convoluted way to mask the true status with the modifier.
                        // Get a QList of all the QCheckBox objects in the preferences dialog.
                        QList<QCheckBox *> listofboxes = prefsdialog->getListCheckboxes();

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
// A parameter of force_delete disregards timestamp checks, and is set by default to false in the header.
int MainWindow::checkMapRwm(bool force_delete)
{
    // Create a QDir object pointing to the campaign map's base directory.
    QDir basedir(QDir::cleanPath(QDir::currentPath() + "/../data/world/maps/base"));

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

            // Create a bool to signal whether the map.RWM needs to be deleted or not, initialise it as false.
            bool needsdel = false;

            // Create a QString to store the beginning of the error message, to make it more modular to the options availaable
            // (force delete, check timestamps, and their success or failure).
            QString message = "The map.rwm file ";

            if (!force_delete)
            {
                // If we don't force delete, we make allusions to how we checked timestamps, and how it was out of date.
                message += "was out of date ";

                // Store the timestamp of the last modification of the map.RWM file to compare with the raw map files.
                QDateTime mapmod = maprwm.lastModified();

                // Store the list of files in the base folder into a QFileInfoList object to check if the map is up-to-date.
                QFileInfoList filelist = basedir.entryInfoList(QDir::Files);

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
            }

            // Check if we need to delete the map.RWM file.
            if (needsdel || force_delete)
            {
                // If we do, create a QFile object pointing to the absolute path of the map.RWM file.
                QFile maprwmfile(maprwmpath);

                // Try to remove it, and, if not successful, send a critical error
                if (!maprwmfile.remove())
                {
                    // First, if we talked about the file being out of date, add a conjunction.
                    if (!force_delete)
                        message += "but ";
                    QMessageBox::critical(this,"Error!", message + "could not be deleted.\nPlease try again. If this error persists, check your"\
                                                         " installation.", QMessageBox::Close, QMessageBox::Close);
                    // Return an error code 404, very cleverly referencing the HTTP error for Forbidden.
                    return 403;
                }

                // First, if we talked about the file being out of date, add a conjunction.
                if (!force_delete)
                    message += "and ";
                // Otherwise, inform the user of our success and the generation process, and return an all ok '0'.
                QMessageBox::information(this,"Success", message + "was successfully deleted.\nAn up-to-date file will be generated by the game"\
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
        QMessageBox::critical(this,"Critical error!", "The base campaign folder was not found. Make sure you have installed the mod correctly,"\
                                                      "and that the launcher is in the location \n[RomeTW install folder]/[mod folder]/[launcher folder]"\
                                                      "/Launcher.exe.", QMessageBox::Close, QMessageBox::Close);
        // Return an error code 404, very cleverly referencing the HTTP error for File Not Found.
        return 404;
    }
}

    // Reset options methods:
// Resets the checkboxes to their default states (reset to defaults).
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

    // Write data and launch game methods:
// Method to write to the player data file.
// Takes as parameter a QString that contains the entire contents of the file to write.
// TODO: Refactor with WritePreferences.
QString MainWindow::writePlayerData(QString dataToWrite)
{
    // Create a QFile object pointing to the the absolute file path for the player data file generated using the
    // relative path defined in optiondata.h (since we'll overwrite it, there's no reason to check if it exists).
    QFile playerData(QDir::cleanPath(QDir::currentPath() + playerDataFilePath));

    // If the file cannot be opened (or created) in write-only mode, and its contents discarded (with Truncate):
    if (!playerData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // Return a string informing the user of the error.
        return QString("The player data file could not be written. Please check your installation and make sure the player data file"\
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

// Method to perform the switch between different campaigns, EDUs, and trees options.
// Requires a QComboBox object point that points to the specific option switcher combobox to process, as well as a
// QList of OptionObject structs containing all the switching options and their information.
QString MainWindow::filefolderSwitch(QComboBox *combobox, QList<OptionObject> options)
{
    // Get the currently selected option of the combobox, specifically, the code, not the text (currentData).
    QString currentoption = combobox->currentData().toString();

    // If the currently selected option code is 'default', we do not need to switch between options as there is only
    // one, so return an errorless empty string.
    if (currentoption == "default")
        return "";

    // Otherwise, declare a variable for the index of the option in the QList to -1 (an out-of-list-bounds value).
    int optionindex = -1;

    // Initialise a counter for the following loop.
    int i = 0;
    // For each option in the QList of options.
    foreach (OptionObject option, options)
    {
        // If the option's code name is the same as the code of the currently selected option in the combobox:
        if (option.optioncode == currentoption)
        {
            // Set the optionindex value to the counter.
            optionindex = i;
        }
        // Add one to the counter.
        i++;
    }

    // If the optionindex is not the default out-of-bounds -1 (hence the option selected was found in the data file):
    if (optionindex != -1)
    {
        // Get the OptionObject struct of the desired option by grabbing the list object at the index we set earlier.
        OptionObject o = options.at(optionindex);

        // If the path type of the option is a directory, and not a file:
        if (o.pathtype=="dir")
        {
            // Check if it exists by using its absolute path, and if so:
            if (dirExists(o.abssrcpath))
            {
                // Get the results of the recusive copy of each option.
                QList<bool> success_and_basemapcopy = copyRecursively(o.abssrcpath,o.absdstpath);

                // If the copyRecursively option was successful:
                if (success_and_basemapcopy[0])
                {
                    // Check if we need to delete the map.rwm file, and do so.
                    if(success_and_basemapcopy[1])
                        checkMapRwm(true);
                    // Finally, return an empty error string.
                    return "";
                }
                // Otherwise, return an error message string saying the folder for the current option could not be copied.
                else
                    return QString("The folder for the game type '%1' could not be copied.\n").arg(o.displayname);
            }
        }

        // If we have reached this point, the path type was a file, so check if it exists by using its absolute path.
        if (fileExists(o.abssrcpath))
        {
            // If it does, make a QFileInfo and QFile objects pointing to the option's absolute path.
            QFileInfo srcfileinfo(o.abssrcpath);
            QFile srcfile(o.abssrcpath);

            // Make a QFileInfo pointing to the file we will replace (assuming the launcher is in the right folder).
            QFileInfo destfileinfo(o.absdstpath + srcfileinfo.fileName());
            // Make a string of the absolute path of the file we will replace.
            QString destfilepath(destfileinfo.absoluteFilePath());

            //qDebug() << edufileinfo.filePath();
            //QFileInfo file(filetoreplacepath);

            // If the file we want to replace exists, we need to delete it first, since copy() does not overwrite:
            if (fileExists(destfilepath))
            {
                // Make a QFile pointing to the file to replace and delete it (through remove()).
                QFile filetorm(destfilepath);
                filetorm.remove();
            }

            // If copying the option file to its destination is not successful, return an error message.
            if (!srcfile.copy(destfilepath))
                return QString("The file for the game type '%1' could not be copied.\n").arg(o.displayname);

            // We reach this point if there were no errors in the file replacing, so return an errorless empty string.
            return "";
        }
    }

    // Otherwise, if the option was not found in the list (and therefore the files needed to replace it), send an error
    // to the user, with the name of the currently selected option.
    return QString("The files for the game type '%1' could not be found.\n").arg(combobox->currentText());
}

// Method to copy folders recursively and paste them recursively to their destination.
// Require QStrings of the paths of the source and destination directories.
// Function taken and adapted from https://forum.qt.io/topic/59245/is-there-any-api-to-recursively-copy-a-directory-and-all-it-s-sub-dirs-and-files/3
// The success_and_basemapcopy parameter is a QList of bools to indicate the success of the operation,
// as well as whether any file in the data/world/maps/base folder was replaced, to force a map.rwm deletion.
// Both are set to false by default in the header, and have two indices. The first informs success, the second, /maps/base change.
QList<bool> MainWindow::copyRecursively(QString sourcedirpath, QString destdirpath, QList<bool> success_and_basemapcopy)
{
    // If the source directory does not exist, return false.
    if(!dirExists(sourcedirpath))
        return {false, false};

    // If the destination directory does not exist:
    if(!dirExists(destdirpath))
    {
        // Create a QDir object pointing to the destination directory path, and create the directory at the path.
        QDir destdir(destdirpath);
        destdir.mkdir(destdirpath);
    }

    // Declare a QDir pointing to the path of the source directory.
    QDir sourcedir(sourcedirpath);
    // Get a list of files in the source directory and store their names in a list of strings.
    QStringList files = sourcedir.entryList(QDir::Files);

    // Loop through each file:
    for(int i = 0; i< files.count(); i++) {
        // Generate the paths for the source and destination files.
        QString srcpath = sourcedirpath + QDir::separator() + files[i];
        QString destpath = destdirpath + QDir::separator() + files[i];

        // We declare a variable to confirm whether the file needs replacing or not.
        bool needsrep = false;

        // If the file we want to replace exists, we first check if we need to replace it:
        if (fileExists(destpath))
        {
            // We make QFile objects for the source and destination paths.
            QFile srcfile(srcpath);
            QFile dstfile(destpath);

            // We check whether the files have the same modification times. If not, we might need to replace them.
            // Idea taken and adapted from https://stackoverflow.com/a/1761709.
            if(QFileInfo(srcpath).lastModified() != QFileInfo(destpath).lastModified())
            {
                // If we can open both files as ReadOnly:
                if(srcfile.open(QFile::ReadOnly) && dstfile.open(QFile::ReadOnly))
                {
                    // We try to hash them. We first make QCryptographicHash objects to do an MD5 hash to each file.
                    // Snippet taken and adapted from https://stackoverflow.com/a/16383433.
                    QCryptographicHash srchash(QCryptographicHash::Md5);
                    QCryptographicHash dsthash(QCryptographicHash::Md5);

                    // If we can add the file data to the hash objects to get a hash:
                    if (srchash.addData(&srcfile) && dsthash.addData(&dstfile))
                    {
                        // We check if the hashes are not the same. If they are not, we proceed with replacing the file.
                        if (srchash.result() != dsthash.result())
                        {
                            // We indicate that the file needs replacing.
                            needsrep = true;
                        }
                    }
                    // If we couldn't get a hash, we try to replace them anyways to be safe.
                    else
                        needsrep = true;

                    srcfile.close();
                    dstfile.close();
                }
                // If we couldn't open the files, we try to replace them anyways to be safe.
                else
                    needsrep = true;
            }

            // If we do need to replace the file:
            if (needsrep == true)
            {
                // Get the QFile we made for the destination file and delete it (through remove()).
                // We need to delete it first, since copy() does not overwrite.
                // If the deletion was not successful, return false.
                if(!dstfile.remove())
                    return {false,false};

                // Assign the success of copying the source file to its destination to the boolean success variable.
                success_and_basemapcopy[0] = QFile::copy(srcpath, destpath);

                // If the copying was not successful, return false.
                if(!success_and_basemapcopy[0])
                    return {false,false};

                // If the current destination directory is the same as the directory of /maps/base, or if /maps/base has been altered earlier:
                if (QDir(destpath).absolutePath().indexOf(QDir(QDir::currentPath() + "/../data/world/maps/base").absolutePath()) != -1 || success_and_basemapcopy[1])
                    // Set the basemapcopy part of the variable as true.
                    success_and_basemapcopy[1] = true;
            }
            else
                success_and_basemapcopy[0] = true;
        }
    }

    // Clear the list of files of its contents.
    files.clear();
    // Fill it once more with all the directories (AllDirs) in the current folder but without the special '.' and '..'
    // folder entries (NoDotAndDotDot).
    files = sourcedir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);

    // Loop through each file:
    for(int i = 0; i< files.count(); i++)
    {
        // Generate the paths for the source and destination directories.
        QString srcname = sourcedirpath + QDir::separator() + files[i];
        QString destname = destdirpath + QDir::separator() + files[i];

        // TODO: Make modular.
        // If the file does not start with the ignore tag ('[ign]'):
        if (!srcname.startsWith("[ign]", Qt::CaseInsensitive))
        {
            // Call the function recursively for the current folder and set the boolean success variable to match the
            // result of the success status of the recursive function call.
            success_and_basemapcopy = copyRecursively(srcname, destname, success_and_basemapcopy);

            // If the recursive copying was not successful, return false.
            if(!success_and_basemapcopy[0])
                return {false, false};
        }
    }

    // We can only reach this point if all folders and files were copied successfully (otherwise, there would have been
    // a return boolean of false, which would have been propagated upwards to the initial call by all the if(!success)
    // checks, regardless of the position in the recursion chain), so check if we copied within /maps/base.

    // Return the QList of bools for the success and basemapcopy.
    return success_and_basemapcopy;
}

// Method to store the command line arguments to use when launching the executable into a QStringList.
QStringList MainWindow::setArguments()
{
    // Make a list of QStrings where we will store the command line arguments for the game.
    QStringList arglist;

    // TODO: Refactor to use other mods.
    // Firstly, add the command line for using the current mod.
    arglist.append("-mod:RTR");
    arglist.append("-noalexander");

    // TODO: Refactor into a loop.
    // Then, manually add each of the checkboxes' respective options if they are currently checked.
    if (ui->showerrBox->isChecked())
        arglist.append("-show_err");
    if (ui->neBox->isChecked())
        arglist.append("-ne");
    if (ui->nmBox->isChecked())
        arglist.append("-nm");
    if (ui->multirunBox->isChecked())
        arglist.append("-multirun");
    if (ui->editorBox->isChecked())
        arglist.append("-enable_editor");
    if (ui->moviecamBox->isChecked())
        arglist.append("-movie_cam");
    if (ui->naBox->isChecked())
        arglist.append("-na");
    if (ui->aiBox->isChecked())
        arglist.append("-ai");
    if (ui->spritesBox->isChecked())
        arglist.append("-sprite_script");
    if (ui->swBox->isChecked())
        arglist.append("-sw");

    // Return the list of command-line arguments.
    return arglist;
}

void MainWindow::on_delmapButton_clicked()
{
    // Delete the map.rwm to have the game generate an updated one.
    checkMapRwm(true);
}
