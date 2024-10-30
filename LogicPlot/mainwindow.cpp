#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTimer> // Thêm để sử dụng QTimer

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serial(new QSerialPort(this))
    , highTime(0)
    , lowTime(0)
    , currentState(HIGH)
    , beforeTime(0.0)
{
    ui->setupUi(this);

    // Cấu hình cổng UART
    serial->setPortName("COM3"); // Đặt tên cổng UART (thay đổi theo cổng của bạn)
    serial->setBaudRate(QSerialPort::Baud19200); // Đặt baud rate đủ cao

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readSerialData);
    if (serial->open(QIODevice::ReadOnly)) {
        qDebug() << "Serial port opened successfully";
    } else {
        qDebug() << "Failed to open serial port";
    }

    // Cấu hình QCustomPlot
    customPlot = new QCustomPlot(this);
    setCentralWidget(customPlot);
    customPlot->addGraph(); // Đường đồ thị 1 (high time)
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // Đặt màu xanh cho high time
    //customPlot->graph(1)->setPen(QPen(Qt::red));  // Đặt màu đỏ cho low time

    // Thiết lập trục X và Y
    customPlot->xAxis->setLabel("Time (ms)");
    customPlot->yAxis->setLabel("Value");

    // Thiết lập giới hạn trục Y
    customPlot->yAxis->setRange(-2, 5);

    // Thiết lập chế độ vẽ nhanh hơn
    customPlot->setNotAntialiasedElements(QCP::aeAll); // Tắt khử răng cưa để tăng hiệu suất
    QPen pen;
    pen.setWidthF(3); // Đặt độ dày nét vẽ nhỏ để vẽ nhanh hơn
    customPlot->graph(0)->setPen(pen);
    //customPlot->graph(1)->setPen(pen);

    // Cập nhật đồ thị định kỳ
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updatePlot);
    timer->start(10); // Cập nhật mỗi 10 ms để giảm tải CPU
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::readSerialData()
{
    static QByteArray buffer; // Bộ đệm tạm để lưu trữ dữ liệu chưa xử lý

    // Đọc dữ liệu từ UART
    buffer += serial->readAll();

    // Tách các dòng dữ liệu hoàn chỉnh dựa trên dấu ','
    QList<QByteArray> parts = buffer.split(',');

    // Xử lý tất cả các phần trước phần cuối cùng (có thể chưa hoàn chỉnh)
    for (int i = 0; i < parts.size() - 1; ++i) {
        const QByteArray& part = parts[i];
        if (part == "1") {
            currentState = HIGH;
            dataSignalQueue.enqueue(currentState);
        }
        else if (part == "0") {
            currentState = LOW;
            dataSignalQueue.enqueue(currentState);
        }
        else {
            bool ok;
            int time = part.toInt(&ok);
            if (ok) {
                dataTimeQueue.enqueue(time);
                qDebug() << "Enqueued signalTime:" << time;
            } else {
                qDebug() << "Invalid signalTime from part:" << part;
            }
        }
    }

    // Giữ lại phần cuối cùng của buffer nếu nó không phải là một phần hoàn chỉnh
    buffer = parts.last();
}


void MainWindow::updatePlot()
{
    static QElapsedTimer elapsedTimer;
    if (!elapsedTimer.isValid()) {
        elapsedTimer.start();
    }
    double key = elapsedTimer.elapsed() / 1000.0; // Chuyển đổi thành giây hoặc ms

    // Cập nhật từ hàng đợi dữ liệu
    while (!dataSignalQueue.isEmpty() && !dataTimeQueue.isEmpty()) {
        auto dataSignal = dataSignalQueue.dequeue();
        double laterTime = dataTimeQueue.dequeue() / 1000.0;

        // Thêm điểm mới vào đồ thị
        customPlot->graph(0)->addData(beforeTime, dataSignal);
        customPlot->graph(0)->addData(beforeTime + laterTime, dataSignal);

        if (dataSignal == HIGH) {
            customPlot->graph(0)->addData(beforeTime + laterTime, 0);
        } else {
            customPlot->graph(0)->addData(beforeTime + laterTime, 1);
        }

        beforeTime += laterTime;
    }

    // Cập nhật trục X
    customPlot->xAxis->setRange(key - 5, key); // Di chuyển mượt mà với thời gian

    // Giới hạn số điểm trên đồ thị để tránh tràn bộ nhớ
    customPlot->graph(0)->data()->removeBefore(key - 15); // Giữ lại 20 giây dữ liệu

    // Cập nhật lại đồ thị
    customPlot->replot();
}

