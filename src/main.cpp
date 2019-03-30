#include <QtPlugin>

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    int result = w.startSetup();
    if (result != 0)
        return result;

    return a.exec();
}
