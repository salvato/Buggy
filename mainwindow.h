#pragma once

#include <QWidget>
#include <QQuaternion>
#include <QByteArray>
#include <QSerialPort>
#include <QStatusBar>
#include <QTimer>


QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)
QT_FORWARD_DECLARE_CLASS(ControlsDialog)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QLineEdit)


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
    void initControls();
    bool serialConnect();
    void processData(QString sData);
    void disableUI();
    void enableUI();
    void connectSignals();

private:

signals:

private slots:
    void onTryToConnect();
    void onConnectPushed();
    void onStartStopPushed();
    void onPIDControlsPushed();
    void onNewDataAvailable();

    void onLPvalueChanged(int value);
    void onLIvalueChanged(int value);
    void onLDvalueChanged(int value);
    void onLSpeedChanged(int value);

    void onRPvalueChanged(int Pvalue);
    void onRIvalueChanged(int Ivalue);
    void onRDvalueChanged(int Dvalue);
    void onRSpeedChanged(int speed);

    void onHidePIDControls();
    void onKeepAlive();
    void onTimeToChangeSpeed();

private:
    GLWidget*        pGLWidget;
    Plot2D*          pLeftPlot;
    Plot2D*          pRightPlot;
    QPushButton*     pButtonConnect;
    QPushButton*     pButtonStartStop;
    QPushButton*     pButtonPIDControls;
    QLineEdit*       pEditObstacleDistance;
    ControlsDialog*  pPIDControlsDialog;
    QStatusBar*      pStatusBar;

    QSerialPort serialPort;
    QString     serialPortName;
    QString     receivedData;
    QQuaternion quat0, quat1;
    QTimer      connectionTimer;
    QTimer      keepAliveTimer;
    QTimer      changeSpeedTimer;

    float  q0, q1, q2, q3;
    double leftSpeed;
    double leftPath;
    double rightSpeed;
    double rightPath;
    double dTime, t0;
    int    nLeftPlotPoints;
    int    nRightPlotPoints;
    int    baudRate;

    double LPvalue;
    double LIvalue;
    double LDvalue;
    double LSpeed;

    double RPvalue;
    double RIvalue;
    double RDvalue;
    double RSpeed;

    double obstacleDistance;
    bool   bUpdateObstacleDistance;
    bool   bUpdateMotors;

    bool   bConnected;
};
