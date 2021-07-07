#include "widget.h"
#include "mainwindow.h"

#include <QApplication>

// 程序入口
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    return a.exec();
}
