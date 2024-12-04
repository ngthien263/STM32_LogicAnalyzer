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

// void Plot::pausePlot() {

//     isPaused = true;

//     if(disconnect(serial->getSerial(), &QSerialPort::readyRead, serial, &Serial::readSerialData))
//     {
//         buffer->getBuffer().clear(); // Xóa bộ đệm khi tạm dừng
//         disconectedFlag = 1;
//     }

//     qDebug() << "Paused"; // Debug message to confirm pause
// }

// void Plot::continuePlot() {

//     isPaused = false;

//     if(connect(serial->getSerial(), &QSerialPort::readyRead, serial, &Serial::readSerialData))
//         disconectedFlag = 0;
//     //customPlot->graph(0)->data()->clear(); // Clear data using `clear`

//     lastByteTime = elapsedTimer->getElapsedTimer().elapsed() / 1000.0;

//     qDebug() << "Continued"; // Debug message to confirm continue
// }

void Plot::plotData(QCustomPlot *customPlot, QByteArray &buffer, double currentTime, int receivedFrequency, int receivedDutyCycle) {

    double highTime = (1.0 / receivedFrequency) * (receivedDutyCycle / 100.0);
    double lowTime  = (1.0 / receivedFrequency) * ((100 - receivedDutyCycle) / 100.0);

    qDebug() << "\nHigh time:" <<  highTime;
    qDebug() << "\nLow time:" <<  lowTime;

    if (lastByteTime == 0.0) {
        lastByteTime = currentTime;
    }

    int bufferIndex = 0;

    while (!buffer.isEmpty()) {
        char byte = buffer.at(bufferIndex);
        buffer.remove(0, 1);

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
        }

        customPlot->xAxis->setRange(currentTime - 1.5, currentTime + 1);
        customPlot->replot();

        // Xử lý byte như bình thường
    }
}
