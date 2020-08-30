#include "mainwindow.h"
#include "ui_mainwindow.h"

/*静态变量只能定义在cpp文件，且在类外。static意味着该变量的作用域只限于定义它的源文件中，其它源文件不能访问。
 *如果出现在头文件中，那包含了该头文件的所有源文件中都定义了这些变量，即该头文件被包含了多少次，这些变量就定义了多少次。
 *这时矛盾的*/
#define FixedColumnCount 6  //文件固定6列
static QSerialPort *my_serialport=new QSerialPort();
static double partnumber;//工件序号
static double picturenumber;//图片序号
static int slotnumber;//8/12/16选择
static double mainnorm;//工件标准
static double sidenorm;
static double mainerr;
static double sideerr;

/*构造函数：执行父类QMainWindow的构造函数，创建一个Ui::MainWindow 类的对象ui。这个ui就是mainwindow.h中定义的指针变量ui*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);//是由.ui文件生成的类的构造函数，这个函数的作用是对界面进行最开始的初始化
    this->initValue();//初始化，执行void MainWindow::initValue()

}

/*构析函数：简单地删除用new创建的指针ui*/
MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化(程序开始就运行了)
void MainWindow::initValue()
{

    // 创建系统托盘
    syetemTray = new SystemTray(this);
    connect(syetemTray->getShowWidget(), SIGNAL(triggered(bool)), this, SLOT(showNormal()));
    connect(syetemTray->getMinWidget(), SIGNAL(triggered(bool)), this, SLOT(showMinimized()));
    // 标题栏
    ui->widgetTitle->setParentWidget(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 图片插入到startButton和stopButton
    QIcon icon1,icon2;
    icon1.addFile(tr(":/image/image/open.png"));
    ui->startButton->setIcon(icon1);
    icon2.addFile(tr(":/image/image/stop.png"));
    ui->stopButton->setIcon(icon2);

    // 数据栏
    //数据模型和选择模型的创建，以及与视图组件的关联等设置
    theModel = new QStandardItemModel(1, FixedColumnCount, this); //数据模型
    theSelection = new QItemSelectionModel(theModel);//创建数据选择模型时需要传递一个数据模型变量作为其参数。数据选择模型theSelection是theModel的项数据选择操作。

    //创建数据模型和选择模型后，为TableView组件设置数据模型和选择模型
    ui->tableView->setModel(theModel); //设置数据模型
    ui->tableView->setSelectionModel(theSelection); //设置选择模型
    ui->tableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectItems);

    /*工件标准初始化*/
    QStringList slotList;
    QStringList mainwidthList;
    QStringList sidewidthList;
    QStringList mainerrorList;
    QStringList sideerrorList;

    slotList<<"8"<<"12"<<"16";
    ui->slotBox->addItems(slotList);
    ui->slotBox->setCurrentIndex(0);

    mainwidthList<<"4mm"<<"5mm"<<"6mm";
    ui->mainwidthBox->addItems(mainwidthList);
    ui->mainwidthBox->setCurrentIndex(0);

    sidewidthList<<"0.1mm"<<"0.2mm"<<"0.3mm"<<"0.4mm"<<"0.5mm";
    ui->sidewidthBox->addItems(sidewidthList);
    ui->sidewidthBox->setCurrentIndex(2);

    mainerrorList<<"0.01mm"<<"0.02mm"<<"0.03mm"<<"0.04mm"<<"0.05mm";
    ui->mainerrorBox->addItems(mainerrorList);
    ui->mainerrorBox->setCurrentIndex(1);

    sideerrorList<<"0.01mm"<<"0.02mm"<<"0.03mm"<<"0.04mm"<<"0.05mm";
    ui->sideerrorBox->addItems(sideerrorList);
    ui->sideerrorBox->setCurrentIndex(1);

    /*测量数据初始化*/
    ui->lcdPartNumber->setDigitCount(5);
    ui->lcdPartNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdPictureNumber->setDigitCount(4);
    ui->lcdPictureNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdMainwidth->setDigitCount(7);
    ui->lcdMainwidth->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdSidewidth1->setDigitCount(7);
    ui->lcdSidewidth1->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdSidewidth2->setDigitCount(7);
    ui->lcdSidewidth2->setSegmentStyle(QLCDNumber::Flat);

    partnumber=0;
    double picturenumber=0;
    double mainwidth=0;
    double sidewidth1=0;
    double sidewidth2=0;
    ui->lcdPartNumber->display(partnumber);
    ui->lcdPictureNumber->display(picturenumber);
    ui->lcdMainwidth->display(mainwidth);
    ui->lcdSidewidth1->display(sidewidth1);
    ui->lcdSidewidth2->display(sidewidth2);

    /*串口初始化*/
    //读取串口信息
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        // 自动读取串口号添加到端口portBox中
        my_serialport->setPort(info);
        if(my_serialport->open(QIODevice::ReadWrite))
        {
            ui->portBox->addItem(info.portName());
            my_serialport->close();
        }
    }
    QStringList baudList;   //波特率
    QStringList parityList; //校验位
    QStringList dataBitsList;   //数据位
    QStringList stopBitsList;   //停止位
    // 波特率    //波特率默认选择下拉第三项：9600
    baudList<<"1200"<<"2400"<<"4800"<<"9600"
           <<"14400"<<"19200"<<"38400"<<"56000"
          <<"57600"<<"115200";
    ui->rateBox->addItems(baudList);
    ui->rateBox->setCurrentIndex(3);
    // 校验       //校验默认选择无
    parityList<<"无"<<"奇"<<"偶";
    ui->checkBox->addItems(parityList);
    ui->checkBox->setCurrentIndex(0);
    // 数据位      //数据位默认选择8位
    dataBitsList<<"5"<<"6"<<"7"<<"8";
    ui->dataBox->addItems(dataBitsList);
    ui->dataBox->setCurrentIndex(3);
    // 停止位      //停止位默认选择1位
    stopBitsList<<"1"<<"2";
    ui->stopBox->addItems(stopBitsList);
    ui->stopBox->setCurrentIndex(0);

}

