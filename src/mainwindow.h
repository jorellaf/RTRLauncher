#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef DATAHEADER_H
#include "dataheader.h"
#endif

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QMainWindow>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
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
    ListOptionObjects listoptions;
    QList<QCheckBox *> listofprefsmainwindow;
    int filefolderSwitch(QString currentoption);
    PreferencesDialog *preferencesd;
    void MainWindow::closeEvent(QCloseEvent *event);
    bool checkDefaults(Ui::MainWindow *ui);
    QList<PlayerOption> playeroptionlist;
};

#endif // MAINWINDOW_H
