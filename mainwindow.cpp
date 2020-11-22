#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BPnet.h"

#include<QPaintEvent> //用于绘画事件
#include<QtGui> //引入用到的控件
#include <fftw3.h>
#include <iostream>
#include <fstream>
#include <cmath>
#define PI 3.1415926



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

     //总切换
     connect(ui->ButtonSwitchTime, SIGNAL(clicked()), this, SLOT(onSwitchTime()));
     connect(ui->ButtonSwitchF, SIGNAL(clicked()), this, SLOT(onSwitchF()));
     connect(ui->ButtonSwitchNet, SIGNAL(clicked()), this, SLOT(onSwitchNet()));

     //时域分析
     connect(ui->ButtonOpenFileT1, SIGNAL(clicked()), this, SLOT(onOpenFileT1()));
     connect(ui->ButtonOpenFileT2, SIGNAL(clicked()), this, SLOT(onOpenFileT2()));

     //频域分析
     connect(ui->ButtonOpenFileF1, SIGNAL(clicked()), this, SLOT(onOpenFileF1()));
     connect(ui->ButtonOpenFileF2, SIGNAL(clicked()), this, SLOT(onOpenFileF2()));


     //神经网络
     connect(ui->ButtonOpenFileNormalData, SIGNAL(clicked()), this, SLOT(onOpenFileNormalData()));
     connect(ui->ButtonOpenFileFaultData, SIGNAL(clicked()), this, SLOT(onOpenFileFaultData()));
     connect(ui->ButtonOpenFileNewData, SIGNAL(clicked()), this, SLOT(onOpenFileNewData()));
     connect(ui->ButtonTrainNet, SIGNAL(clicked()), this, SLOT(trainNet()));
     connect(ui->ButtonPredict, SIGNAL(clicked()), this, SLOT(predict()));

     initWidget();
     srand((unsigned)time(NULL));
     this->setWindowTitle("轴承故障分析系统");

}
MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initWidget(){
    chartT1 = new QChart();
    chartT1->setTheme(QChart::ChartThemeBlueNcs);//设置系统主题
    ui->widgetT1->setChart(chartT1);
    chartT1->setTitle("正常轴承时域图像");

    chartT2 = new QChart();
    chartT2->setTheme(QChart::ChartThemeBlueNcs);//设置系统主题
    ui->widgetT2->setChart(chartT2);
    chartT2->setTitle("故障轴承时域图像");

    chartF1 = new QChart();
    chartF1->setTheme(QChart::ChartThemeBlueNcs);//设置系统主题
    ui->widgetF1->setChart(chartF1);
    chartF1->setTitle("正常轴承频域图像");

    chartF2 = new QChart();
    chartF2->setTheme(QChart::ChartThemeBlueNcs);//设置系统主题
    ui->widgetF2->setChart(chartF2);
    chartF2->setTitle("故障轴承频域图像");

    chartNet = new QChart();
    chartNet->setTheme(QChart::ChartThemeBlueNcs);//设置系统主题
    ui->widgetNet->setChart(chartNet);
    chartNet->setTitle("人工神经网络训练");

}
void MainWindow::onOpenFileT1(){
    fileT1Name = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileT1PathLabel->setText(fileT1Name);
    minY=0;minX=0;
    chartT1->removeSeries(seriesT1);
    seriesT1->clear();
    list1.clear();

    ifstream fileStream(fileT1Name.toStdString());
    if(fileStream.fail())//文件打开失败
        {
            ui->textBrowser->insertPlainText("文件不存在，请重新选择！\n");
            fileStream.close();
            system("pause");
        }
        else{
        ui->textBrowser->insertPlainText("正常数据读取中...\n");
                rowCount =1;	//初始化当前行数为0
                double tmp = 0;	//当前读入的数值
                while (!fileStream.eof()){
                    fileStream >> tmp;
                    seriesT1->append(rowCount, tmp);
                    list1.append(tmp);
                    if(maxY<tmp){
                        maxY = tmp;
                    }

                    if(minY>tmp){
                        minY = tmp;
                    }
                    rowCount++;	// 换下一行
                }
                fileStream.close();
        }
    rowCount=rowCount-1;

   //绘制信号图
   chartT1->setTitle(tr("正常数据信号图"));
   seriesT1->setName("检测信号1");

   //创建坐标
   axisXT1 = new QValueAxis;//X轴
   axisXT1->setRange(0, rowCount);//设置坐标轴范围
   axisXT1->setTitleText("sampling point");//标题
   axisXT1->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
   axisXT1->setTickCount(101); //主分隔个数：0到10分成20个单位
   axisXT1->setMinorTickCount(10); //每个单位之间绘制了多少虚网线
   axisXT1->setGridLineVisible(true);
   axisYT1 = new QValueAxis; //Y 轴
   axisYT1->setRange(minY,maxY);
   axisYT1->setTitleText("value");
   axisYT1->setLabelFormat("%.1f"); //标签格式
   axisYT1->setTickCount(11);
   axisYT1->setMinorTickCount(0);
   //为序列设置坐标轴
   chartT1->addSeries(seriesT1);
   chartT1->createDefaultAxes();
   chartT1->setAxisX(axisXT1,seriesT1);
   chartT1->setAxisY(axisYT1,seriesT1);
   ui->textBrowser->insertPlainText("正常信号图显示成功\n");
    //计算指标
    array.clear();
    getTimeFactors(list1,array);
    ui->lineEditKurtosisT1->setText(QString::number(array[0], 'f', 2));
    ui->lineEditCrestT1->setText(QString::number(array[1], 'f', 2));
    ui->lineEditClearanceT1->setText(QString::number(array[2], 'f', 2));
    ui->lineEditImpulseT1->setText(QString::number(array[3], 'f', 2));
    ui->lineEditShapeT1->setText(QString::number(array[4], 'f', 2));
}
void MainWindow::onOpenFileT2(){
    fileT2Name = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileT2PathLabel->setText(fileT2Name);
    if(fileT2Name.isEmpty()){
        fileCanOpen = false;
        ui->textBrowser->insertPlainText("请选择文件！\n");
    }else{
        fileCanOpen =true;
    }
    minY=0;minX=0;
    chartT2->removeSeries(seriesT2);
    seriesT2->clear();
    list2.clear();

    if(fileCanOpen ==true){
    ifstream fileStream(fileT2Name.toStdString());
    if(fileStream.fail())//文件打开失败
        {
            ui->textBrowser->insertPlainText("文件不存在，请重新选择！\n");
            fileStream.close();
            system("pause");
        }
        else{
        ui->textBrowser->insertPlainText("故障数据读取中...\n");
                rowCount =1;	//初始化当前行数为0
                double tmp = 0;	//当前读入的数值
                while (!fileStream.eof()){
                    fileStream >> tmp;
                    seriesT2->append(rowCount, tmp);
                    list2.append(tmp);
                    if(maxY<tmp){
                        maxY = tmp;
                    }
                    if(minY>tmp){
                        minY = tmp;
                    }
                    rowCount++;	// 换下一行
                }
                fileStream.close();
    }
    rowCount=rowCount-1;

   //绘制信号图
   chartT2->setTitle(tr("故障数据信号图"));
   seriesT2->setName("检测信号1");

   //创建坐标
   axisXT2 = new QValueAxis;//X轴
   axisXT2->setRange(0, rowCount);//设置坐标轴范围
   axisXT2->setTitleText("sampling point");//标题
   axisXT2->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
   axisXT2->setTickCount(101); //主分隔个数：0到10分成20个单位
   axisXT2->setMinorTickCount(10); //每个单位之间绘制了多少虚网线
   axisXT2->setGridLineVisible(true);
   axisYT2 = new QValueAxis; //Y 轴
   axisYT2->setRange(minY,maxY);
   axisYT2->setTitleText("value");
   axisYT2->setLabelFormat("%.1f"); //标签格式
   axisYT2->setTickCount(11);
   axisYT2->setMinorTickCount(0);
   //为序列设置坐标轴
   chartT2->addSeries(seriesT2);
   chartT2->createDefaultAxes();
   chartT2->setAxisX(axisXT2,seriesT2);
   chartT2->setAxisY(axisYT2,seriesT2);
   ui->textBrowser->insertPlainText("故障信号图显示成功\n");
    //计算指标
    array.clear();
    getTimeFactors(list2,array);
    ui->lineEditKurtosisT2->setText(QString::number(array[0], 'f', 2));
    ui->lineEditCrestT2->setText(QString::number(array[1], 'f', 2));
    ui->lineEditClearanceT2->setText(QString::number(array[2], 'f', 2));
    ui->lineEditImpulseT2->setText(QString::number(array[3], 'f', 2));
    ui->lineEditShapeT2->setText(QString::number(array[4], 'f', 2));
    }else{
        ui->textBrowser->insertPlainText("请重新选择文件后再开始\n");
    }
}
void MainWindow::onOpenFileNormalData(){
    fileNormalDataName = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileNormalDataPathLabel->setText(fileNormalDataName);
}
void MainWindow::onOpenFileFaultData(){
    fileFaultDataName = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileFaultDataPathLabel->setText(fileFaultDataName);
}
void MainWindow::onOpenFileNewData(){
    fileNewDataName = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileNewDataPathLabel->setText(fileNewDataName);
}


