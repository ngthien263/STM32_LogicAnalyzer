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
void Plot::plotData(QCustomPlot *customPlot, QByteArray &buffer, double &currentTime, int receivedFrequency, int receivedDutyCycle, int &updateTimeFlag) {
    double highTime = (1.0 / receivedFrequency) * (receivedDutyCycle / 100.0);
    double lowTime = (1.0 / receivedFrequency) * ((100 - receivedDutyCycle) / 100.0);

    if (lastByteTime == 0.0 || lastByteTime < 0 || updateTimeFlag == 1) {
        updateTimeFlag = 0;
        lastByteTime = currentTime; // Khởi tạo lại nếu cần thiết
    }

    if (customPlot == nullptr || customPlot->graph(0) == nullptr) {
        qDebug() << "customPlot hoặc customPlot->graph(0) là null, thoát hàm.";
        return;
    }

    while (!buffer.isEmpty()) {
        char lastByte = buffer.at(0); // Lấy byte đầu tiên
        buffer.remove(0, 1); // Xóa byte đầu tiên khỏi buffer
        qDebug() << "Đang xử lý byte:" << lastByte;

        if (lastByte == '1') {
            customPlot->graph(0)->addData(lastByteTime, 0);
            customPlot->graph(0)->addData(lastByteTime, 1);
            lastByteTime += highTime;
            customPlot->graph(0)->addData(lastByteTime, 1);
        } else if (lastByte == '0') {
            customPlot->graph(0)->addData(lastByteTime, 1);
            customPlot->graph(0)->addData(lastByteTime, 0);
            lastByteTime += lowTime;
            customPlot->graph(0)->addData(lastByteTime, 0);
        } else if (lastByte == 'N') {
            qDebug() << "'N' encountered, resetting IsFreqAndDutyRead" << lastByte;
            MainWindow::IsFreqAndDutyRead = 0;
            break;
        } else {
            qDebug() << "Giá trị byte không hợp lệ:" << lastByte;
            MainWindow::IsFreqAndDutyRead = 0;
            break;
        }
    }

    // Điều chỉnh phạm vi trục x để phản ánh thời gian hiện tại
    customPlot->xAxis->setRange(lastByteTime - highTime - lowTime, lastByteTime + highTime + lowTime);
    customPlot->replot();
}





