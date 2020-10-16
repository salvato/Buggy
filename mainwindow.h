#pragma once

#include <QWidget>
#include <QTimer>
#include <pigpiod_if2.h>


QT_FORWARD_DECLARE_CLASS(Robot)
QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QThread)
QT_FORWARD_DECLARE_CLASS(RobotMove)
QT_FORWARD_DECLARE_CLASS(RPMmeter)
QT_FORWARD_DECLARE_CLASS(DcMotor)


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void restoreSettings();
    void saveSettings();
    void createButtons();
    void initLayout();
    void initPlots();
    bool initGpio();

private:

signals:
    void startMove();

private slots:
    void onLoopTimeElapsed();
    void onStartStopPushed();
    void onMoveDone();
    void onMoveThreadDone();

private:
    uint leftSpeedPin;
    uint rightSpeedPin;
    uint leftForwardPin;
    uint leftBackwardPin;
    uint rightForwardPin;
    uint rightBackwardPin;

    GLWidget*    pGLWidget;
    Plot2D*      pLeftPlot;
    Plot2D*      pRightPlot;
    RPMmeter*    pLeftSpeed;
    RPMmeter*    pRightSpeed;
    DcMotor*     pLeftMotor;
    DcMotor*     pRightMotor;
    Robot*       pRobot;
    QPushButton* pButtonStartStop;
    QThread*     pMoveThread;
    RobotMove*   pRobotMove;

    QTimer       loopTimer;

    int   gpioHostHandle;
    float q0, q1, q2, q3;
    int   nLeftPlotPoints;
    int   nRightPlotPoints;
};
