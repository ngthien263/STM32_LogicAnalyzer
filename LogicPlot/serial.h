#ifndef SERIAL_H
#define SERIAL_H

#include <QSerialPort>
#include <QByteArray>
#include <QElapsedTimer>
#include <QSerialPort>
#include <QObject>
#include <QDebug>
#include <QLabel>
#include "qcustomplot.h"

class Plot;

class Serial : public QObject {
    Q_OBJECT

private:
    QSerialPort *serial;
    QElapsedTimer elapsedTimer;
    Plot *plot;
    QCustomPlot *customPlot;
    QByteArray buffer;
    int receivedFrequency;
    int receivedDutyCycle;
    int IsFreqAndDutyRead = 0;

    QLabel *frequencyLabel;      // Nhãn hiển thị tần số
    QLabel *dutyCycleLabel;

public:
    explicit Serial(QObject *parent = nullptr);

    void setPlot(Plot *plotObj);
    QElapsedTimer getElapsedTimer() const;
    void updateFrequencyAndDuty(int frequency, int dutyCycle);

    // Getter and Setter for QSerialPort
    QSerialPort* getSerial() const;
    void setSerial(QSerialPort* port);
    QByteArray getBuffer() const;

    // Getter không cần đánh dấu là slot
    int getFrequency() const;
    int getDutyCycle() const;

public slots:
    //void readSerialData(QSerialPort *serial);
    bool setupSerialPort(QSerialPort *serialPort, const QString &portName, int baudRate);

    // Hàm thử nghiệm
    //void testReadSerialData();
};

#endif // SERIAL_H