/*主界面*/
//打开串口
void MainWindow::on_startButton_clicked()
{
    if(my_serialport->isOpen())//如果串口已经打开了,先给他关闭了
    {
        my_serialport->clear();
        my_serialport->close();
    }

    my_serialport->setPortName(ui->portBox->currentText());//当前选择的串口名字

    if(my_serialport->open(QIODevice::ReadWrite))//用ReadWrite的模式尝试打开串口
    {
        QMessageBox::information(NULL,"提示","打开成功",QMessageBox::Yes|QMessageBox::No);
        // 设置波特率
        my_serialport->setBaudRate(ui->rateBox->currentText().toInt());
        // 设置数据位数
        switch(ui->dataBox->currentIndex())
        {
        case 5: my_serialport->setDataBits(QSerialPort::Data5); break;
        case 6: my_serialport->setDataBits(QSerialPort::Data6); break;
        case 7: my_serialport->setDataBits(QSerialPort::Data7); break;
        case 8: my_serialport->setDataBits(QSerialPort::Data8); break;
        default: break;
        }
        // 设置奇偶校验
        switch(ui->checkBox->currentIndex())
        {
        case 0: my_serialport->setParity(QSerialPort::NoParity); break;
        case 1: my_serialport->setParity(QSerialPort::OddParity); break;
        case 2: my_serialport->setParity(QSerialPort::EvenParity); break;
        default: break;
        }
        // 设置流控制
        my_serialport->setFlowControl(QSerialPort::NoFlowControl);
        // 设置停止位
        switch(ui->stopBox->currentIndex())
        {
        case 1: my_serialport->setStopBits(QSerialPort::OneStop); break;
        case 2: my_serialport->setStopBits(QSerialPort::TwoStop); break;
        default: break;
        }
        // 串口下拉菜单控件使能
        ui->portBox->setEnabled(false);
        ui->rateBox->setEnabled(false);
        ui->dataBox->setEnabled(false);
        ui->checkBox->setEnabled(false);
        ui->stopBox->setEnabled(false);
        // 数据标准下拉菜单使能
        ui->slotBox->setEditable(false);
        ui->mainwidthBox->setEditable(false);
        ui->sidewidthBox->setEditable(false);
        ui->mainerrorBox->setEditable(false);
        ui->sideerrorBox->setEditable(false);


        // 选择8/12/16个槽数
        if(ui->slotBox->currentIndex()==0)
            slotnumber=8;
        else if(ui->slotBox->currentIndex()==1)
            slotnumber=12;
        else
            slotnumber=16;

        // 主宽度标准
        if(ui->mainwidthBox->currentIndex()==0)
            mainnorm=4;
        else if(ui->mainwidthBox->currentIndex()==1)
            mainnorm=5;
        else
            mainnorm=6;
        // 侧宽度标准
        if(ui->sidewidthBox->currentIndex()==0)
            sidenorm=0.1;
        else if(ui->sidewidthBox->currentIndex()==1)
            sidenorm=0.2;
        else if(ui->sidewidthBox->currentIndex()==2)
            sidenorm=0.3;
        else if(ui->sidewidthBox->currentIndex()==3)
            sidenorm=0.4;
        else
            sidenorm=0.5;
        // 主允许误差
        if(ui->mainerrorBox->currentIndex()==0)
            mainerr=0.01;
        else if(ui->mainerrorBox->currentIndex()==1)
            mainerr=0.02;
        else if(ui->mainerrorBox->currentIndex()==2)
            mainerr=0.03;
        else if(ui->mainerrorBox->currentIndex()==3)
            mainerr=0.04;
        else
            mainerr=0.05;
        //侧允许误差
        if(ui->sideerrorBox->currentIndex()==0)
            sideerr=0.01;
        else if(ui->sideerrorBox->currentIndex()==1)
            sideerr=0.02;
        else if(ui->sideerrorBox->currentIndex()==2)
            sideerr=0.03;
        else if(ui->sideerrorBox->currentIndex()==3)
            sideerr=0.04;
        else
            sideerr=0.05;

    }
    else
    {
        QMessageBox::information(NULL,"提示","打开失败",QMessageBox::Yes|QMessageBox::No);
        return;
    }

    /*定时器初始化*/
    fTimer=new QTimer(this);
    fTimer->stop();
    fTimer->setInterval(120000/slotnumber);//时间间隔  2分钟÷slotnumber
    connect(fTimer,SIGNAL(timeout()),this,SLOT(timer_timerout()));

/*将my_serialport对象的readyRead()信号与另外一个this对象(本部件，即Widget)的槽函数receiveInfo()相关联，这样当发射者发射信号时，接收者的槽函数将被调用*/
    connect(my_serialport,SIGNAL(readyRead()),this,SLOT(receiveInfo()));

}
// 关闭串口
void MainWindow::on_stopButton_clicked()
{
    my_serialport->close();
    // 串口下拉按键使能
    ui->portBox->setEnabled(true);
    ui->rateBox->setEnabled(true);
    ui->dataBox->setEnabled(true);
    ui->checkBox->setEnabled(true);
    ui->stopBox->setEnabled(true);
    // 数据标准下拉菜单使能
    ui->slotBox->setEditable(true);
    ui->mainwidthBox->setEditable(true);
    ui->sidewidthBox->setEditable(true);
    ui->mainerrorBox->setEditable(true);
    ui->sideerrorBox->setEditable(true);

    //关闭定时器
    fTimer->stop();
}
// 接收到单片机发送的数据进行解析
void MainWindow::receiveInfo()
{
    //工件序号+1
    partnumber+=1;
    ui->lcdPartNumber->display(partnumber);
    //图片序号初始化为0
    picturenumber=0;
    //延时0.2s
    QTime n=QTime::currentTime();
    QTime now;
    do{
        now=QTime::currentTime();
    }   while(n.msecsTo(now)<=200);

    fTimer->start();//开始定时拍摄(第一次timeout才开始)
    timer_timerout();//定时开始时，先立刻开始拍照一次

}

