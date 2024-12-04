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

private slots:
    void updateCOMPorts();
    void startSerialConnection();
    void stopSerialConnection();
    void openSerialPort();

private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    Plot *plot;
    Serial *serial;
    QSerialPort *serialPort;
    QByteArray buffer;

    void setupUI();              // Cài đặt giao diện
    void setupPlot();            // Cài đặt đồ thị

    QPushButton *startButton;    // Nút bắt đầu
    QPushButton *stopButton;    // Nút dừng
    QPushButton *openButton;

    QComboBox *baudrateComboBox; // ComboBox chọn baudrate
    QComboBox *comPortComboBox;  // ComboBox chọn COM port

    QLabel *frequencyLabel;      // Nhãn hiển thị tần số
    QLabel *dutyCycleLabel;

};

#endif // MAINWINDOW_H
