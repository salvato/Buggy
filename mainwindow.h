#pragma once

#include <QWidget>
#include <QQuaternion>
#include <QByteArray>
#include <QSerialPort>
#include <QStatusBar>


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
    void onStartStopPushed();
    void onPIDControlsPushed();
    void onNewDataAvailable();

    void onLPvalueChanged(double value);
    void onLIvalueChanged(double value);
    void onLDvalueChanged(double value);
    void onLSpeedChanged(double value);

    void onRPvalueChanged(double Pvalue);
    void onRIvalueChanged(double Ivalue);
    void onRDvalueChanged(double Dvalue);
    void onRSpeedChanged(double speed);

private:
    GLWidget*        pGLWidget;
    Plot2D*          pLeftPlot;
    Plot2D*          pRightPlot;
    QPushButton*     pButtonStartStop;
    QPushButton*     pButtonPIDControls;
    ControlsDialog*  pPIDControlsDialog;
    QStatusBar*      pStatusBar;

    QSerialPort serialPort;
    QString     serialPortName;
    QString     receivedData;
    QQuaternion quat0, quat1;

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
};
