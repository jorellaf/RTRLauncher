#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

// Including the data handler structs and classes from optiondata.h
#ifndef OPTIONDATA_H
#include "optiondata.h"
#endif

// Include the QDialog class to use it for our preferences dialog UI element.
#include <QDialog>

// Include some QWidget classes for handling data.
#include <QCheckBox>
#include <QList>
#include <QString>
#include <QTextStream>

//
// Standard QT UI window class declaration inside the Ui namespace.
namespace Ui {
class PreferencesDialog;
}

//
// Create a class for the main window of the launcher, with a public inheritance from the QMainWindow class.
class PreferencesDialog : public QDialog
{
    // Qt Q_OBJECT macro for the Qt Meta-Object Compiler.
    Q_OBJECT

public:
    // Constructor for the UI window element class (parent window intialised to none by default; *parent = nullptr).
    explicit PreferencesDialog(QWidget *parent = nullptr);

    // Destructor method.
    ~PreferencesDialog();

    // QString containing the contents of the preferences file.
    QString preferencesText;

    // 'Setter' method to set the private changesmade variable to false.
    //void setChangesMadeFalse(){ changesmade = false; }

    // 'Getter' method to obtain the QList of the boolean modifiers for the the default settings of each checkbox.
    QList<bool> getMods(){ return prefmodifiers; }

    // 'Getter' method to obtain the QList pointing to the checkboxes in the preferences dialog.
    QList<QCheckBox *> getListCheckboxes(){ return listofcheckboxes; }

    // Method to write the data to the preferences file.
    int writePreferences(QString dataToWrite);

private slots:
    // Slot for the restore defaults button.
    void on_defaultsButton_clicked();

    // Slot for the save settings button.
    void on_saveButton_clicked();

    // Slot for the discard settings button.
    void on_discardButton_clicked();

private:
        // Variables.
    // MainWindow class.
    Ui::PreferencesDialog *ui;

    // Boolean to indicate whether any changes to the settings have been made by the user.
    bool changesmade = false;

    // QList of QCheckBox pointers of the options checkboxes in the preferences dialog.
    QList<QCheckBox *> listofcheckboxes;

    // QList of boolean modifiers (masks) for the the default settings of each checkbox.
    QList<bool> prefmodifiers;

        // Methods.
    // Method to check whether the user's configuration is different from the defaults.
    bool checkDefaults();

    // Method to reset the checkboxes to their default states (reset to defaults).
    void resetChecks();

    // Method to generate the preferences text.
    QString preferencesTextGen();
};

#endif // PREFERENCESDIALOG_H
