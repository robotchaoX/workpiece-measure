#include "mainwindow.h"
#include <QApplication>
#include <globaldef.hpp>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);//定义并创建应用程序
    MainWindow mainwindow;//执行到MainWindow构造函数，然后结束
    LOADQSS(GlobalSpace::STYLE_QSS_FILE_PATH);
    mainwindow.showNormal();//显示窗口
    return a.exec();//应用程序一直运行，等待用户操作/事件发生
}
