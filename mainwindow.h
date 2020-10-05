#pragma once

#include <QWidget>
#include <QTimer>
#include <pigpiod_if2.h>


QT_FORWARD_DECLARE_CLASS(Robot)
QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void initLayout();
    void restoreSettings();
    void saveSettings();
    void createButtons();
    void createPlot();

private slots:
    void onLoopTimeElapsed();


private:
    bool initGpio();

private:
    GLWidget* pGLWidget;
    Plot2D*   pPlotVal;
    Robot*    pRobot;

    QTimer    loopTimer;

    int gpioHostHandle;
    float q0, q1, q2, q3;
};
