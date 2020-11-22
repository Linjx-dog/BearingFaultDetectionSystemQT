#include "mainwindow.h"

#include <QApplication>

#include <QtCharts>//显示图表
#include <QDoubleSpinBox>
#include <fftw3.h>
#include <stdlib.h>
#include <QDebug>
#define PI 3.1415926

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
