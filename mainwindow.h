#pragma once

#include <QWidget>
#include <QTimer>
#include <QQuaternion>


QT_FORWARD_DECLARE_CLASS(Robot)
QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QThread)


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

private slots:
    void onStartStopPushed();

    void onUpdateOrientation(float q0, float q1, float q2, float q3);

private:

private:

    double currentLspeed;
    double currentRspeed;

    GLWidget*        pGLWidget;
    Plot2D*          pLeftPlot;
    Plot2D*          pRightPlot;

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
