#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon("./images/virtual-pen.svg"));
    MainWindow w;
    w.show();
    return a.exec();
}
