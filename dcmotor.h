#pragma once

#include <QObject>

class DcMotor : public QObject
{
    Q_OBJECT

public:
    explicit DcMotor(uint forwardPin, uint reversPin, int gpioHandle, QObject *parent = nullptr);
    bool stop();
    bool goForward(double speed=1.0);
    bool goBackward(double speed=1.0);

signals:

public slots:

private:
    bool init();

private:
    uint forward;
    uint backward;
    uint pwmFrequency;
    int gpioHostHandle;
    int forwardPWMrange;
    int backwardPWMrange;
};
