#include <QApplication>
#include <QLibrary>
#include <QString>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "canfunc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
