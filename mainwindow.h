#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QtCharts>
#include <fstream>
#include <iostream>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <ActiveQt/QAxObject>
#include <QDesktopServices>
#include "BPnet.h"

using namespace std;
QT_CHARTS_USE_NAMESPACE
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initWidget();
    void getTimeFactors(QList<double> list,vector<double> &array);
protected slots:

    void onSwitchTime();
    void onSwitchF();
    void onSwitchNet();
    //时域
    void onOpenFileT1();
    void onOpenFileT2();
    //神经网络
    void onOpenFileNormalData();
    void onOpenFileFaultData();
    void onOpenFileNewData();
    void trainNet();
    void predict();
    //频域
    void onOpenFileF1();
    void onOpenFileF2();

private:
    Ui::MainWindow *ui;
    vector<double> array;

    //时域
    QChart *chartT1;
    QChart *chartT2;
    QLineSeries *seriesT1 = new QLineSeries();
    QLineSeries *seriesT2 = new QLineSeries();
    int rowCount;
    int colCount;
    QString fileT1Name;
    QString fileT2Name;
    QList<double> list1;
    QList<double> list2;
    QValueAxis *axisXT1;
    QValueAxis *axisYT1;
    QValueAxis *axisXT2;
    QValueAxis *axisYT2;
    double maxY;
    double maxX;
    double minY=0;
    double minX=0;
    boolean fileOpened = false;
    boolean fileCanOpen = false;

    //频域
    QString fileF1Name;
    QString fileF2Name;
    QLineSeries *seriesF1 = new QLineSeries();
    QLineSeries *seriesF2 = new QLineSeries();
    QScatterSeries *seriesFS1 = new QScatterSeries();
    QChart *chartF1;
    QChart *chartF2;
    QValueAxis *axisXF1;
    QValueAxis *axisYF1;
    QValueAxis *axisXF2;
    QValueAxis *axisYF2;

    //神经网络用
    QChart *chartNet;
    QLineSeries *seriesNet =new QLineSeries();
    vector<double> errors;
    QString fileNormalDataName;
    QString fileFaultDataName;
    QString fileNewDataName;
    BpNet testNet;
    QList<double> listNet;
    boolean isTrainEnd = false;
    QValueAxis *axisXN1;
    QValueAxis *axisYN1;
    boolean normalDataOK = false;
    boolean faultDataOK = false;

    double peak;
    double rms;
    double fuzhi;
    double average;
    double kurtosis;
    double crest;
    double clearance;
    double impulse;
    double shape;
};
#endif // MAINWINDOW_H