/*定时操作，拍摄得到1张图片，3个宽度，并处理数据，进行相应的界面显示*/
void MainWindow::timer_timerout()
{
    //开始拍照

    //获取的图片和计算结果
    QImage img;
    double mainwidth;
    double sidewidth1;
    double sidewidth2;

    //质量
    QString quality;

    //图片序号+1
    picturenumber+=1;
    ui->lcdPictureNumber->display(picturenumber);

    //显示图片
    ui->picture_show_label->setPixmap(QPixmap::fromImage(img));//img转为像素图，放在picture_show_label中

    //三个宽度数据显示
    ui->lcdMainwidth->display(mainwidth);
    ui->lcdSidewidth1->display(sidewidth1);
    ui->lcdSidewidth2->display(sidewidth2);

    //报警灯
    QPixmap green(pic.lightChange(0));
    QPixmap red(pic.lightChange(1));
    if(mainwidth<mainnorm-mainerr)//主宽度过小
    {
        ui->light_main_s->setPixmap(red);
        ui->light_main_l->setPixmap(green);
    }
    else if(mainwidth>mainnorm+mainerr)//主宽度过大
    {
        ui->light_main_s->setPixmap(green);
        ui->light_main_l->setPixmap(red);
    }
    else
    {
        ui->light_main_s->setPixmap(green);
        ui->light_main_l->setPixmap(green);
    }

    if(sidewidth1<sidenorm-sideerr)//侧宽度1过小
    {
        ui->light_side1_s->setPixmap(red);
        ui->light_side1_l->setPixmap(green);
    }
    else if(sidewidth1>sidenorm+sideerr)//侧宽度1过大
    {
        ui->light_side1_s->setPixmap(green);
        ui->light_side1_l->setPixmap(red);
    }
    else
    {
        ui->light_side1_s->setPixmap(green);
        ui->light_side1_l->setPixmap(green);
    }

    if(sidewidth2<sidenorm-sideerr)//侧宽度2过小
    {
        ui->light_side2_s->setPixmap(red);
        ui->light_side2_l->setPixmap(green);
    }
    else if(sidewidth2>sidenorm+sideerr)//侧宽度2过大
    {
        ui->light_side2_s->setPixmap(green);
        ui->light_side2_l->setPixmap(red);
    }
    else
    {
        ui->light_side2_s->setPixmap(green);
        ui->light_side2_l->setPixmap(green);
    }

    //根据主宽度判断工件是否合格
    if(mainwidth>mainnorm+mainerr)
        quality="偏大";
    else if(mainwidth<mainnorm-mainerr)
        quality="偏小";
    else
        quality="合格";

    //将数据记录在theModel上,显示在tabelview
    QList<QStandardItem*>  aItemList; //容器类
    QStandardItem *aItem1=new QStandardItem(QString::number(partnumber));
    QStandardItem *aItem2=new QStandardItem(QString::number(picturenumber));
    QStandardItem *aItem3=new QStandardItem(QString::number(mainwidth));
    QStandardItem *aItem4=new QStandardItem(QString::number(sidewidth1));
    QStandardItem *aItem5=new QStandardItem(QString::number(sidewidth2));
    QStandardItem *aItem6=new QStandardItem(quality);
    aItemList<<aItem1<<aItem2<<aItem3<<aItem4<<aItem5<<aItem6;
    theModel->insertRow(theModel->rowCount(),aItemList); //插入一行，需要每个Cell的Item
    QModelIndex curIndex=theModel->index(theModel->rowCount()-1,0);//创建最后一行的ModelIndex
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行

    //当完成所有拍摄时，停止工作
    if(picturenumber==slotnumber)
        fTimer->stop();

}


