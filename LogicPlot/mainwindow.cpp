#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QPushButton>
QByteArray buffer;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , customPlot(new QCustomPlot(this))
    , isZooming(false)
    , lastByteTime(0)
    , isPaused(false)
    , actX(0)
    , timeFrame(10)
    , autorun(true)
{
    ui->setupUi(this);

    // Set up the QCustomPlot
    setCentralWidget(customPlot);
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // Đặt màu xanh cho high time
    customPlot->xAxis->setLabel("Time");
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

    // Create a layout for buttons
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    // Create and add pause and continue buttons
    QPushButton *pauseButton = new QPushButton("Pause", this);
    QPushButton *continueButton = new QPushButton("Continue", this);
    layout->addWidget(customPlot);
    layout->addWidget(pauseButton);
    layout->addWidget(continueButton);

    // Set widget as central widget
    setCentralWidget(widget);

    // Connect button signals to slots
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pausePlot);
    connect(continueButton, &QPushButton::clicked, this, &MainWindow::continuePlot);

    // Setup the serial port
    serial = new QSerialPort(this);
    serial->setPortName("COM4"); // Change the port name as needed
    serial->setBaudRate(QSerialPort::Baud115200);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
    if (!serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open serial port:" << serial->errorString();
    }

    // Start the elapsed timer
    elapsedTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
int disconected = 0;
void MainWindow::pausePlot() {
    isPaused = true;
    if(disconnect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData))
    {
        buffer.clear(); // Xóa bộ đệm khi tạm dừng
        disconected = 1;
    }

    qDebug() << "Paused"; // Debug message to confirm pause
}

void MainWindow::continuePlot() {
    isPaused = false;
    if(connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData))
        disconected = 0;
    //customPlot->graph(0)->data()->clear(); // Clear data using `clear`
    lastByteTime = elapsedTimer.elapsed() / 1000.0;
    qDebug() << "Continued"; // Debug message to confirm continue
}

int receivedFrequency;
int receivedDutyCycle;
int IsFreqAndDutyRead = 0;
void MainWindow::readSerialData() {
    const int MAX_SIZE = 1000; // Giới hạn kích thước tối đa

    if (buffer.size() < MAX_SIZE) {
        buffer += serial->readAll();
    } else {
        // Xử lý trường hợp mảng đã đầy
        qDebug() << "Buffer is full!";
    }


    QString strFreq;
    QString strDuty;
    double currentTime = elapsedTimer.elapsed() / 1000.0; // Convert to seconds

    while (IsFreqAndDutyRead == 0) {
        if(buffer[0] == '1' || buffer[0] == '0')
            break;
        int indexFreq = buffer.indexOf("F:");
        int indexDuty = buffer.indexOf("D:");

        // Check if both "F:" and "D:" exist in the buffer
        if (indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq) {
            break; // Break if either "F:" or "D:" are not found or if indexDuty < indexFreq
        }

        // Find the end of the frequency string
        int endIndexFreq = buffer.indexOf('\n', indexFreq);
        if (endIndexFreq == -1) {
            break; // Incomplete frequency string, exit the loop
        }

        // Find the end of the duty cycle string
        int endIndexDuty = buffer.indexOf('\n', indexDuty);
        if (endIndexDuty == -1) {
            break; // Incomplete duty cycle string, exit the loop
        }

        // Extract the frequency and duty cycle values
        strFreq = buffer.mid(indexFreq + 2, endIndexFreq - (indexFreq + 2));
        strDuty = buffer.mid(indexDuty + 2, endIndexDuty - (indexDuty + 2));

        // Convert strings to integers
        receivedFrequency = strFreq.toInt();
        receivedDutyCycle = strDuty.toInt();

        qDebug() << "Received Frequency:" << receivedFrequency;
        qDebug() << "Received Duty Cycle:" << receivedDutyCycle;

        // Remove frequency and duty cycle parts
        buffer.remove(indexFreq, endIndexFreq - indexFreq + 1);
        buffer.remove(indexDuty - (endIndexFreq - indexFreq + 1), endIndexDuty - indexDuty + 1);
        qDebug() << "Plotting data for byte:" << buffer;
        IsFreqAndDutyRead = 1;
    }
    while (!buffer.isEmpty()) {
        char byte = buffer.at(0);
        buffer.remove(0, 1); // Remove the processed character from the buffer
        qDebug() << "Plotting data for byte:" << byte;
        plotData(currentTime, byte); // Call plotData with currentTime and byte
    }
}


void MainWindow::plotData(double currentTime, int byte) {
    double highTime = (1.0 / receivedFrequency) * (receivedDutyCycle / 100.0);
    double lowTime  = (1.0 / receivedFrequency) * ((100 - receivedDutyCycle) / 100.0);
    qDebug() << "\nHigh time:" <<  highTime;
    qDebug() << "\nLow time:" <<  lowTime;

    if (lastByteTime == 0) {
        lastByteTime = currentTime;
    }

    if (byte == '1') {
        qDebug() << "Plotting high state";
        customPlot->graph(0)->addData(lastByteTime, 0);
        customPlot->graph(0)->addData(lastByteTime, 1);
        customPlot->graph(0)->addData(lastByteTime + highTime, 1);
        lastByteTime += highTime;
    } else if (byte == '0') {
        qDebug() << "Plotting low state";
        customPlot->graph(0)->addData(lastByteTime, 1);
        customPlot->graph(0)->addData(lastByteTime, 0);
        customPlot->graph(0)->addData(lastByteTime + lowTime, 0);
        lastByteTime += lowTime;
    }
    else{
        ;
    }
    customPlot->xAxis->setRange(currentTime - 1.5, currentTime + 1);
    customPlot->replot();
}
