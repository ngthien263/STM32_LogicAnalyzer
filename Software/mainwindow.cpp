#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QElapsedTimer>
#include "serial.h"
#include "plot.h"
int MainWindow::IsFreqAndDutyRead = 0;
int MainWindow::IsFreqAndDutyRead_ch1 = 0;
int MainWindow::IsFreqAndDutyRead_ch2 = 0;
QCPGraph* MainWindow::graph1 = nullptr;
QCPGraph* MainWindow::graph2 = nullptr;
QCPGraph* MainWindow::graph3 = nullptr;
MainWindow::MainWindow(Serial *serial, Plot *plot, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , customPlot(new QCustomPlot(this))
    , serialPort(new QSerialPort(this))
    , serial(serial)
    , plot(new Plot(customPlot, this))
    , buffer(), lblFreqChannel1(nullptr), lblDutyChannel1(nullptr),
    lblFreqChannel2(nullptr), lblDutyChannel2(nullptr), channel_1_time(1), channel_2_time(2)
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

typedef struct
{
    int Freq;
    int Duty;
} FreqAndDuty_typedef;

FreqAndDuty_typedef Channel1FD;
FreqAndDuty_typedef Channel2FD;
FreqAndDuty_typedef channelFreqAndDuty[2] = {};

void MainWindow::setupUI() {
    // Tạo các widget
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Layout chính ngang chứa đồ thị và menu
    QHBoxLayout *mainHorizontalLayout = new QHBoxLayout();

    // Create a layout for buttons
    QVBoxLayout *layout = new QVBoxLayout();
    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    layout->addWidget(customPlot);

        // Menu bên phải
    QVBoxLayout *menuLayout = new QVBoxLayout();

    // Menu tiêu đề
    QLabel *menuLabel = new QLabel("Settings", this);
    menuLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
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
    comPortComboBox = new QComboBox(this);
    comPortComboBox->setFixedSize(200, 30); // Đặt kích thước cố định cho ComboBox
    updateCOMPorts();
    comPortLayout->addWidget(comPortComboBox); // Thêm ComboBox vào layout
    // Thêm layout ngang này vào menuLayout
    menuLayout->addLayout(comPortLayout);
    // Cập nhật danh sách các cổng COM khả dụng


    // Start and Stop buttons
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    openButton = new QPushButton("Open", this); // Tạo nút Open

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    menuLayout->addLayout(buttonLayout);

    // Kết nối các nút
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openSerialPort);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startSerialConnection);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSerialConnection);

    QGroupBox *channel1Group = createChannelGroup("Channel 1", lblFreqChannel1, lblDutyChannel1);
    QGroupBox *channel2Group = createChannelGroup("Channel 2", lblFreqChannel2, lblDutyChannel2);

    // Thêm các group box vào menuLayout
    menuLayout->addWidget(channel1Group);
    menuLayout->addWidget(channel2Group);

    // Thêm menuLayout vào mainHorizontalLayout
    mainHorizontalLayout->addLayout(menuLayout, 1); // Menu chiếm 1 phần
    mainHorizontalLayout->addWidget(customPlot, 2); // Đồ thị chiếm 2 phần


    // Đặt layout chính
    mainLayout->addLayout(mainHorizontalLayout);
    setCentralWidget(centralWidget);
}



QGroupBox *MainWindow::createChannelGroup(const QString &title, QLabel *&freqLabel, QLabel *&dutyLabel) {
    QGroupBox *groupBox = new QGroupBox(title);

    // Layout cho group box
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    // Nhãn hiển thị
    freqLabel = new QLabel("Frequency: 0 Hz", groupBox);
    dutyLabel = new QLabel("Duty Cycle: 0%", groupBox);

    // Căn chỉnh và thêm nhãn vào layout
    freqLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    dutyLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    layout->addWidget(freqLabel);
    layout->addWidget(dutyLabel);

    return groupBox;
}


