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

    CanFunc test_can_funcs;
    if(test_can_funcs.LoadDllFuncs()) {
        w.SetTextInfo("CAN_TO_USB.dll加载成功\n驱动函数链接成功");
    } else {
        QMessageBox::warning(nullptr, "错误", test_can_funcs.GetErrorMsg().c_str(), QMessageBox::Cancel);
    }

    return a.exec();
}
