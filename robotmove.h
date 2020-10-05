#pragma once

#include <QObject>


QT_FORWARD_DECLARE_CLASS(Robot)


class RobotMove : public QObject
{
    Q_OBJECT
public:
    explicit RobotMove(Robot* pRobot, QObject *parent = nullptr);

signals:

public slots:
    void startMove();

public slots:

};
