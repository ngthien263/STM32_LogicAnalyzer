#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QPushButton>

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
    customPlot->addGraph(); // Đường đồ thị 1 (high time)
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

void MainWindow::pausePlot() {
    isPaused = true;
    autorun = false; // Disable autorun when paused
    qDebug() << "Paused"; // Debug message to confirm pause
}

void MainWindow::continuePlot() {
    isPaused = false;
    autorun = true; // Enable autorun when continuing
    customPlot->graph(0)->data()->clear(); // Clear data using `clear`
    dataBuffer.clear(); // Clear the buffer to ensure no old data is plotted

    qDebug() << "Continued"; // Debug message to confirm continue
}


void MainWindow::readSerialData() {
    static QByteArray buffer;

    // Read data from UART
    buffer += serial->readAll();
    qDebug() << "Received buffer:" << buffer;

    // Get the current elapsed time
    double currentTime = elapsedTimer.elapsed() / 1000.0; // Convert to seconds

    // Process each character in the buffer
    while (!buffer.isEmpty()) {
        char byte = buffer.at(0);
        buffer.remove(0, 1); // Remove the processed character from the buffer

        // Calculate the interval between bytes
        double interval = currentTime - lastByteTime;
        qDebug() << "Time interval between bytes:" << interval << " seconds";

        // Update the last byte time
        lastByteTime = currentTime;

        if (isPaused) {
            ;
        } else {
            plotData(currentTime, byte);
            customPlot->xAxis->setRange(currentTime - 1.5, currentTime + 1);
            customPlot->replot();
        }
    }
}

void MainWindow::plotData(double currentTime, char byte) {
    if (byte == '1') {
        customPlot->graph(0)->addData(lastByteTime, 0); // Ensure the previous state is added
        customPlot->graph(0)->addData(lastByteTime, 1); // Rising edge
        customPlot->graph(0)->addData(currentTime, 1); // Maintain state until now
    } else if (byte == '0') {
        customPlot->graph(0)->addData(lastByteTime, 1); // Ensure the previous state is added
        customPlot->graph(0)->addData(lastByteTime, 0); // Falling edge
        customPlot->graph(0)->addData(currentTime, 0); // Maintain state until now
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QMainWindow::mouseMoveEvent(event);
    isZooming = (event->buttons() & Qt::RightButton) || (event->buttons() & Qt::MiddleButton);
}
