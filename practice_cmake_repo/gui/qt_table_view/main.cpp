#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
//http://www.stellarium.org/wiki/index.php/Compile_with_VS2013_and_Qt5
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
