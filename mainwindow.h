#pragma once

#include <QWidget>
#include <QTimer>
#include <QQuaternion>
#include <pigpiod_if2.h>


QT_FORWARD_DECLARE_CLASS(Robot)
QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QThread)
QT_FORWARD_DECLARE_CLASS(MotorController)
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
    void operate();

    void stopLMotor();
    void LPvalueChanged(double Pvalue);
    void LIvalueChanged(double Ivalue);
    void LDvalueChanged(double Dvalue);
    void LSpeedChanged(double speed);

    void stopRMotor();
    void RPvalueChanged(double Pvalue);
    void RIvalueChanged(double Ivalue);
    void RDvalueChanged(double Dvalue);
    void RSpeedChanged(double speed);

private slots:
    void onStartStopPushed();

    void onLeftMotorThreadDone();
    void onNewLMotorValues(double wantedSpeed, double currentSpeed, double speed);
    void onRightMotorThreadDone();
    void onNewRMotorValues(double wantedSpeed, double currentSpeed, double speed);

    void onUpdateOrientation(float q0, float q1, float q2, float q3);

private:
    void CreateLeftMotorThread();
    void CreateRightMotorThread();

private:
    uint leftSpeedPin;
    uint rightSpeedPin;
    uint leftForwardPin;
    uint leftBackwardPin;
    uint rightForwardPin;
    uint rightBackwardPin;

    double currentLspeed;
    double currentRspeed;

    GLWidget*        pGLWidget;
    Plot2D*          pLeftPlot;
    Plot2D*          pRightPlot;

    RPMmeter*        pLeftSpeed;
    RPMmeter*        pRightSpeed;
    DcMotor*         pLeftMotor;
    DcMotor*         pRightMotor;
    MotorController* pLMotor;
    MotorController* pRMotor;
    QThread*         pRightMotorThread;
    QThread*         pLeftMotorThread;

    Robot*           pRobot;

    QPushButton*     pButtonStartStop;

    QTimer loopTimer;

    int   gpioHostHandle;
    QQuaternion quat0, quat1;
    float q0, q1, q2, q3;
    int   nLeftPlotPoints;
    int   nRightPlotPoints;
};
