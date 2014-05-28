#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bla3Dscan::MainWindow w;
    w.show();

    return a.exec();
}
