#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <QComboBox>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
class Serial;
class Plot;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(Serial *serial, Plot *plot, QWidget *parent = nullptr);
    ~MainWindow();
    QCustomPlot *customPlot;
    static int IsFreqAndDutyRead_ch1;
    static int IsFreqAndDutyRead_ch2;
    static int IsFreqAndDutyRead;
    static QCPGraph *graph1;
    static QCPGraph *graph2;
    static QCPGraph *graph3;
    void updateChannel1(int freq, int duty);
    void updateChannel2(int freq, int duty);
private slots:
    void updateCOMPorts();
    void startSerialConnection();
    void stopSerialConnection();
    void openSerialPort();
    void readSerialData(QSerialPort *serialPort);
    void getFreqandDuty(int channel);
    //void updateFrequencyAndDuty(int frequency, int dutyCycle);
    // void handleTimeout();
private:

    Ui::MainWindow *ui;
    Plot *plot;
    Serial *serial;
    QSerialPort *serialPort;
    QByteArray buffer;
    QElapsedTimer elapsedTimer; // Add this line

    void setupUI();              // Cài đặt giao diện
    void setupPlot();            // Cài đặt đồ thị

    QPushButton *startButton;    // Nút bắt đầu
    QPushButton *stopButton;    // Nút dừng
    QPushButton *openButton;

    QComboBox *baudrateComboBox; // ComboBox chọn baudrate
    QComboBox *comPortComboBox;  // ComboBox chọn COM port
    QGroupBox *createChannelGroup(const QString &title, QLabel *&freqLabel, QLabel *&dutyLabel);


    QLabel *frequencyLabel;      // Nhãn hiển thị tần số
    QLabel *dutyCycleLabel;

    QLabel *lblFreqChannel1;
    QLabel *lblDutyChannel1;
    QLabel *lblFreqChannel2;
    QLabel *lblDutyChannel2;

    int channel_1_time;
    int channel_2_time;

    char lastByte;
};

#endif // MAINWINDOW_H
