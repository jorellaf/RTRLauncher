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

    int writePreferences(QString dataToWrite);

private slots:
    void on_defaultsButton_clicked();

    void on_saveButton_clicked();

    void on_discardButton_clicked();

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
