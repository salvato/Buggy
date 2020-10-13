#include "robotmove.h"
#include "robot.h"

RobotMove::RobotMove(Robot* pRobot, QObject *parent)
    : QObject(parent)
    , pRobby(pRobot)
{
}


void
RobotMove::startMove() {
    double speed = 0.25;
    for(int i=0; i<3; i++) {
        pRobby->forward(speed);
        QThread::sleep(3);
        pRobby->stop();
        pRobby->right(speed);
        QThread::sleep(1);
        pRobby->stop();
    }
    emit moveDone();
}
