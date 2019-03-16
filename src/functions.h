#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifndef DATAHEADER_H
#include "dataheader.h"
#endif

#ifndef MAINWINDOW_H
#include "mainwindow.h"
#endif

#include <QString>
#include <QList>
#include <QComboBox>

class Functions
{
public:
    bool fileExists(QString path);
    bool dirExists(QString path);
    QString rtrPresent();
    int readLauncherData(ListOptionObjects *l);
    void setOptions(ListOptionObjects *l, QComboBox *comboBoxObject, QString optiontypelocal);
    //void setCampOptions(ListOptionObjects *l, QComboBox *campComboBox);
    void readPlayerData(QList<PlayerOption> *l, MainWindow *w);
    int writePlayerData(QString dataToWrite);
    bool copyRecursively(QString sourceFolder, QString destFolder);
    int filefolderSwitch(QString currentoption, QList<OptionObject> listoptions);
    void readPreferences(PreferencesDialog *p);
    int writePreferences(QString dataToWrite);
};

#endif // FUNCTIONS_H
