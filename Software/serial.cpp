#include "serial.h"
#include "plot.h"
#include "mainwindow.h"

Serial::Serial(QObject *parent)
    : QObject(parent), serial(new QSerialPort(this)), receivedFrequency(0), receivedDutyCycle(0)
{
    // connect(serial, &QSerialPort::readyRead, this, [this]() {
    //     buffer += serial->readAll();
    //     readSerialData();
    // });

    elapsedTimer.start();
}

void Serial::setPlot(Plot *plotObj) {
    plot = plotObj;
}


int Serial::getFrequency() const
{
    return receivedFrequency;
}

int Serial::getDutyCycle() const
{
    return receivedDutyCycle;
}

QElapsedTimer Serial::getElapsedTimer() const {
    return elapsedTimer;
}

// Getter and Setter for QSerialPort
QSerialPort* Serial::getSerial() const {
    return serial;
}

void Serial::setSerial(QSerialPort* port) {
    serial = port;
}

// void Serial::readSerialData(QSerialPort *serialPort) {

//     const int MAX_SIZE = 1000; // Giới hạn kích thước tối đa

//     if (buffer.size() < MAX_SIZE) {
//         buffer += serialPort->readAll();
//     } else {
//         // Xử lý trường hợp mảng đã đầy
//         qDebug() << "Buffer is full!";
//         buffer.clear(); // Xóa sạch dữ liệu trong buffer
//         buffer += serialPort->readAll();
//     }

//     QString strFreq;
//     QString strDuty;

//     // Convert to seconds

//     while (IsFreqAndDutyRead == 0) {
//         qDebug() << "Entered loop, buffer:" << buffer;

//         if (buffer[0] == '1' || buffer[0] == '0') {
//             qDebug() << "Condition buffer[0] == '1' || buffer[0] == '0' met, breaking loop.";
//             break;
//         }

//         int indexFreq = buffer.indexOf("F:");
//         int indexDuty = buffer.indexOf("D:");
//         qDebug() << "indexFreq:" << indexFreq << ", indexDuty:" << indexDuty;

//         if (indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq) {
//             qDebug() << "Condition indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq met, breaking loop.";
//             break;
//         }

//         int endIndexFreq = buffer.indexOf('\n', indexFreq);
//         if (endIndexFreq == -1) {
//             qDebug() << "Incomplete frequency string, breaking loop.";
//             break;
//         }

//         int endIndexDuty = buffer.indexOf('\n', indexDuty);
//         if (endIndexDuty == -1) {
//             qDebug() << "Incomplete duty cycle string, breaking loop.";
//             break;
//         }

//         strFreq = buffer.mid(indexFreq + 2, endIndexFreq - (indexFreq + 2));
//         strDuty = buffer.mid(indexDuty + 2, endIndexDuty - (indexDuty + 2));

//         qDebug() << "Extracted Frequency:" << strFreq << ", Duty Cycle:" << strDuty;

//         if (!strFreq.isEmpty() && !strDuty.isEmpty()) {
//             receivedFrequency = strFreq.toInt();
//             receivedDutyCycle = strDuty.toInt();
//         } else {
//             qDebug() << "Failed to extract frequency or duty cycle!";
//         }

//         updateFrequencyAndDuty(receivedFrequency, receivedDutyCycle);

//         qDebug() << "Received Frequency:" << receivedFrequency;
//         qDebug() << "Received Duty Cycle:" << receivedDutyCycle;
//         buffer.remove(indexFreq, endIndexFreq - indexFreq + 1);
//         buffer.remove(indexDuty - (endIndexFreq - indexFreq + 1), endIndexDuty - indexDuty + 1);
//         qDebug() << "Plotting data for byte:" << buffer;
//         IsFreqAndDutyRead = 1;
//         qDebug() << "Plotting data for byte:" << buffer;
//     }
//     qDebug() << "Plotting data for byte:" << buffer;
//     customPlot = new QCustomPlot;
//     customPlot->graph(0);
//     if (IsFreqAndDutyRead == 1) {
//         double currentTime = elapsedTimer.elapsed() / 1000.0;
//         plot->plotData(customPlot, buffer, currentTime, receivedFrequency, receivedDutyCycle);
//     } else {
//         qDebug() << "Plot object is null!";
//     }
// }
bool Serial::setupSerialPort(QSerialPort *serialPort, const QString &portName, int baudRate)
{
    // Đảm bảo đối tượng QSerialPort không null
    if (!serialPort) {
        qDebug() << "Error: QSerialPort object is null!";
        return false;
    }
    // Đặt tên cổng
    serialPort->setPortName(portName);

    // Đặt các thông số cổng nối tiếp
    serialPort->setBaudRate(baudRate);
    // serialPort->setDataBits(QSerialPort::Data8);   // Sử dụng 8 bit dữ liệu
    // serialPort->setParity(QSerialPort::NoParity); // Không dùng bit chẵn lẻ
    // serialPort->setStopBits(QSerialPort::OneStop); // 1 bit dừng
    // serialPort->setFlowControl(QSerialPort::NoFlowControl); // Không dùng điều khiển luồng

    // Mở cổng ở chế độ chỉ đọc hoặc đọc-ghi
    if (!serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Error: Failed to open serial port!" << serialPort->errorString();
        return false;
    }

    // In thông tin cấu hình ra console (gỡ lỗi)
    qDebug() << "Serial port configured:"
             << "Port:" << portName
             << "Baud Rate:" << baudRate;

    return true;
}