/*图片栏*/
// 保存图片
void MainWindow::on_save_Button_clicked()
{
    QString dirPath = QFileDialog::getSaveFileName(NULL, QStringLiteral("ImageSaveAs"), "D:\\QTprojects\\demo02\\Image_save\\bad.png", tr("*.bmp;; *.png;; *.jpg;; *.tif;; *.GIF"));
    QImage img=ui->picture_show_label->pixmap()->toImage();
    if (dirPath.isEmpty())
    {
        return;
    }
    else
    {
        if (!(img.save(dirPath))) //保存图像
        {
            QMessageBox::information(this,tr("提示"),tr("保存图片失败!"));
            return;
        }
    }

}
// 从保存的库图片中读取图片
void MainWindow::on_read_Button_clicked()
{
    QString dirPath=QFileDialog::getOpenFileName(this,tr("选择图像"),"D:\\QTprojects\\demo02\\Image_save\\",tr("Images (*.png *.bmp *.jpg)"));
        if(dirPath.isEmpty())
            return;
        else
        {
            QImage img;
            if(!(img.load(dirPath))) //加载图像
            {
                QMessageBox::information(this, tr("提示"),tr("打开图像失败!"));
                return;
            }
            ui->picture_show_label->setPixmap(QPixmap::fromImage(img.scaled(ui->picture_show_label->size())));
        }
}

