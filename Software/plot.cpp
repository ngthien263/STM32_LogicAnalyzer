#include "plot.h"
#include "serial.h"

Plot::Plot(QObject *parent)
    : QObject(parent) {
    // Logic khởi tạo mặc định
}

Plot::Plot(QCustomPlot *plot, QObject *parent)
    : QObject(parent), customPlot(plot), lastTimeChannel2(0), lastTimeChannel1(0), serial(nullptr)
{
}

void Plot::setSerial(Serial *serialObj) {
    serial = serialObj;
}
QCustomPlot* Plot::setupPlot()
{
    // Set up the QCustomPlot
    //setCentralWidget(customPlot);
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->xAxis->setLabel("Times");
    customPlot->yAxis->setLabel("Signal");
    customPlot->xAxis->setRange(5, 10);
    customPlot->yAxis->setRange(-2, 5);

    // Enable zooming and panning
    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal); // Only allow zooming on x-axis
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal); // Only allow dragging on x-axis
    //customPlot->xAxis->setTickLabels(false);
    // Thiết lập chế độ vẽ nhanh hơn
    customPlot->setNotAntialiasedElements(QCP::aeAll); // Tắt khử răng cưa để tăng hiệu suất
    QPen pen;
    pen.setWidthF(3); // Đặt độ dày nét vẽ nhỏ để vẽ nhanh hơn
    customPlot->graph(0)->setPen(pen);
    return customPlot;
}
char lastByte_ch1 = '0';
char lastByte_ch2 = '0';
char lastByte[2] = {'0', '0'};

void Plot::plotDataChannel(QCPGraph *graph, int channelTime, double &currentTime, int receivedFrequency, int receivedDutyCycle, int &updateTimeFlag) {
    qDebug()<<"Plotting";
    double highTime = (1.0 / receivedFrequency) * (receivedDutyCycle / 100.0);
    double lowTime = (1.0 / receivedFrequency) * ((100 - receivedDutyCycle) / 100.0);
    qDebug()<<"High time: "<<highTime<<"Low time:"<<lowTime ;
    static char channel;
    static char receivedByte;

    if (lastTimeChannel1 == 0.0 || lastTimeChannel1 < 0 || updateTimeFlag == 1) {
        updateTimeFlag = 0;
        lastTimeChannel1 = currentTime; // Khởi tạo lại nếu cần thiết
    }

    if (lastTimeChannel2 == 0.0 || lastTimeChannel2 < 0 || updateTimeFlag == 1) {
        updateTimeFlag = 0;
        lastTimeChannel2 = currentTime; // Khởi tạo lại nếu cần thiết
    }

    if (graph == nullptr) {
        qDebug() << "graph is null, exiting function.";
        return;
    }

    if (graph == MainWindow::graph1) {
        qDebug() << "graph is 1";
        channel = 1;
        receivedByte = '1';

    } else if (graph == MainWindow::graph2) {
        qDebug() << "graph is 2";
        receivedByte = '1';
        channel = 2;
    } else {
        qDebug()<<"Graph khong hop le";
    }

    // if (lastByte[channel - 1] == '1') {
    //     lastByte[channel - 1] = '0';
    // }else if (lastByte[channel - 1] == '0') {
    //     lastByte[channel - 1] = '1';
    // }

    // //buffer.remove(0, 1);

    // qDebug() << "Đang xử lý byte:" << lastByte[channel - 1];

    if(channelTime == 1)
    {
        graph->addData(lastTimeChannel1, 0);
        graph->addData(lastTimeChannel1, 1);
        lastTimeChannel1 += highTime;

        graph->addData(lastTimeChannel1, 1);
        graph->addData(lastTimeChannel1, 0);
        lastTimeChannel1 += lowTime;
        //qDebug() << "lastByteTime: " << lastTimeChannel1;
        graph->addData(lastTimeChannel1, 0);

        //graph->keyAxis()->setRange(lastTimeChannel1 - highTime - lowTime, lastTimeChannel1 + highTime + lowTime);
        graph->keyAxis()->setRange(lastTimeChannel1 - highTime - lowTime, lastTimeChannel1 + highTime + lowTime);
        customPlot->replot();
    }
    else
    {
        graph->addData(lastTimeChannel2, 0);
        graph->addData(lastTimeChannel2, 1);
        lastTimeChannel2 += highTime;

        graph->addData(lastTimeChannel2, 1);
        graph->addData(lastTimeChannel2, 0);
        lastTimeChannel2 += lowTime;
        //qDebug() << "lastByteTime: " << lastTimeChannel1;
        graph->addData(lastTimeChannel2, 0);
        graph->keyAxis()->setRange(lastTimeChannel2 - highTime - lowTime, lastTimeChannel2 + highTime + lowTime);
        customPlot->replot();
    }
    // else if (lastByte[channel - 1] == '0') {
    //     graph->addData(lastByteTime, 1);
    //     graph->addData(lastByteTime, 0);
    //     lastByteTime += lowTime;
    //     graph->addData(lastByteTime, 0);
    // }

    // Điều chỉnh phạm vi trục x để phản ánh thời gian hiện tại


}





