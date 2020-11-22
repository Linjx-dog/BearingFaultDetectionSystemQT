#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include <stdlib.h>
#include <time.h>

using namespace std;

#define innode 5        //输入结点数
#define hidenode 5      //隐含结点数
#define outnode 1       //输出结点数
#define learningRate 0.9//学习速率

inline double get_11Random()    // -1 ~ 1
{
    return ((2.0 * (double)rand() / RAND_MAX) - 1);
}

inline double sigmoid(double x)
{
    double ans = 1 / (1 + exp(-x));
    return ans;
}

typedef struct inputNode
{
    double value;
    vector<double> weight, wDeltaSum;
}inputNode;


typedef struct outputNode
{
    double value, delta, rightout, bias, bDeltaSum;
}outputNode;

typedef struct hiddenNode
{
    double value, delta, bias, bDeltaSum;
    vector<double> weight, wDeltaSum;
}hiddenNode;

typedef struct sample
{
    vector<double> in, out;
}sample;

class BpNet
{
public:
    BpNet();
    void forwardPropagationEpoc();
    void backPropagationEpoc();

    void training( vector<sample> sampleGroup, int trainTimes,vector<double> &errors);
    void predict(vector<sample>& testGroup);

    void setInput( vector<double> sampleIn);
    void setOutput( vector<double> sampleOut);

public:
    double error;
    inputNode* inputLayer[innode];
    outputNode* outputLayer[outnode];
    hiddenNode* hiddenLayer[hidenode];
};
