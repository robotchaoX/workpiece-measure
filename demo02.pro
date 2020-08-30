
QT       += core gui #项目中加入core gui模块。core gui是Qt用于GUI设计的类库模块，如果创建的是控制台（Console）应用程序，就不需要添加core gui
QT       += serialport #串口通讯
#QT      += sql #如果项目中使用到了涉及数据库操作的类就需要用到sql模块，在pro文件中需要增加如下一行

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets #条件执行语句，表示当 Qt 主版本大于 4 时，才加入 widgets 模块

TARGET = demo02  #表示生成的目标可执行文件的名称，即编译后生成的可执行文件是demo03.exe
TEMPLATE = app  #表示项目使用的模板是 app，是一般的应用程序


#SOURCES、HEADERS、FORMS 记录了项目中包含的源程序文件、头文件和窗体文件（.ui 文件）的名称。这些文件列表是Qt Creator自动添加到项目管理文件里面的，用户不需要手动修改
SOURCES += main.cpp\
        mainwindow.cpp \
    titlebar/maintitlebar.cpp \
    titlebar/titlebar.cpp \
    systemtray/systemtray.cpp \
    images_change/images_change.cpp


HEADERS  += mainwindow.h \
    globaldef.hpp \
    titlebar/maintitlebar.h \
    titlebar/titlebar.h \
    systemtray/systemtray.h \
    images_change/images_change.h


FORMS    += mainwindow.ui \
    titlebar/maintitlebar.ui

RESOURCES += \
    res.qrc
