#ifndef PLOT_H
#define PLOT_H

#include <QSerialPort>
#include <QElapsedTimer>
#include <QDebug>
#include "qcustomplot.h"
#include "mainwindow.h"
class Serial;

class Plot : public QObject{
    Q_OBJECT

private:
    double lastByteTime = 0.0;
    bool isZooming;
    bool isPaused; // Correctly declare isPaused
    double actX;     // Active X position
    double timeFrame ; // Time frame for the x-axis
    bool autorun; // Flag to auto-follow new data

    Serial *serial;
    // Serial *elapsedTimer;
    // Serial *buffer;

    int IsFreqAndDutyRead = 0;

    int disconectedFlag = 0;

    char lastByte = 0;
    QCustomPlot *customPlot;
    QElapsedTimer elapsedTimer;
public:
    explicit Plot(QObject *parent = nullptr); // Constructor mặc định

    explicit Plot(QCustomPlot *plot, QObject *parent = nullptr);

    void plotData(QCustomPlot *customPlot, QByteArray &buffer, double &currentTime, int receivedFrequency, int receivedDutyCycle, int &updateTimeFlag);

    QCustomPlot* setupPlot();

    //void continuePlot(QCustomPlot *customPlot, double currentTime);
public slots:
    //void pausePlot();       // Ensure correct naming
    //void continuePlot();    // Ensure correct naming
    void setSerial(Serial *serialObj);

};
#endif // PLOT_H
