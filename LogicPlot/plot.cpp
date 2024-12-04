#include "plot.h"
#include "serial.h"

Plot::Plot(QObject *parent)
    : QObject(parent) {
    // Logic khởi tạo mặc định
}

Plot::Plot(QCustomPlot *plot, QObject *parent)
    : QObject(parent), customPlot(plot), lastByteTime(0), serial(nullptr)
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
void Plot::plotData(QCustomPlot *customPlot, QByteArray &buffer, double currentTime, int receivedFrequency, int receivedDutyCycle) {
    double highTime = (1.0 / receivedFrequency) * (receivedDutyCycle / 100.0);
    double lowTime  = (1.0 / receivedFrequency) * ((100 - receivedDutyCycle) / 100.0);
    qDebug() << "\nHigh time:" <<  highTime;
    qDebug() << "\nLow time:" <<  lowTime;

    if (lastByteTime == 0.0) {
        lastByteTime = currentTime;
    }

    if (customPlot == nullptr || customPlot->graph(0) == nullptr) {
        qDebug() << "customPlot or customPlot->graph(0) is null, exiting function.";
        return;
    }

    if (buffer.isEmpty()) {
        qDebug() << "Buffer is empty, exiting function.";
        return;
    }

    int bufferIndex = 0;
    while (!buffer.isEmpty()) {
        char byte = buffer.at(bufferIndex);
        buffer.remove(0, 1);
        qDebug() << "Processing byte:" << byte;

        if (byte == '1') {
            qDebug() << "Plotting high state" << byte;
            customPlot->graph(0)->addData(lastByteTime, 0);
            customPlot->graph(0)->addData(lastByteTime, 1);
            customPlot->graph(0)->addData(lastByteTime + highTime, 1);
            lastByteTime += highTime;
        } else if (byte == '0') {
            qDebug() << "Plotting low state" << byte;
            customPlot->graph(0)->addData(lastByteTime, 1);
            customPlot->graph(0)->addData(lastByteTime, 0);
            customPlot->graph(0)->addData(lastByteTime + lowTime, 0);
            lastByteTime += lowTime;
        } else {
            qDebug() << "Unexpected byte value:" << byte;
        }

        // if (bufferIndex >= buffer.size()) {
        //     qDebug() << "Buffer index out of range, stopping loop.";
        //     break;
        // }
    }

    customPlot->xAxis->setRange(currentTime - 1.5, currentTime + 1);
    customPlot->replot();
}




