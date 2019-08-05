// Include class headers.
#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

// Include Qt classes used in the code.
#include <QMessageBox>
#include <QDir>

// ==================
// Public methods:

// Constructor for the UI window element class (parent window intialised to none by default; *parent = nullptr).
PreferencesDialog::PreferencesDialog(QWidget *parent) :
    // Default Qt UI window class inheritance.
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    // Default Qt UI setup declaration.
    ui->setupUi(this);
    // Setting the window flags. The preferences dialog should be a window (Qt::Window), it should have a close button
    // but no minimise or maximise (Qt::WindowClose...), and, on Windows, since it is a window of fixed size, it should
    // look as such (Qt::MSWindowsFixedSize...).
    setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    // Set the locale of the preferences dialog to be the system's locale.
    this->setLocale(QLocale::system());

    // Since some of the preferences options are worded negatively (e.g. show banners vs disable auto save), this would
    // make the default checkbox configuration convoluted, since in the default configuration, some would have to be
    // checked, so, to have the default be all the checkboxes unchecked we need to mask their checked status.
    // TODO: Refactor into single variable.
    prefmodifiers = {false,true,false,true,true,true,false};
    listofcheckboxes = {ui->SHOW_BANNERS, ui->DISABLE_ARROW_MARKERS, ui->UNIT_EXPERIENCE_UPGRADE_EFFECT, ui->MINIMAL_UI,
                            ui->UNLIMITED_MEN_ON_BATTLEFIELD, ui->AUTO_SAVE};

    // When the preferences dialog window starts, set the checkboxes to their default status in case there is an error
    // when loading the preferences file, or it does not exists.
    resetChecks();
}

// Destructor method.
PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

// Method to write the data to the preferences file.
// TODO: Refactor with WritePlayerData.
int PreferencesDialog::writePreferences(QString dataToWrite)
{
    // Create a QFile object pointing to the the absolute file path for the mod's RTW preferences file generated using
    // the relative path defined in optiondata.h (since we'll overwrite it, there's no reason to check if it exists).
    QFile preferencesData(QDir::cleanPath(QDir::currentPath() + preferencesFilePath));

    // If the file cannot be opened (or created) in write-only mode, and its contents discarded (with Truncate):
    if (!preferencesData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // Alert the player of the error and return an error code.
        QMessageBox::critical(this,"File not writeable", "The preferences file could not be written. Please check your RTR installation and make sure the preferences file"\
                                                     " is accessible.\n\nError: " + preferencesData.errorString());
        // Very cleverly referencing the HTTP error code for Bad Request.
        return 400;
    }

    // Otherwise, create a QTextStream interface 'out' with a reference to the player data file QFile object.
    QTextStream out(&preferencesData);

    // Set the encoding to UTF-8 for wider script support.
    out.setCodec("UTF-8");

    // Put the data received as a method parameter into the QTextStream (and therefore, the player data file).
    out << dataToWrite;

    // Close the QFile since we finished using it.
    preferencesData.close();

    // Return an a-ok 0, signalling there were no errors.
    return 0;
}

// ==================
// Slots:

// Slot for the restore defaults button.
void PreferencesDialog::on_defaultsButton_clicked()
{
    // If the currently selected settings are not the default ones (checked with checkdefaults()), then:
    if (!checkDefaults())
    {
        // Make a QMessageBox StandardButton object that will store the button the user pressed.
        QMessageBox::StandardButton confirm;

        // TODO: De-deprecate functions.
        // Create a message box asking the player to confirm if they want to discard their changes and restore defaults.
        confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to reset to the default settings?\nYour previous settings will still"\
                                       " be stored in the mod's\npreferences file unless you manually save.", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        // If the user chose yes, then reset the checkboxes to defaults.
        if (confirm == QMessageBox::Yes)
            resetChecks();
    }
}

// Slot for the save settings button.
void PreferencesDialog::on_saveButton_clicked()
{
    // Generate the text of the mod's RTW preferences file with the modification and store them in a QString.
    QString preferencestext = preferencesTextGen();

    // If the writing of the text to the preferences file was successful (returned 0):
    if (writePreferences(preferencestext) == 0)
    {
        // Inform the player and close the preferences dialog window.
        QMessageBox::information(this,"Success", "Your settings have been successfully saved.", QMessageBox::Ok, QMessageBox::Ok);
        this->close();
    }
}

