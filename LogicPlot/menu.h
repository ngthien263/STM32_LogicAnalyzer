#ifndef MENU_H
#define MENU_H
#include <QWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
class Menu: public QWidget
{
    Q_OBJECT
public:
    explicit Menu(QWidget *parent);
    QString getPort();
    int getBaudRate();
    QString getStopBits();
    QString getParity();
private:
    QComboBox *portComboBox;
    QComboBox *baudComboBox;
    QComboBox *stopBitsComboBox;
    QComboBox *parityBitsComboBox;
    QPushButton *pauseButton;
    QPushButton *continueButton;
    QPushButton *applyButton;
private slots:
    void applySettings(QString &port,
                       int &baudRate,
                       QString &stopBits,
                       QString &parity);

};

#endif // MENU_H
