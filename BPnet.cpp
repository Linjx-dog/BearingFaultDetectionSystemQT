#include "BPnet.h"

using namespace std;

BpNet::BpNet()
{
    srand((unsigned)time(NULL));
    error = 100.f;

    // 初始化输入层
    for (int i = 0; i < innode; i++){
        inputLayer[i] = new inputNode();
        for (int j = 0; j < hidenode; j++){
            inputLayer[i]->weight.push_back(get_11Random());
            inputLayer[i]->wDeltaSum.push_back(0.f);
        }
    }

    // 初始化隐藏层
    for (int j = 0; j < hidenode; j++){
        hiddenLayer[j] = new hiddenNode();
        hiddenLayer[j]->bias = get_11Random();
        for (int k = 0; k < outnode; k++){
            hiddenLayer[j]->weight.push_back(get_11Random());
            hiddenLayer[j]->wDeltaSum.push_back(0.f);
        }
    }

    // 初始化输出层
    for (int i = 0; i < outnode; i++){
        outputLayer[i] = new outputNode();
        outputLayer[i]->bias = get_11Random();
    }
}

void BpNet::forwardPropagationEpoc()
{
    //输入层到隐层前向传播
    for (int j = 0; j < hidenode; j++){
        double sum = 0.f;
        for (int k = 0; k < innode; k++)
        {
            sum += inputLayer[k]->value * inputLayer[k]->weight[j];
        }
        sum += hiddenLayer[j]->bias;
        hiddenLayer[j]->value = sigmoid(sum);
    }

    //隐层到输出层前向传播
    for (int i = 0; i < outnode; i++){
        double sum = 0.f;
        for (int j = 0; j < hidenode; j++){
            sum += hiddenLayer[j]->value * hiddenLayer[j]->weight[i];
        }
        sum += outputLayer[i]->bias;
        outputLayer[i]->value = sigmoid(sum);
    }
}

void BpNet::backPropagationEpoc()
{
    //输出层后向传播
    for (int i = 0; i < outnode; i++){
        double tmpe = fabs(outputLayer[i]->value - outputLayer[i]->rightout);
        error += tmpe * tmpe / 2;

        outputLayer[i]->delta
            = (outputLayer[i]->value - outputLayer[i]->rightout) * (1 - outputLayer[i]->value) * outputLayer[i]->value;
        outputLayer[i]->bDeltaSum += outputLayer[i]->delta;
    }

    //隐层后向传播
    for (int j = 0; j < hidenode; j++){
        double sum = 0.f;
        for (int k = 0; k < outnode; k++) {
            sum += outputLayer[k]->delta * hiddenLayer[j]->weight[k];
            hiddenLayer[j]->wDeltaSum[k] += hiddenLayer[j]->value * outputLayer[k]->delta;
        }
        hiddenLayer[j]->delta = sum * (1 - hiddenLayer[j]->value) * hiddenLayer[j]->value;
        hiddenLayer[j]->bDeltaSum += hiddenLayer[j]->delta;
    }
    //输入层后向传播
    for (int i = 0; i < innode; i++){
        for (int j = 0; j < hidenode; j++){
            inputLayer[i]->wDeltaSum[j] += inputLayer[i]->value * hiddenLayer[j]->delta;
        }
    }
}

void BpNet::training( vector<sample> sampleGroup, int trainTimes,vector<double> &errors)
{
    int sampleNum = sampleGroup.size();
    int iter = 0;
    for(int time=0;time<trainTimes;time++)
    {
        iter++;
        cout << iter<<"training error: " << error << endl;
        errors.push_back(error);
        error = 0.f;
        //每次循环开始前初始化
        for (int i = 0; i < innode; i++) inputLayer[i]->wDeltaSum.assign(inputLayer[i]->wDeltaSum.size(), 0.f);
            for (int j = 0; j < hidenode; j++)
            {
                hiddenLayer[j]->wDeltaSum.assign(hiddenLayer[j]->wDeltaSum.size(), 0.f);
                hiddenLayer[j]->bDeltaSum = 0.f;
            }

        for (int i = 0; i < outnode; i++)
            outputLayer[i]->bDeltaSum = 0.f;

        for (int iter = 0; iter < sampleNum; iter++){
            setInput(sampleGroup[iter].in);
            setOutput(sampleGroup[iter].out);

            forwardPropagationEpoc();
            backPropagationEpoc();
        }

        // 更新输入层
        for (int i = 0; i < innode; i++){
            for (int j = 0; j < hidenode; j++){
                inputLayer[i]->weight[j] -= learningRate * inputLayer[i]->wDeltaSum[j] / sampleNum;
            }
        }
        // 更新隐层
        for (int j = 0; j < hidenode; j++){
            // bias
            hiddenLayer[j]->bias -= learningRate * hiddenLayer[j]->bDeltaSum / sampleNum;

            // weight
            for (int k = 0; k < outnode; k++){
                hiddenLayer[j]->weight[k] -= learningRate * hiddenLayer[j]->wDeltaSum[k] / sampleNum;
            }
        }
        // 更新输出层
        for (int i = 0; i < outnode; i++){
            outputLayer[i]->bias -= learningRate * outputLayer[i]->bDeltaSum / sampleNum;
        }
    }
}

void BpNet::predict(vector<sample>& testGroup)
{
    int testNum = testGroup.size();

    for (int iter = 0; iter < testNum; iter++)
    {
        testGroup[iter].out.clear();
        setInput(testGroup[iter].in);

        // 输入层到隐层
        for (int j = 0; j < hidenode; j++){
            double sum = 0.f;
            for (int k = 0; k < innode; k++){
                sum += inputLayer[k]->value * inputLayer[k]->weight[j];
            }
            sum += hiddenLayer[j]->bias;
            hiddenLayer[j]->value = sigmoid(sum);
        }

        // 隐层到输出层
        for (int i = 0; i < outnode; i++){
            double sum = 0.f;
            for (int j = 0; j < hidenode; j++){
                sum += hiddenLayer[j]->value * hiddenLayer[j]->weight[i];
            }
            sum += outputLayer[i]->bias;
            outputLayer[i]->value = sigmoid(sum);
            testGroup[iter].out.push_back(outputLayer[i]->value);
        }
    }
}

void BpNet::setInput( vector<double> sampleIn)
{
    for (int i = 0; i < innode; i++)
        inputLayer[i]->value = sampleIn[i];
}

void BpNet::setOutput( vector<double> sampleOut)
{
    for (int i = 0; i < outnode; i++)
        outputLayer[i]->rightout = sampleOut[i];
}
