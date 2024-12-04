#include "menu.h"

Menu::Menu(QWidget *parent)
{
    QFormLayout *menuLayout = new QFormLayout();

    portComboBox = new QComboBox();
    portComboBox->addItems({"COM1", "COM2", "COM3", "COM4"});
    menuLayout->addRow("Port", portComboBox);

    baudComboBox = new QComboBox();
    baudComboBox->addItems({"9600", "19200", "38400", "57600", "115200"});
    menuLayout->addRow("Baud", baudComboBox);

    stopBitsComboBox = new QComboBox();
    stopBitsComboBox->addItems({"0.5", "1", "1.5", "2"});
    menuLayout->addRow("Stop bit", stopBitsComboBox);

    parityBitsComboBox = new QComboBox();
    parityBitsComboBox->addItems({"None", "Even", "Odd"});
    menuLayout->addRow("Parity bit", parityBitsComboBox);

    pauseButton = new QPushButton();
    continueButton = new QPushButton();
    menuLayout->addRow(pauseButton, continueButton);
}

QString Menu::getPort()
{
    return portComboBox->currentText();
}
int Menu::getBaudRate()
{
    return baudComboBox->currentText().toInt();
}
QString Menu::getStopBits()  {
    return stopBitsComboBox->currentText();
}
QString Menu::getParity()  {
    return parityBitsComboBox->currentText();
}

void Menu::applySettings(
    QString &port,
    int &baudRate,
    QString &stopBits,
    QString &parity  )
{
    port     = getPort();
    baudRate = getBaudRate();
    stopBits = getStopBits();
    parity   = getParity();
}

