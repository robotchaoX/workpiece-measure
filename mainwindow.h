/*****************************************
 * 功能：最外层主窗体，绑定点击显示响应的页面之间
 * 的信号与槽，以及创建系统托盘
 * ***************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "systemtray/systemtray.h"
#include "images_change/images_change.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QBuffer>
#include <QLCDNumber>
#include <QList>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QStringList>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QByteArray>
#include <QTimer>
#include <QTime>

/*声明了一个名称为Ui的命名空间（namespace），包含MainWindow类，用于描述界面组件的*/
namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT        /*宏Q_OBJECT，这是使用Qt的信号与槽（signal 和 slot）机制的类都必须加入的一个宏*/

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();  /*public类定义了MainWindow类的构造函数和析构函数*/

    Images_Change pic;//创建一个图片切换类作为MainWindow的成员变量，使MainWindow可以使用它

private slots:
    //主界面
    void on_startButton_clicked();
    void on_stopButton_clicked();
    //串口接收信号时相应
    void receiveInfo();
    //图片栏
    void on_save_Button_clicked();
    void on_read_Button_clicked();
    void on_clear_Button_clicked();
    //数据栏
    void on_openfileButton_clicked();
    void on_savedataButton_clicked();
    void on_cleardataButton_clicked();
    void on_adddataButton_clicked();
    void on_insertdataButton_clicked();
    void on_deletedataButton_clicked();

    //定时器
    void timer_timerout();

private:
    Ui::MainWindow *ui;/*指针Ui是用前面声明的namespace Ui里的MainWindow类定义的，所以指针ui是指向可视化设计的界面，要访问界面上的组件，都需要通过这个指针ui*/
    void initValue();
    SystemTray *syetemTray;

    //数据栏
    QStandardItemModel * theModel; //数据模型
    QItemSelectionModel *theSelection; //选择模型
    void iniModelFromStringList (QStringList&) ; //在打开文件时，从一个QStringList变量的内容创建数据模型

    //定时和计时
    QTimer *fTimer;

};

#endif // MAINWINDOW_H
