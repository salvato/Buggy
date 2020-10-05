#pragma once

#include <QWidget>
#include <QTimer>
#include <pigpiod_if2.h>


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

signals:

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void restoreSettings();
    void saveSettings();
    void createButtons();
    void initLayout();
    void initPlot();
    bool initGpio();

private slots:
    void onLoopTimeElapsed();
    void onStartStopPushed();

private:
    static void go(Robot* pRobot);

private:
    GLWidget*    pGLWidget;
    Plot2D*      pPlotVal;
    Robot*       pRobot;
    QPushButton* pButtonStartStop;
    QThread*     pMovingThread;

    QTimer       loopTimer;

    int   gpioHostHandle;
    float q0, q1, q2, q3;
};