void MainWindow::setupPlot() {
    customPlot->plotLayout()->clear(); // Xóa các phần tử cũ nếu có

    // Tạo Channel 1
    QCPAxisRect *axisRect1 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(0, 0, axisRect1);
    graph1 = customPlot->addGraph(axisRect1->axis(QCPAxis::atBottom), axisRect1->axis(QCPAxis::atLeft));
    QPen pen(Qt::red);
    pen.setWidth(1); // Độ dày nét vẽ (2 pixels, có thể điều chỉnh)
    graph1->setPen(pen);
    axisRect1->axis(QCPAxis::atBottom)->setLabel("Channel 1");
    axisRect1->setupFullAxesBox(true);

    // Tạo Channel 2
    QCPAxisRect *axisRect2 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(1, 0, axisRect2);
    graph2 = customPlot->addGraph(axisRect2->axis(QCPAxis::atBottom), axisRect2->axis(QCPAxis::atLeft));
    graph2->setPen(QPen(Qt::green));
    axisRect2->axis(QCPAxis::atBottom)->setLabel("Channel 2");
    axisRect2->setupFullAxesBox(true);

    // Tạo Channel 3
    QCPAxisRect *axisRect3 = new QCPAxisRect(customPlot);
    customPlot->plotLayout()->addElement(2, 0, axisRect3);
    graph3 = customPlot->addGraph(axisRect3->axis(QCPAxis::atBottom), axisRect3->axis(QCPAxis::atLeft));
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

int receivedFrequency;
int  receivedDutyCycle;
// void MainWindow::readSerialData(QSerialPort *serialPort) {
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
//     qDebug() << "Entered loop, buffer:" << buffer;
//     qDebug() << "IsFreqAndDutyRead: " << IsFreqAndDutyRead;

//     // Reset IsFreqAndDutyRead when 'N' is encountered
//     if (buffer.contains('N')) {
//         qDebug() << "'N' encountered, resetting IsFreqAndDutyRead";
//         IsFreqAndDutyRead = 0;
//         buffer.remove(0, buffer.indexOf('N') + 1); // Remove 'N' and preceding characters
//     }

//     while (IsFreqAndDutyRead == 0) {
//         if (buffer.startsWith('1') || buffer.startsWith('0')) {
//             IsFreqAndDutyRead = 0;
//             qDebug() << "Condition buffer.startsWith('1') || buffer.startsWith('0') met, breaking loop.";
//             break;
//         }

//         int indexFreq = buffer.indexOf("F:");
//         int indexDuty = buffer.indexOf("D:");
//         qDebug() << "indexFreq:" << indexFreq << ", indexDuty:" << indexDuty;

//         if (indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq) {
//             IsFreqAndDutyRead = 0;
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

//         qDebug() << "Received Frequency:" << receivedFrequency;
//         qDebug() << "Received Duty Cycle:" << receivedDutyCycle;
//         buffer.remove(0, endIndexDuty + 1); // Xóa cả chuỗi đã xử lý khỏi buffer
//         updateTimeFlag = 1;
//         IsFreqAndDutyRead = 1;
//         break; // Thoát khỏi vòng lặp sau khi xử lý xong chuỗi F và D
//     }

//     if (IsFreqAndDutyRead == 1) {
//         currentTime = elapsedTimer.elapsed() / 1000.0;
//         plot->plotData(customPlot, buffer, currentTime, receivedFrequency, receivedDutyCycle, updateTimeFlag);
//     }
// }

void MainWindow::readSerialData(QSerialPort *serialPort) {
    const int MAX_SIZE = 1000; // Giới hạn kích thước tối đa

    if (buffer.size() < MAX_SIZE) {
        buffer += serialPort->readAll();
        qDebug() << "buffer: " << buffer;
    } else {
        qDebug() << "Buffer is full! Removing oldest data.";
        buffer.remove(0, buffer.size() / 2); // Xóa một nửa dữ liệu cũ
        buffer += serialPort->readAll();
    }
    while (buffer.contains("F:") && buffer.contains("D:") && buffer.contains("\n"))
    {
        //qDebug() << "'N' encountered, resetting IsFreqAndDutyRead";
        //IsFreqAndDutyRead = 0;

        buffer.remove(0, buffer.indexOf('N') + 1); // Remove 'N' and preceding characters

        int indexChannel = buffer.indexOf("C");

        if (buffer.at(indexChannel + 1) == '1'){
            getFreqandDuty(1);
            updateChannel1(Channel1FD.Freq, Channel1FD.Duty);
            IsFreqAndDutyRead = 1;
        }
        else if (buffer.at(indexChannel + 1) == '2'){
            getFreqandDuty(2);
            updateChannel2(Channel2FD.Freq, Channel2FD.Duty);
            IsFreqAndDutyRead = 1;
        }
    }

    if (IsFreqAndDutyRead == 1) {
        while (!buffer.isEmpty()) {
            char channel = buffer.at(0);
            qDebug() << "buffer.at(0): " << buffer.at(0);
            if (channel == '1') {
                plot->plotDataChannel(graph1, channel_1_time, currentTime, Channel1FD.Freq, Channel1FD.Duty, updateTimeFlag);
            } else if (channel == '2') {
                plot->plotDataChannel(graph2, channel_2_time, currentTime, Channel2FD.Freq, Channel2FD.Duty, updateTimeFlag);
            } else {
                break; // Ký tự không hợp lệ hoặc buffer không chứa dữ liệu phù hợp
            }
            buffer.remove(0, 1); // Xóa ký tự đã xử lý
        }
    }
}

void MainWindow::getFreqandDuty(int channel) {
    int indexFreq = buffer.indexOf("F:");
    int indexDuty = buffer.indexOf("D:");\
    int endIndex = buffer.indexOf('\n', qMax(indexFreq, indexDuty));
    if (indexFreq == -1 || indexDuty == -1 || indexDuty < indexFreq) {
        qDebug() << "Invalid data format. Skipping.";
        return;
    }

    // if (endIndexFreq == -1 || endIndexDuty == -1) {
    //     qDebug() << "Incomplete data. Waiting for more.";
    //     return;
    // }

    // strFreq = buffer.mid(indexFreq + 2, endIndexFreq - (indexFreq + 2)).trimmed();
    // strDuty = buffer.mid(indexDuty + 2, endIndexDuty - (indexDuty + 2)).trimmed();

    // Trích xuất giá trị Frequency và Duty
    QString strFreq = buffer.mid(indexFreq + 2, indexDuty - (indexFreq + 2)); // Lấy giá trị sau "F:"
    QString strDuty = buffer.mid(indexDuty + 2, endIndex - (indexDuty + 2)); // Lấy giá trị sau "D:"

    if (strFreq.isEmpty() || strDuty.isEmpty()) {
        qDebug() << "Frequency or duty cycle string is empty. Skipping.";
        return;
    }

    FreqAndDuty_typedef *channelData = (channel == 1) ? &Channel1FD : &Channel2FD;
    channelData->Freq = strFreq.toInt();
    channelData->Duty = strDuty.toInt();

    qDebug() << "Channel" << channel << ": Frequency =" << channelData->Freq << ", Duty Cycle =" << channelData->Duty;

    buffer.remove(0, endIndex + 1); // Xóa phần dữ liệu đã xử lý
}

void MainWindow::startSerialConnection() {
    qDebug() << "Channel 1 - Frequency:" << Channel1FD.Freq << ", Duty Cycle:" << Channel1FD.Duty;
    qDebug() << "Channel 2 - Frequency:" << Channel2FD.Freq << ", Duty Cycle:" << Channel2FD.Duty;
    plot->plotDataChannel(graph1, 1, currentTime, Channel1FD.Freq, Channel1FD.Duty, updateTimeFlag);
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
    {
        updateTimeFlag = 1;
        currentTime = elapsedTimer.elapsed() / 1000.0; // Cập nhật currentTime ở đây
        QByteArray command(1, '1'); // Tạo QByteArray chứa ký tự '1'
        serialPort->write(command);
        connect(serialPort, &QSerialPort::readyRead, this, [=]() { 
            readSerialData(serialPort);
            //plot->plotDataChannel(graph1, buffer, currentTime, Channel1FD.Freq, Channel1FD.Duty, updateTimeFlag);
            //plot->plotDataChannel(graph2, buffer, currentTime, Channel2FD.Freq, Channel2FD.Duty, updateTimeFlag);
            }
        );
    }
}

void MainWindow::stopSerialConnection()
{
    if (serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Serial port stopped.";
    }
}

// void MainWindow::updateFrequencyAndDuty(int frequency, int dutyCycle)
// {
//     QString freqText = QString("Frequency: %1 Hz").arg(frequency);
//     QString dutyText = QString("Duty Cycle: %1%").arg(dutyCycle);

//     frequencyLabel->setText(freqText); // Cập nhật nhãn Frequency
//     dutyCycleLabel->setText(dutyText); // Cập nhật nhãn Duty Cycle
// }

void MainWindow::updateChannel1(int freq, int duty) {
    lblFreqChannel1->setText(QString("Freq: %1 Hz").arg(freq));
    lblDutyChannel1->setText(QString("Duty: %1 %").arg(duty));
}

void MainWindow::updateChannel2(int freq, int duty) {
    lblFreqChannel2->setText(QString("Freq: %1 Hz").arg(freq));
    lblDutyChannel2->setText(QString("Duty: %1 %").arg(duty));
}


MainWindow::~MainWindow()
{
    delete ui;
}
