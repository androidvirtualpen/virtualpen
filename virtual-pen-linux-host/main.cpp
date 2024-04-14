#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    foreach (QString arg, a.arguments()) {
        if(arg == "-d"){
            MainWindow::isDebugMode = true;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}
