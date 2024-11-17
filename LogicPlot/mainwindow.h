#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "qcustomplot.h"
#include <QElapsedTimer>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void readSerialData();
    void pausePlot();       // Ensure correct naming
    void continuePlot();    // Ensure correct naming
    //void setupTimer();
private:
    Ui::MainWindow *ui;
    QCustomPlot *customPlot;
    QSerialPort *serial;
    QElapsedTimer elapsedTimer;
    double lastByteTime;
    bool isZooming;
    bool isPaused; // Correctly declare isPaused
    double actX;     // Active X position
    double timeFrame ; // Time frame for the x-axis
    bool autorun; // Flag to auto-follow new data

    std::vector<std::pair<double, char>> dataBuffer;
    void plotData(double currentTime, int byte);

};

#endif // MAINWINDOW_H
