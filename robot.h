#pragma once

#include <QObject>
#include <QTimer>

#include <sys/time.h>
#include <inttypes.h>


class Robot : public QObject
{
    Q_OBJECT
public:
    explicit Robot(QObject *parent = nullptr);
    bool forward(double speed);
    bool backward(double speed);
    bool stop();
    bool left(double speed);
    bool right(double speed);
    bool getOrientation(float* q0, float* q1, float* q2, float* q3);

signals:
    void sendOrientation(float q0, float q1, float q2, float q3);

public slots:
    void onUpdateTimeElapsed();
    void onTimeToSendData();

protected:

private:
    float samplingFrequency;

    QTimer updateTimer;
    QTimer sendDataTimer;
    __suseconds_t lastUpdate;
    __suseconds_t now;
    float delta;
    float values[9];

    float q0;
    float q1;
    float q2;
    float q3;
};
