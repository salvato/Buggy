#pragma once

#include <QWidget>
#include <QQuaternion>
#include <QByteArray>
#include <QSerialPort>
#include <QStatusBar>


QT_FORWARD_DECLARE_CLASS(GLWidget)
QT_FORWARD_DECLARE_CLASS(Plot2D)
QT_FORWARD_DECLARE_CLASS(QPushButton)


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
    bool serialConnect();
    void executeCommand(QString command);

private:

signals:

private slots:
    void onStartStopPushed();
    void onNewDataAvailable();

private:
    GLWidget*        pGLWidget;
    Plot2D*          pLeftPlot;
    Plot2D*          pRightPlot;
    QPushButton*     pButtonStartStop;
    QStatusBar*      pStatusBar;

    QSerialPort serialPort;
    QString     serialPortName;
    QString     receivedCommand;
    QQuaternion quat0, quat1;

    float  q0, q1, q2, q3;
    double leftSpeed;
    double leftPath;
    double dTime, t0;
    int    nLeftPlotPoints;
    int    nRightPlotPoints;
    int    baudRate;
};
