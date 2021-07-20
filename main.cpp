#include "widget.h"
#include "mainwindow.h"

#include <QApplication>

/** \mainpage 基于OpenGL和Qt的3D点云渲染 - documentation
 *
 *  See:
 *   - <a href="modules.html" >C++ API organized by modules</a>
 *   - <a href="https://github.com/jlblancoc/nanoflann" >Online README</a>
 */

// 程序入口  
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow window;
    window.show();
    return a.exec();
}