// Slot for the discard settings button.
void PreferencesDialog::on_discardButton_clicked()
{
    // TODO: Add confirmation dialog.
    // TODO: Add check for changes.
    // TODO: Add button tooltips.

    // Make a QMessageBox StandardButton object that will store the button the user pressed.
    QMessageBox::StandardButton confirm;

    // TODO: De-deprecate functions.
    // Create a message box asking the player to confirm if they want to discard their changes and close the window.
    confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to discard your changes?\nYour previous settings will still"\
                                   " be stored in the mod's\npreferences file.", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    // If the user chose yes, then close the preferences dialog window.
    if (confirm == QMessageBox::Yes)
        this->close();
}

// ==================
// Private methods:

// Method to check whether the user's configuration is different from the defaults.
bool PreferencesDialog::checkDefaults()
{
    // Make a bool variable and initialise to true to signal whether the settings are the defaults or not.
    bool checked = true;

    // For each QCheckBox object pointing to each checkbox in the list of checkboxes:
    foreach (QCheckBox *c, listofcheckboxes)
    {
        // The result of checked AND the opposite of the checked/unchecked status of each checkbox makes it so the
        // checked variable is set to false (settings are not the defaults) if even one checkbox is marked, and once it
        // is set to false, it will not be possible for it to go back to true (settings are the defaults).
        checked = checked && !c->isChecked();
    }

    // Return the value of the 'checked' variable.
    return checked;
}

// Method to reset the checkboxes to their default states (reset to defaults).
void PreferencesDialog::resetChecks()
{
    // For each QCheckBox object pointing to each checkbox in the list of checkboxes:
    foreach (QCheckBox *c, listofcheckboxes)
    {
        // Set the checkbox status to unchecked.
        c->setChecked(false);
    }
}

// Method to generate the preferences text.
QString PreferencesDialog::preferencesTextGen()
{
    // Store the contents of the original preferences file to a different QString.
    QString prefstext = preferencesText;

    // Make a boolean to store the masked value of each preference option.
    bool valuemasked = false;

    // Make a QString to store the text equivalent of the boolean value for each preference option.
    QString valuetext;

    // Make a counter variable initialised to 0.
    int i = 0;

    // For each QCheckBox object pointing to each checkbox in the list of checkboxes.
    foreach (QCheckBox *c, listofcheckboxes)
    {
        // The value of valuemasked is the XNOR (==) of the checked status of the checkbox and its respective modifier.
        valuemasked = c->isChecked() == prefmodifiers.at(i);

        // Store the text value of the result of the masking for the preferences file. The syntax (Bool ? X : Y) checks
        // a boolean and assigns a value for the true or false check inline. If the value post-masking is a boolean
        // true, assign the value of X (in this case the string 'TRUE'), and, if not, assign Y (in this case, 'FALSE').
        valuetext = valuemasked ? "TRUE" : "FALSE";

        // Get the position of the option in the preferences text QString by using the name of the current checkbox.
        int indexoption = prefstext.indexOf(c->objectName());

        // Check if the position index is not -1 (hence the option was found in the file).
        if (indexoption != -1)
        {
            // If so, get the lenght of the option's name by using the name of the current checkbox + 1 (for the ':').
            int optionnamelen = c->objectName().length() + 1;

            // Get the position of the next line break to know the end of the line.
            int nextnl = prefstext.indexOf("\n", indexoption);

            // If there is a line break after the option (i.e. it is not -1).
            if (nextnl != -1)
            {
                // Remove, starting after the colon of the current option in the text, the amount of characters between
                // said colon and the end of the line (hence the previous length of either 'TRUE' or 'FALSE').
                prefstext.remove(indexoption + optionnamelen, nextnl - (indexoption + optionnamelen));

                // Now insert, starting from the position of the colon, the text of the option's modified value.
                prefstext.insert(indexoption + optionnamelen, valuetext);
            }

            // Otherwise, if there is no next line break, then we are probably at the end of the file, so insert,
            // starting from the position of the colon, the option's modified value and a line break.
            else
                prefstext.insert(indexoption + optionnamelen, valuetext + "\n");
        }

        // Otherwise, if the option was not found in the original file:
        else
        {
            // Add the option by using the checkbox object's name, a colon, the option's value, and a line break.
            prefstext.append(c->objectName() + ":" + valuetext + "\n");
        }

        // Increase the counter.
        i++;
    }

    // Return our modified text of the mod's RTW preferences file.
    return prefstext;
}
