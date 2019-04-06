#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Including the data handler structs and classes from optiondata.h
#ifndef OPTIONDATA_H
#include "optiondata.h"
#endif

// Including the preferences dialog UI window class in order to be able to launch it later. in the code.
#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

// Include the QMainWindow class to use it for our own main window UI element.
#include <QMainWindow>
// Include the QComboBox class for one of the method declarations.
#include <QComboBox>

// Standard QT UI window class declaration inside the Ui namespace.
namespace Ui {
class MainWindow;
}

// Create a class for the main window of the launcher, with a public inheritance from the QMainWindow class.
class MainWindow : public QMainWindow
{
    // Qt Q_OBJECT macro for the Qt Meta-Object Compiler.
    Q_OBJECT

public:
    // Constructor for the UI window element class (parent window intialised to none by default; *parent = nullptr).
    explicit MainWindow(QWidget *parent = nullptr);

    // Method for the initial loading of variables from the player data file and set-up of previously selected options.
    int startSetup();

private slots:
    // Slots for the main launcher button.
    void on_launchButton_pressed();
    void on_launchButton_released();
    void on_launchButton_clicked();

    // Slot for the map.rwm checker button.
    void on_mapButton_clicked();

    // Slot for the button to show the editor of the mod's RTW preferences.txt file.
    void on_preferencesButton_clicked();

    // Slot for the save settings button.
    void on_saveButton_clicked();

    // Slot for the discard settings button.
    void on_discardButton_clicked();

    // Slot for the restore defaults button.
    void on_defaultsButton_clicked();

private:
            // Variables.
    // MainWindow class.
    Ui::MainWindow *ui;

    // OptionData class for handling the list of options for the campaigns, EDUs, and trees selectors.
    OptionData launcheroptionslist;

    // Declare a QList of PlayerOption structs for the options of the player data file (TODO: refactor).
    QList<PlayerOption> playeroptionslist;

    // Declare a QList of QCheckBox pointers for handling all the checkboxes of the mainwindow.
    QList<QCheckBox *> checkboxeslist;

    // Declare the PreferencesDialog UI window object for handling the preferences dialog.
    PreferencesDialog *prefsdialog;


            // Methods.
        // Check file/folder existence methods:
    // Method to check if a file with the specified absolute path exists and is indeed a file.
    bool fileExists(QString abspath);

    // Method to check if a directory with the specified absolute path exists and is indeed a directory.
    bool dirExists(QString abspath);

    // Method to check if all the requirements for running the mod successfully are present.
    QString launcherReqFilesCheck();


        // Read data methods:
    // Method to read the launcher data file.
    int readLauncherData(OptionData *l);

    // Method to read the player data file.
    void readPlayerData();

    // Method to generate the text of the player data file.
    QString playerDataTextGen();

    // Method to read the mod's RTW preferences.txt file.
    void readPreferences();

    // Method to add the available options of the launcher data file to their respective comboboxes.
    void setOptions(OptionData *l, QComboBox *combobox, QString optiontypelocal);


        // Checking data methods:
    // Method to check whether the user's configuration is different from the defaults.
    bool checkDefaults();

    // Method to check whether the map.rwm file is up to date to the rest of the contents of the other map files.
    int checkMapRwm();

        // Reset options methods:
    // Resets the checkboxes to their default states (reset to defaults).
    void resetChecks();


        // Write data and launch game methods.
    // Method to write to the player data file.
    int writePlayerData(QString dataToWrite);

    // Method to perform the switch between different campaigns, EDUs, and trees options.
    QString filefolderSwitch(QComboBox *combobox, QList<OptionObject> launcheroptionslist);

    // Method to copy folders recursively and paste them recursively to their destination.
    bool copyRecursively(QString sourceFolder, QString destFolder);

    // Method to store the command line arguments to use when launching the executable into a QStringList.
    QStringList setArguments();
};

// TODO: Ask for confirmation to exit when closing the application.

#endif // MAINWINDOW_H