// 清空图片
void MainWindow::on_clear_Button_clicked()
{
    ui->picture_show_label->clear();
}

/*数据栏*/
/*打开文件:这段代码让用户选择所需要打开的数据文本文件，然后用只读和文本格式打开文件，逐行读取其内容，并且添加到一个临时的QStringList类型的变量fFileContent里。*/
void MainWindow::on_openfileButton_clicked()
{
    QString curPath="D:\\QTprojects\\demo02";
    //调用打开文件对话框打开一个文件
    QString aFileName=QFileDialog::getOpenFileName(this,"打开一个文件",curPath, "数据文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty())
        return; //如果未选择文件，退出
    QStringList fFileContent;//文件内容字符串列表
    QFile aFile(aFileName);  //以文件方式读出
    if (aFile.open(QIODevice::ReadOnly | QIODevice::Text)) //以只读文本方式打开文件
    {
        QTextStream aStream(&aFile); //QTextStream与IO读写设备结合，为数据读写提供了一些方便的方法的类，以流的方式读写文件
        while(!aStream.atEnd())
        {
            QString line=aStream.readLine();//读取文件的一行
            fFileContent.append(line); //添加到QStringList类型的fFileContent
        }
        aFile.close();//关闭文件
        iniModelFromStringList(fFileContent);//从StringList的内容初始化数据模型
    }

}

void MainWindow::iniModelFromStringList(QStringList& aFileContent)
{
    //从一个StringList 获取数据，初始化数据Model
    int rowCnt=aFileContent.count(); //文本行数，第1行是标题
    theModel->setRowCount(rowCnt-1); //实际数据行数
    //设置表头
    QString header=aFileContent.at(0);//第1行是表头
    //一个或多个空格、TAB等分隔符隔开的字符串，分解为一个StringList
    QStringList headerList=header.split(QRegExp("\\s+"),QString::SkipEmptyParts);
    theModel->setHorizontalHeaderLabels(headerList); //设置表头文字
    //设置表格数据
    int j;
    QStandardItem   *aItem;
    for (int i=1;i<rowCnt;i++)
    {
        QString aLineText=aFileContent.at(i); //获取数据区的一行
        //一个或多个空格、TAB等分隔符隔开的字符串，分解为一个StringList
        QStringList tmpList=aLineText.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        for (j=0;j<FixedColumnCount;j++) //tmpList的列数等于FixedColumnCount
        {
            aItem=new QStandardItem(tmpList.at(j));//创建item
            theModel->setItem(i-1,j,aItem); //为模型的某个行列位置设置Item
        }
    }
}

///*备用的自定义函数iniModelFromStringList()*/
//void MainWindow::iniModelFromStringList(QStringList& aFileContent)
//{
//    //从一个StringList 获取数据，初始化数据Model
//    int rowCnt=aFileContent.count(); //文本行数，第1行是标题
//    theModel->setRowCount(rowCnt-1); //实际数据行数
//    //设置表头
//    QString header=aFileContent.at(0);//第1行是表头
//    //一个或多个空格、TAB等分隔符隔开的字符串，分解为一个StringList
//    QStringList headerList=header.split(QRegExp("\\s+"),QString::SkipEmptyParts);
//    theModel->setHorizontalHeaderLabels(headerList); //设置表头文字
//    //设置表格数据
//    for(int i=1;i<rowCnt;i++)
//    {
//        QString aLineText=aFileContent.at(i);//获取数据区的一行
//        QStringList tmpList=aLineText.split(QRegExp("\\s+"),QString::SkipEmptyParts);//一行数据转为QStringList
//        for(int j=0;j<FixedColumnCount;j++)
//        {
//            QModelIndex index = theModel->index(i, j);
//            theModel->setData(index, tmpList.at(j), Qt::DisplayRole);//文本形式程序数据
//            theModel->setData(index, int(Qt::AlignLeft | Qt::AlignVCenter),Qt::TextAlignmentRole);//对齐方式(水平方向在左，垂直方向在中)
//        }
//    }
//}

//添加行
void MainWindow::on_adddataButton_clicked()
{
    QList<QStandardItem*>  aItemList; //容器类
    QStandardItem  *aItem;
    for(int i=0;i<FixedColumnCount;i++)
    {
        aItem=new QStandardItem("0"); //创建Item
        aItemList<<aItem;   //添加到容器
    }
    theModel->insertRow(theModel->rowCount(),aItemList); //插入一行，需要每个Cell的Item
    QModelIndex curIndex=theModel->index(theModel->rowCount()-1,0);//创建最后一行的ModelIndex
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行
}

//插入行
void MainWindow::on_insertdataButton_clicked()
{
    QList<QStandardItem*>  aItemList; //容器类
    QStandardItem  *aItem;
    for(int i=0;i<FixedColumnCount;i++)
    {
        aItem=new QStandardItem("0"); //创建Item
        aItemList<<aItem;   //添加到容器
    }
    QModelIndex curIndex=theSelection->currentIndex();//获取当前的模型索引
    theModel->insertRow(curIndex.row()+1,aItemList); //插入一行
    theSelection->clearSelection();//清空选择项
    theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);//设置刚插入的行为当前选择行

}

