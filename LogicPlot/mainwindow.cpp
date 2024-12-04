#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QElapsedTimer>
#include "serial.h"
#include "plot.h"

MainWindow::MainWindow(Serial *serial, Plot *plot, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , customPlot(new QCustomPlot(this))
    , serialPort(new QSerialPort(this))
    , serial(serial)
    , plot(new Plot(customPlot, this))
    , buffer()
{
    ui->setupUi(this);

    serial->setPlot(plot);
    plot->setSerial(serial);

    setupUI();
    setupPlot();
}

void MainWindow::setupUI()
{
    // Tạo các widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Create a layout for buttons
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(layout);

    // Create and add pause and continue buttons
    // QPushButton *pauseButton = new QPushButton("Pause", this);
    // QPushButton *continueButton = new QPushButton("Continue", this);
    layout->addWidget(customPlot);
    // layout->addWidget(pauseButton);
    // layout->addWidget(continueButton);

    // Set widget as central widget
    //setCentralWidget(widget);

    // Connect button signals to slots
    // connect(pauseButton, &QPushButton::clicked, plot, &Plot::pausePlot);
    // connect(continueButton, &QPushButton::clicked, plot, &Plot::continuePlot);

    // Menu bên phải
    QVBoxLayout *menuLayout = new QVBoxLayout();
    QLabel *menuLabel = new QLabel("Menu", this);
    menuLayout->addWidget(menuLabel);

    // Tạo layout ngang cho Baudrate
    QHBoxLayout *baudrateLayout = new QHBoxLayout();
    baudrateLayout->addWidget(new QLabel("Baudrate:", this)); // Nhãn "Baudrate"
    baudrateComboBox = new QComboBox(this);
    baudrateComboBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    baudrateComboBox->setFixedSize(200, 30); // Đặt kích thước cố định cho ComboBox
    baudrateLayout->addWidget(baudrateComboBox); // Thêm ComboBox vào cùng hàng

    // Thêm layout ngang này vào menuLayout
    menuLayout->addLayout(baudrateLayout);

    // COM Port ComboBox
    // Tạo layout ngang cho COM Port

    QHBoxLayout *comPortLayout = new QHBoxLayout();
    comPortLayout->addWidget(new QLabel("COM Port:", this)); // Thêm nhãn "COM Port:"

    // Tạo ComboBox để hiển thị danh sách cổng COM
    comPortComboBox = new QComboBox(this);
    comPortComboBox->setFixedSize(200, 30); // Đặt kích thước cố định cho ComboBox
    comPortLayout->addWidget(comPortComboBox); // Thêm ComboBox vào layout

    // Thêm layout ngang này vào menuLayout
    menuLayout->addLayout(comPortLayout);

    // Cập nhật danh sách các cổng COM khả dụng
    updateCOMPorts();

    // Start and Stop buttons
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);

    //menuLayout->addWidget(startButton);
    //menuLayout->addWidget(stopButton);
    startButton->setGeometry(550, 250, 100, 30);
    stopButton->setGeometry(670, 250, 100, 30);

    //nút Open mở cổng
    openButton = new QPushButton("Open", this); // Tạo nút Open
    //menuLayout->addWidget(openButton); // Thêm nút vào menu
    openButton->setGeometry(550, 280, 100, 30);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openSerialPort); // Kết nối tín hiệu nút với slot


    // Frequency and Duty Cycle labels
    frequencyLabel = new QLabel("Frequency: 0 Hz", this);
    dutyCycleLabel = new QLabel("Duty Cycle: 0%", this);
    menuLayout->addWidget(frequencyLabel);
    menuLayout->addWidget(dutyCycleLabel);

    // Set up layout
    QHBoxLayout *mainHorizontalLayout = new QHBoxLayout();
    mainHorizontalLayout->addWidget(customPlot, 2); // Đồ thị chiếm 2 phần
    mainHorizontalLayout->addLayout(menuLayout, 1); // Menu chiếm 1 phần
    mainLayout->addLayout(mainHorizontalLayout);

    setCentralWidget(centralWidget);

    // Kết nối các nút
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSerialConnection);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSerialConnection);
}

void MainWindow::setupPlot()
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
}

void MainWindow::updateCOMPorts()
{
    comPortComboBox->clear(); // Xóa danh sách cũ
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        comPortComboBox->addItem(port.portName()); // Thêm từng cổng COM vào ComboBox
    }

    if (comPortComboBox->count() == 0) {
        comPortComboBox->addItem("No COM Ports"); // Hiển thị thông báo nếu không có cổng khả dụng
        comPortComboBox->setEnabled(false); // Vô hiệu hóa ComboBox
    } else {
        comPortComboBox->setEnabled(true); // Kích hoạt ComboBox nếu có cổng
    }
}

void MainWindow::startSerialConnection()
{
    if (!serialPort->isOpen()) {
        qDebug() << "Error: Serial port is not open!";
        return;
    }

    // Xóa dữ liệu cũ trong serialPort
    serialPort->clear();

    // Gửi lệnh "REQ" tới vi điều khiển
    QByteArray command(1, '1'); // Tạo QByteArray chứa ký tự '1'
    qint64 bytesWritten = serialPort->write(command);

    qDebug() << "Write:" << command;

    // Kiểm tra số byte đã thực sự được gửi
    if (bytesWritten == -1) {
        qDebug() << "Write error:" << serialPort->errorString();
        return;
    } else if (bytesWritten < command.size()) {
        qDebug() << "Only part of the data was written. Bytes written:" << bytesWritten;
        return;
    } else {
        qDebug() << "Command '1' sent successfully!";
    }

    // Theo dõi lỗi giao tiếp
    connect(serialPort, &QSerialPort::errorOccurred, this, [](QSerialPort::SerialPortError error) {
        if (error != QSerialPort::NoError) {
            qDebug() << "Serial port error occurred:" << error;
        }
    });

    // Kết nối tín hiệu readyRead để xử lý dữ liệu nhận được
        serial->readSerialData(serialPort);
}



void MainWindow::openSerialPort()
{
    // Đảm bảo cổng nối tiếp không mở trước khi mở
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    // Lấy thông tin cổng và baudrate từ ComboBox
    QString portName = comPortComboBox->currentText();
    int baudRate = baudrateComboBox->currentText().toInt();

    serial->setupSerialPort(serialPort, portName, baudRate);
    connect(serialPort, &QSerialPort::readyRead, serial, &Serial::readSerialData);

}


void MainWindow::stopSerialConnection()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Serial port stopped.";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

