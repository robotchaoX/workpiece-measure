/*****************************************
 * 功能：主标题栏，内含有窗体最大化，最小化，关闭
 * 以及窗口的Title，以及名称
 * ***************************************/
#include "maintitlebar.h"
#include "ui_maintitlebar.h"
#include <QTimer>
#include <QDateTime>

MainTitleBar::MainTitleBar(QWidget *parent) :
    TitleBar(parent),
    ui(new Ui::MainTitleBar)
{
    ui->setupUi(this);
    ui->pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/image/image/normal_normal.png);}"
                                           "QPushButton:hover{border-image: url(:/image/image/normal_hover.png);}");
    ui->pushButtonClose->setStyleSheet("QPushButton{border-image: url(:/image/image/close_normal.png);}"
                                       "QPushButton:hover{border-image: url(:/image/image/close_hover.png);}");
    ui->pushButtonMin->setStyleSheet("QPushButton{border-image: url(:/image/image/min_normal.png);}"
                                       "QPushButton:hover{border-image: url(:/image/image/min_hover.png);}");
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);
}

MainTitleBar::~MainTitleBar()
{
    delete ui;
}

void MainTitleBar::on_pushButtonClose_clicked()
{
    if(parentWidget)
    {
        parentWidget->close();
    }
}

void MainTitleBar::on_pushButtonMin_clicked()
{
    if(parentWidget)
    {
        parentWidget->showMinimized();
    }
}


void MainTitleBar::on_pushButtonNormalMax_clicked()
{
    if(parentWidget == nullptr) return;
    if (parentWidget->isMaximized())
    {
        parentWidget->showNormal();
        ui->pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/image/image/normal_normal.png);}"
                                               "QPushButton:hover{border-image: url(:/image/image/normal_hover.png);}");
    }
    else
    {
        parentWidget->showMaximized();
        ui->pushButtonNormalMax->setStyleSheet("QPushButton{border-image: url(:/image/image/max_normal.png);}"
                                               "QPushButton:hover{border-image: url(:/image/image/max_hover.png);}");
    }
}
void MainTitleBar::timerUpdate()
{
    QDateTime time = QDateTime::currentDateTime();

    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");

    ui->dateTime->setText(str);
}

