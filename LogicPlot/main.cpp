 #include "mainwindow.h"
#include "serial.h"
#include "plot.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Serial serial;

    Plot plot;

    MainWindow w(&serial, &plot);

    w.show();

    //w.updatePlot();

    return a.exec();
}