void MainWindow::getTimeFactors(QList<double> list1,vector<double> &array){
    double sum1=0.0;
    double sum2=0.0;
    double sum3=0.0;
    double sum4=0.0;
    peak = 0.0;
    int l=list1.length();
    for (int i=0;i<list1.length();i++) {
        if(abs(list1.at(i))>peak){
            peak = abs(list1.at(i));
        }
        sum1=sum1+abs(list1.at(i));
        sum2=sum2+sqrt(abs(list1.at(i)));
        sum3=sum3+pow(list1.at(i),2);
        sum4=sum4+pow(list1.at(i),4);
    }
    average = sum1/l;
    rms = sqrt(sum3/l);
    fuzhi = pow(sum2/l,2);
    kurtosis = sum4/(l*pow(rms,4));
    crest = peak/rms;
    clearance = peak/fuzhi;
    impulse = peak/average;
    shape = rms/average;
    array.push_back(kurtosis);
    array.push_back(crest);
    array.push_back(clearance);
    array.push_back(impulse);
    array.push_back(shape);
}

void MainWindow::onSwitchTime(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::onSwitchF(){
    ui->stackedWidget->setCurrentIndex(1);
}void MainWindow::onSwitchNet(){
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::trainNet(){
       if(fileFaultDataName ==NULL|| fileNormalDataName == NULL){
           ui->MessageBrowser3->insertPlainText("请先选择样本文件后再训练模型\n");
       }else{
           ui->MessageBrowser3->insertPlainText("训练中...\n");
           normalDataOK = true;
           faultDataOK = true;
           const int typeNum = 2;
           const int normalSampleLen = ui->lineEditNormalSampleLen->text().toInt();
           const int faultSampleLen = ui->lineEditFaultSampleLen->text().toInt();
           const int oneNormalSampleNum = ui->lineEditNormalSampleNum->text().toInt();
           const int oneFaultSampleNum = ui->lineEditFaultSampleNum->text().toInt();
           const int allSampleNum = oneNormalSampleNum+oneFaultSampleNum;
           if(normalSampleLen*oneNormalSampleNum>240000){
               ui->MessageBrowser3->insertPlainText("正常数据样本长度/个数过大，请重新选择\n");
               normalDataOK = false;
           }
           if(faultSampleLen*oneFaultSampleNum>120000){
               ui->MessageBrowser3->insertPlainText("故障数据样本长度/个数过大，请重新选择\n");
               faultDataOK = false;
           }
            if(normalDataOK && faultDataOK){
                errors.clear();
                chartNet->removeSeries(seriesNet);
                seriesNet->clear();
                vector<double> samplein[allSampleNum];
                vector<double> sampleout[allSampleNum];
                QList<double> listN1;
                ifstream fileStream;
                int type=0;
                //当前读入正常数据
                fileStream.open(fileNormalDataName.toStdString());
                int rowCount = 1;	//初始化当前行数为0
                double tmp = 0;	//当前读入的数值
                int k = 0;
                int j = 0;
                for (k = 0; k < oneNormalSampleNum; k++) {
                    for (j = 0; j < normalSampleLen; j++) {
                        fileStream >> tmp;
                        listN1.append(tmp);
                    }
                    array.clear();
                    getTimeFactors(listN1,array);
                    samplein[k].push_back(array[0]);
                    samplein[k].push_back(array[1]);
                    samplein[k].push_back(array[2]);
                    samplein[k].push_back(array[3]);
                    samplein[k].push_back(array[4]);
                    sampleout[k].push_back(0);
                    //printf("the %d sample:峭度=%.2f,峰值=%.2f,裕度=%.2f,脉冲=%.2f,波形=%.2f\n",k+ type * oneSampleNum, kurtosis, crest, clearance, impulse, shape);
                }
                fileStream.close();

                //当前读入故障数据
                type++;
                fileStream.open(fileFaultDataName.toStdString());
                 rowCount = 1;	//初始化当前行数为0
                 tmp = 0;	//当前读入的数值
                 k = 0;
                 j = 0;
                 QList<double> listN2;
                for (k = 0; k < oneFaultSampleNum; k++) {
                    for (j = 0; j < faultSampleLen; j++) {
                         if(fileStream.eof()){

                         }

                        fileStream >> tmp;
                        listN2.append(tmp);
                    }

                    array.clear();
                    getTimeFactors(listN2,array);
                    samplein[k +  oneNormalSampleNum].push_back(array[0]);
                    samplein[k +  oneNormalSampleNum].push_back(array[1]);
                    samplein[k +  oneNormalSampleNum].push_back(array[2]);
                    samplein[k +  oneNormalSampleNum].push_back(array[3]);
                    samplein[k +  oneNormalSampleNum].push_back(array[4]);
                    sampleout[k + oneNormalSampleNum].push_back(1);
                    //printf("the %d sample:峭度=%.2f,峰值=%.2f,裕度=%.2f,脉冲=%.2f,波形=%.2f\n", k + type * oneSampleNum, kurtosis, crest, clearance, impulse, shape);
                }
                fileStream.close();

                // 学习样本
                int trainTimes = ui->lineEditTrainNum->text().toInt();
                sample sampleInOut[allSampleNum];
                for (int i = 0; i < allSampleNum; i++)
                {
                    sampleInOut[i].in = samplein[i];
                    sampleInOut[i].out = sampleout[i];
                }
                vector<sample> sampleGroup(sampleInOut, sampleInOut + allSampleNum);
                testNet.training(sampleGroup, trainTimes,errors);
                //绘制误差曲线
                for(int i=0;i<trainTimes;i++){
                    seriesNet->append(i,errors[i]);
                }
                chartNet->addSeries(seriesNet);
                seriesNet->setName("误差曲线");
                chartNet->createDefaultAxes();
                axisXN1 = new QValueAxis;//X轴
                axisXN1->setRange(0, trainTimes);//设置坐标轴范围
                axisXN1->setTitleText("训练次数");//标题
                axisXN1->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
                axisXN1->setTickCount(11); //主分隔个数：0到10分成20个单位
                axisXN1->setMinorTickCount(0); //每个单位之间绘制了多少虚网线
                axisXN1->setGridLineVisible(true);
                //为序列设置坐标轴
                chartNet->setAxisX(axisXN1,seriesNet);
                isTrainEnd = true;
                ui->MessageBrowser3->insertPlainText("模型训练完成！\n");
            }
         }
}
void MainWindow::predict(){
    if(isTrainEnd == false){
        ui->MessageBrowser3->insertPlainText("请先训练模型\n");
    }else{
        if(fileNewDataName==NULL){
            ui->MessageBrowser3->insertPlainText("请选择待预测文件后再开始\n");
        }else{
            ui->MessageBrowser3->insertPlainText("请等待，模型预测中...\n");
            ifstream fileStream;
            listNet.clear();
            const int testSampleNum = 1;
            vector<double> testin[testSampleNum];
            vector<double> testout[testSampleNum];
            fileStream.open(fileNewDataName.toStdString());
            rowCount = 0;	//初始化当前行数为0
            double tmp = 0;	//当前读入的数值
            while (!fileStream.eof()){
                    fileStream >> tmp;
                    listNet.append(tmp);
                    rowCount++;
                }
                array.clear();
                getTimeFactors(listNet,array);
                testin[0].push_back(array[0]);
                testin[0].push_back(array[1]);
                testin[0].push_back(array[2]);
                testin[0].push_back(array[3]);
                testin[0].push_back(array[4]);
                ui->lineEditKurtosisN1->setText(QString::number(array[0], 'f', 2));
                ui->lineEditCrestN1->setText(QString::number(array[1], 'f', 2));
                ui->lineEditClearanceN1->setText(QString::number(array[2], 'f', 2));
                ui->lineEditImpulseN1->setText(QString::number(array[3], 'f', 2));
                ui->lineEditShapeN1->setText(QString::number(array[4], 'f', 2));
                fileStream.close();
            // 测试数据

            sample testInOut[testSampleNum];
            for (int i = 0; i < testSampleNum; i++)
                testInOut[i].in = testin[i];
            vector<sample> testGroup(testInOut, testInOut + testSampleNum);

            // 预测测试数据，并输出结果
            testNet.predict(testGroup);

            for (int i = 0; i < testGroup.size(); i++)
            {
                for (int j = 0; j < testGroup[i].in.size(); j++) cout << testGroup[i].in[j] << "\t";
                cout << "-- prediction :";
                for (int j = 0; j < testGroup[i].out.size(); j++) {
                    cout << testGroup[i].out[j] << "\t";
                    QString result;
                    if(testGroup[i].out[j]<0.5){
                        result = "正常轴承";
                    }else{
                        result = "故障轴承";
                    }
                ui->lineEditResult->setText(result);
                }
                cout << endl;
            }
            ui->MessageBrowser3->insertPlainText("预测完成！\n");
        }
    }
}

void MainWindow::onOpenFileF1(){
    seriesF1->clear();
    fileF1Name = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileF1PathLabel->setText(fileF1Name);
    ifstream fileStream;
    fileStream.open(fileF1Name.toStdString());
    if(fileStream.fail())//文件打开失败
        {
            ui->MessageBrowser2->insertPlainText("文件不存在，请重新选择！\n");
            fileStream.close();
            system("pause");
        }else{
        int FNums=ui->lineEditF1sampleNum->text().toInt();
        fftw_complex *in1_c, *out1_c;//声明复数类型的输入数据in1_c和FFT变换的结果out1_c
        fftw_plan p;//声明变换策略
        in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FNums);//申请动态内存
        out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FNums);
        p = fftw_plan_dft_1d(FNums, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);//返回变换策略

        rowCount = 0;
        double tmp = 0.0;
        for(int n=0;n<FNums;n++){
            fileStream >> tmp;
            in1_c[n][0] = tmp;
            in1_c[n][1] = 0;
        }
        fftw_execute(p);//执行变换
        fftw_destroy_plan(p);//销毁策略
        for (int n = 0; n<FNums/2; n++)
        {
            double mag = sqrt(out1_c[n][0]*out1_c[n][0] + out1_c[n][1]*out1_c[n][1]);
            seriesF1->append(n,mag);
        }
        fftw_free(in1_c);
        fftw_free(out1_c);//释放内存
        fileStream.close();
        //创建坐标
        axisXF1 = new QValueAxis;//X轴
        axisXF1->setRange(0, FNums/2);//设置坐标轴范围
        axisXF1->setTitleText("Frequency");//标题
        axisXF1->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
        axisXF1->setTickCount(21); //主分隔个数：0到10分成20个单位
        axisXF1->setMinorTickCount(0); //每个单位之间绘制了多少虚网线
        axisXF1->setGridLineVisible(true);
        //为序列设置坐标轴
        chartF1->addSeries(seriesF1);
        chartF1->createDefaultAxes();
        chartF1->setAxisX(axisXF1,seriesF1);
        ui->MessageBrowser2->setText("正常轴承信号频域图显示完成\n");
    }
}
void MainWindow::onOpenFileF2(){
    seriesF2->clear();
    fileF2Name = QFileDialog::getOpenFileName(this,tr("打开文件"),".",tr("文本文件(*.txt)"));
    ui->FileF2PathLabel->setText(fileF2Name);
    ifstream fileStream;
    fileStream.open(fileF2Name.toStdString());
    if(fileStream.fail())//文件打开失败
        {
            ui->MessageBrowser2->insertPlainText("文件不存在，请重新选择！\n");
            fileStream.close();
            system("pause");
        }else{
        int FNums=ui->lineEditF2sampleNum->text().toInt();
        fftw_complex *in1_c, *out1_c;//声明复数类型的输入数据in1_c和FFT变换的结果out1_c
        fftw_plan p;//声明变换策略
        in1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FNums);//申请动态内存
        out1_c = (fftw_complex*)fftw_malloc(sizeof(fftw_complex)* FNums);
        p = fftw_plan_dft_1d(FNums, in1_c, out1_c, FFTW_FORWARD, FFTW_ESTIMATE);//返回变换策略

        rowCount = 0;
        double tmp = 0.0;
        for(int n=0;n<FNums;n++){
            fileStream >> tmp;
            in1_c[n][0] = tmp;
            in1_c[n][1] = 0;
        }
        fftw_execute(p);//执行变换
        fftw_destroy_plan(p);//销毁策略
        for (int n = 0; n<FNums/2; n++)
        {
            double mag = sqrt(out1_c[n][0]*out1_c[n][0] + out1_c[n][1]*out1_c[n][1]);
            seriesF2->append(n,mag);
        }
        fftw_free(in1_c);
        fftw_free(out1_c);//释放内存
        fileStream.close();
        //创建坐标
        axisXF2 = new QValueAxis;//X轴
        axisXF2->setRange(0, FNums/2);//设置坐标轴范围
        axisXF2->setTitleText("Frequency");//标题
        axisXF2->setLabelFormat("%d"); //标签格式：每个单位保留几位小数
        axisXF2->setTickCount(21); //主分隔个数：0到10分成20个单位
        axisXF2->setMinorTickCount(0); //每个单位之间绘制了多少虚网线
        axisXF2->setGridLineVisible(true);
        //为序列设置坐标轴
        chartF2->addSeries(seriesF2);
        chartF2->createDefaultAxes();
        chartF2->setAxisX(axisXF2,seriesF2);
        ui->MessageBrowser2->setText("故障轴承信号频域图显示完成\n");
    }
}
