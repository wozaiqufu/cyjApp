#include "mainwindow.h"
#include <QApplication>
//#include <QVector>
#include "cyjdata.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<CYJData>("CYJData");
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
