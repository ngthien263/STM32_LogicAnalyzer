#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSerialData(); // Hàm để đọc dữ liệu từ UART
    void updatePlot();

private:
    QSerialPort *serial;  // Cổng UART
    QCustomPlot *customPlot; // Đồ thị
    QTimer dataTimer;     // Bộ đếm thời gian để cập nhật dữ liệu
    QVector<double> highTimes, lowTimes; // Các vector lưu dữ liệu
    Ui::MainWindow *ui;
    int highTime;
    int lowTime;
    int signalTime;
    double beforeTime;

    // Trạng thái hiện tại (đang đọc thời gian mức cao hay mức thấp)
    enum State { HIGH = 1, LOW = 0 };
    State currentState;

    // Hàng đợi để lưu trữ dữ liệu đọc từ UART
    QQueue<int> dataSignalQueue;
    QQueue<int> dataTimeQueue;

    // Biến để lưu giá trị thời gian cũ nhằm loại bỏ dữ liệu cũ khi cần
    double oldKey;
};
#endif // MAINWINDOW_H
