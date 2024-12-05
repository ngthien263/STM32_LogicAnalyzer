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
    setupUI();
    elapsedTimer.start();
    setupPlot();
    serial->setPlot(plot);
    plot->setSerial(serial);
}
double currentTime;
int updateTimeFlag = 0;
void MainWindow::setupUI()
{
    // Tạo các widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Create a layout for buttons
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    layout->addWidget(customPlot);
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

void MainWindow::setupPlot() {
    customPlot->plotLayout()->clear(); // Xóa các phần tử cũ nếu có

    // Tạo Channel 1
    QCPAxisRect *axisRect1 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(0, 0, axisRect1);
    QCPGraph *graph1 = customPlot->addGraph(axisRect1->axis(QCPAxis::atBottom), axisRect1->axis(QCPAxis::atLeft));
    graph1->setPen(QPen(Qt::red));
    axisRect1->axis(QCPAxis::atBottom)->setLabel("Channel 1");
    axisRect1->setupFullAxesBox(true);

    // Tạo Channel 2
    QCPAxisRect *axisRect2 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(1, 0, axisRect2);
    QCPGraph *graph2 = customPlot->addGraph(axisRect2->axis(QCPAxis::atBottom), axisRect2->axis(QCPAxis::atLeft));
    graph2->setPen(QPen(Qt::green));
    axisRect2->axis(QCPAxis::atBottom)->setLabel("Channel 2");
    axisRect2->setupFullAxesBox(true);

    // Tạo Channel 3
    QCPAxisRect *axisRect3 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(2, 0, axisRect3);
    QCPGraph *graph3 = customPlot->addGraph(axisRect3->axis(QCPAxis::atBottom), axisRect3->axis(QCPAxis::atLeft));
    graph3->setPen(QPen(Qt::blue));
    axisRect3->axis(QCPAxis::atBottom)->setLabel("Channel 3");
    axisRect3->setupFullAxesBox(true);

    axisRect1->axis(QCPAxis::atLeft)->setRange(-0.2, 1.2);
    axisRect2->axis(QCPAxis::atLeft)->setRange(-0.2, 1.2);
    axisRect3->axis(QCPAxis::atLeft)->setRange(-0.2, 1.2);
    axisRect1->axis(QCPAxis::atBottom)->setRange(0, 5);
    axisRect2->axis(QCPAxis::atBottom)->setRange(0, 5);
    axisRect3->axis(QCPAxis::atBottom)->setRange(0, 5);

    // Bật tính năng phóng to và kéo thả cho từng axis rect, chỉ theo trục x
    axisRect1->setRangeZoom(Qt::Horizontal);
    axisRect1->setRangeDrag(Qt::Horizontal);
    axisRect2->setRangeZoom(Qt::Horizontal);
    axisRect2->setRangeDrag(Qt::Horizontal);
    axisRect3->setRangeZoom(Qt::Horizontal);
    axisRect3->setRangeDrag(Qt::Horizontal);

    customPlot->setInteraction(QCP::iRangeZoom, true);
    customPlot->setInteraction(QCP::iRangeDrag, true);

    // Tắt khử răng cưa để tăng hiệu suất
    customPlot->setNotAntialiasedElements(QCP::aeAll);
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
int IsFreqAndDutyRead = 0;
int receivedFrequency;
int  receivedDutyCycle;
void MainWindow::readSerialData(QSerialPort *serialPort) {

    const int MAX_SIZE = 1000; // Giới hạn kích thước tối đa

    if (buffer.size() < MAX_SIZE) {
        buffer += serialPort->readAll();
    } else {
        // Xử lý trường hợp mảng đã đầy
        qDebug() << "Buffer is full!";
        buffer.clear(); // Xóa sạch dữ liệu trong buffer
        buffer += serialPort->readAll();
    }

    QString strFreq;
    QString strDuty;

    // Convert to seconds

    while (IsFreqAndDutyRead == 0) {
        qDebug() << "Entered loop, buffer:" << buffer;

        if (buffer[0] == '1' || buffer[0] == '0') {
            qDebug() << "Condition buffer[0] == '1' || buffer[0] == '0' met, breaking loop.";
            break;
        }

        int indexFreq = buffer.indexOf("F:");
        int indexDuty = buffer.indexOf("D:");
        qDebug() << "indexFreq:" << indexFreq << ", indexDuty:" << indexDuty;

        if (indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq) {
            qDebug() << "Condition indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq met, breaking loop.";
            break;
        }

        int endIndexFreq = buffer.indexOf('\n', indexFreq);
        if (endIndexFreq == -1) {
            qDebug() << "Incomplete frequency string, breaking loop.";
            break;
        }

        int endIndexDuty = buffer.indexOf('\n', indexDuty);
        if (endIndexDuty == -1) {
            qDebug() << "Incomplete duty cycle string, breaking loop.";
            break;
        }

        strFreq = buffer.mid(indexFreq + 2, endIndexFreq - (indexFreq + 2));
        strDuty = buffer.mid(indexDuty + 2, endIndexDuty - (indexDuty + 2));

        qDebug() << "Extracted Frequency:" << strFreq << ", Duty Cycle:" << strDuty;

        if (!strFreq.isEmpty() && !strDuty.isEmpty()) {
            receivedFrequency = strFreq.toInt();
            receivedDutyCycle = strDuty.toInt();
        } else {
            qDebug() << "Failed to extract frequency or duty cycle!";
        }

        serial->updateFrequencyAndDuty(receivedFrequency, receivedDutyCycle);

        qDebug() << "Received Frequency:" << receivedFrequency;
        qDebug() << "Received Duty Cycle:" << receivedDutyCycle;
        buffer.remove(indexFreq, endIndexFreq - indexFreq + 1);
        buffer.remove(indexDuty - (endIndexFreq - indexFreq + 1), endIndexDuty - indexDuty + 1);
        qDebug() << "Plotting data for byte:" << buffer;
        IsFreqAndDutyRead = 1;
        qDebug() << "Plotting data for byte:" << buffer;
    }

    qDebug() << "Plotting data for byte:" << buffer;
    if (IsFreqAndDutyRead == 1) {
        currentTime = elapsedTimer.elapsed() / 1000.0;
        plot->plotData(customPlot, buffer, currentTime, receivedFrequency, receivedDutyCycle, updateTimeFlag);
    } else {
        qDebug() << "Plot object is null!";
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
    readSerialData(serialPort);
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

    if(serial->setupSerialPort(serialPort, portName, baudRate))
        updateTimeFlag = 1;
    connect(serialPort, &QSerialPort::readyRead, this, [=]() {
        currentTime = elapsedTimer.elapsed() / 1000.0; // Cập nhật currentTime ở đây
        plot->plotData(customPlot, buffer, currentTime, receivedFrequency, receivedDutyCycle, updateTimeFlag);
        readSerialData(serialPort);
    });
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
