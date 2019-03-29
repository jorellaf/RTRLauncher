#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef OPTIONDATA_H
#include "optiondata.h"
#endif

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QMainWindow>
#include <QCloseEvent>
#include <QComboBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    int startUp();
    void resetChecks();
    ~MainWindow();

private slots:
    void on_launchButton_pressed();

    void on_launchButton_released();

    void on_launchButton_clicked();

    void on_mapButton_clicked();

    void on_preferencesButton_clicked();

    void on_saveButton_clicked();

    void on_discardButton_clicked();

    void on_defaultsButton_clicked();

private:
    Ui::MainWindow *ui;
    OptionData listoptions;
    QList<QCheckBox *> listofprefsmainwindow;
    int filefolderSwitch(QString currentoption);
    PreferencesDialog *preferencesd;
    void closeEvent(QCloseEvent *event);
    bool checkDefaults(Ui::MainWindow *ui);
    QList<PlayerOption> playeroptionlist;
    bool fileExists(QString path);
    bool dirExists(QString path);
    QString rtrPresent();
    int readLauncherData(OptionData *l);
    void setOptions(OptionData *l, QComboBox *comboBoxObject, QString optiontypelocal);
    //void setCampOptions(ListOptionObjects *l, QComboBox *campComboBox);
    void readPlayerData(QList<PlayerOption> *l, MainWindow *w);
    int writePlayerData(QString dataToWrite);
    bool copyRecursively(QString sourceFolder, QString destFolder);
    int filefolderSwitch(QString currentoption, QList<OptionObject> listoptions);
    void readPreferences(PreferencesDialog *p);
};

#endif // MAINWINDOW_H