//删除行
void MainWindow::on_deletedataButton_clicked()
{
    QModelIndex curIndex=theSelection->currentIndex () ;//获取模型索引
    if (curIndex.row()==theModel->rowCount()-1) //最后一行
        theModel->removeRow(curIndex.row()); //删除最后一行
    else
    {
        theModel->removeRow(curIndex.row());//删除一行，并重新设置当前选择行
        theSelection->setCurrentIndex(curIndex,QItemSelectionModel::Select);
    }
}

//保存：在视图组件上对数据的修改都会自动更新到数据模型里，保存按钮可以将数据模型的数据另存为一个数据文本文件
void MainWindow::on_savedataButton_clicked()
{
    QString curPath="D:\\QTprojects\\demo02";
    //调用打开文件对话框选择一个文件
    QString aFileName=QFileDialog::getSaveFileName(this,tr("选择一个文件"),curPath,
                 "井斜数据文件(*.txt);;所有文件(*.*)");
    if (aFileName.isEmpty()) //未选择文件，退出
        return;
    QFile aFile(aFileName);
    if (!(aFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)))
        return; //以读写、覆盖原有内容方式打开文件
    QTextStream aStream(&aFile); //用文本流读取文件
    QStandardItem *aItem;
    int i,j;
    QString str;
    //获取表头文字
    for (i=0;i<theModel->columnCount();i++)
    {
        aItem=theModel->horizontalHeaderItem(i); //获取表头的项数据
        str=str+aItem->text()+"\t\t";  //以TAB见隔开
    }
    aStream<<str<<"\n";  //文件里需要加入换行符\n
    //获取数据区文字
    for ( i=0;i<theModel->rowCount();i++)
    {
        str="";
        for( j=0;j<theModel->columnCount();j++)
        {
            aItem=theModel->item(i,j);
            str=str+aItem->text()+QString::asprintf("\t\t");
        }
         aStream<<str<<"\n";
    }
}


//清空数据
void MainWindow::on_cleardataButton_clicked()
{
    theModel->clear();

}








