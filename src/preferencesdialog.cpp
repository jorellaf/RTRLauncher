#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QMessageBox>
#include <QDebug>

/*#ifndef FUNCTIONS_H
#include "functions.h"
#endif*/

void PreferencesDialog::resetChecks()
{
    foreach (QCheckBox *c, listofprefs)
    {
        c->setChecked(false);
    }
}

QString PreferencesDialog::preferencesTextGen()
{
    QString prefText = preferencesText;
    bool valueResult;
    QString valueText;
    int i = 0;
    foreach (QCheckBox *c, listofprefs)
    {
        valueResult = c->isChecked() == prefmodifiers.at(i);
        valueText = valueResult ? "TRUE" : "FALSE";
        //qDebug() << c->objectName();
        int indexoption = prefText.indexOf(c->objectName());
        if (indexoption != -1)
        {
            int lengthproperty = c->objectName().length()+1;
            int nextnl = prefText.indexOf("\n", indexoption);
            if (nextnl!=-1)
            {
                prefText.remove(indexoption+lengthproperty,nextnl-(indexoption+lengthproperty));
                prefText.insert(indexoption+lengthproperty,valueText);
            }
            else
                prefText.insert(indexoption+lengthproperty,valueText);
        }
        else
            prefText.append(c->objectName() + ":" + valueText + "\n");
        i++;
    }
    return prefText;
}

bool PreferencesDialog::checkDefaults()
{
    bool checked = true;
    foreach (QCheckBox *c, listofprefs)
    {
        //qDebug() << c->objectName();
        checked = checked && !c->checkState();
    }
    return checked;
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    this->setLocale(QLocale::system());

    prefmodifiers = {false,true,false,true,true,true,false};
    listofprefs = {ui->SHOW_BANNERS, ui->DISABLE_ARROW_MARKERS, ui->UNIT_EXPERIENCE_UPGRADE_EFFECT, ui->MINIMAL_UI,
                            ui->UNLIMITED_MEN_ON_BATTLEFIELD, ui->AUTO_SAVE};
    resetChecks();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_defaultsButton_clicked()
{
    if (!checkDefaults())
    {
    QMessageBox::StandardButton confirm;
      confirm = QMessageBox::warning(this, "Confirm", "Are you sure you want to reset to the default settings?\nYour previous settings will still be stored in the mod's\npreferences file unless you manually save or launch the game.",
                                    QMessageBox::Yes|QMessageBox::No);
      if (confirm == QMessageBox::Yes)
        resetChecks();
    }
}

int PreferencesDialog::writePreferences(QString dataToWrite)
{
    QFile preferencesData(QCoreApplication::applicationDirPath() + preferencesFile);
    if (!preferencesData.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        QMessageBox::critical(this,"File not writeable", "The preferences file could not be written. Please check your RTR installation and make sure the preferences file"\
                                                     " is accessible.\n\nError: " + preferencesData.errorString());
        return 400;
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

void PreferencesDialog::on_saveButton_clicked()
{
    QString preferencedata = preferencesTextGen();
    if (writePreferences(preferencedata) == 0)
        QMessageBox::information(this,"Success", "Your settings have been successfully saved.", QMessageBox::Ok);
    //this->close();
}

void PreferencesDialog::reject()
{
        done(0);
}

void PreferencesDialog::on_discardButton_clicked()
{
    // TODO: Add confirmation dialog.
    // TODO: Add check for changes.
    // TODO: Add button tooltips.
    this->close();
}
