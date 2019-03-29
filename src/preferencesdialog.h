#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#ifndef OPTIONDATA_H
#include "optiondata.h"
#endif

#include <QDialog>
#include <QCheckBox>
#include <QList>
#include <QString>
#include <QTextStream>

int writePreferences(QString dataToWrite);

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    void setChangesMadeFalse(){changesmade = false;}
    ~PreferencesDialog();
    QString preferencesText;
    QList<bool> getMods(){return prefmodifiers;}
    QList<QCheckBox *> getListPrefs(){return listofprefs;}

private slots:
    void on_defaultsButton_clicked();

    void on_saveButton_clicked();

//    void on_SHOW_BANNERS_stateChanged(int arg1);

//    void on_DISABLE_ARROW_MARKERS_stateChanged(int arg1);

//    void on_UNIT_EXPERIENCE_UPGRADE_EFFECT_stateChanged(int arg1);

//    void on_MINIMAL_UI_stateChanged(int arg1);

//    void on_UNLIMITED_MEN_ON_BATTLEFIELD_stateChanged(int arg1);

//    void on_EDIT_SETTLEMENT_NAMES_stateChanged(int arg1);

//    void on_AUTO_SAVE_stateChanged(int arg1);

private:
    Ui::PreferencesDialog *ui;
    void reject();
    bool changesmade = false;
    QList<QCheckBox *> listofprefs;
    void resetChecks();
    bool checkDefaults();
    QString preferencesTextGen();
    QList<bool> prefmodifiers;
};

#endif // PREFERENCESDIALOG_H
