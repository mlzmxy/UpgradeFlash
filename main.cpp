#include <QApplication>
#include <QLibrary>
#include <QString>
#include <QMessageBox>
#include <QDebug>

#include "mainwindow.h"
#include "canfunc.h"

unsigned int flag_dll_funcs = 0;  //驱动及函数加载标志

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    CanFunc test_can_funcs;
    if(test_can_funcs.LoadDllFuncs()) {
        flag_dll_funcs = 1;
        w.SetTextInfo("CAN_TO_USB.dll加载成功\n驱动函数链接成功");
    } else {
        QMessageBox::warning(NULL, "错误", test_can_funcs.GetErrorMsg().c_str(), QMessageBox::Cancel);
    }

    return a.exec();
}
