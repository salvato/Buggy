#ifndef RPMMETER_H
#define RPMMETER_H

#include <QObject>

class RPMmeter : public QObject
{
    Q_OBJECT
public:
    explicit RPMmeter(int gpioPin, int gpioHandle, QObject *parent = nullptr);

signals:

public slots:


private:
    int inputPin;
    int gpioHostHandle;
};

#endif // RPMMETER_H
