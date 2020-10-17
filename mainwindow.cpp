#include <mainwindow.h>
#include <dcmotor.h>
#include <robot.h>
#include <plot2d.h>
#include <GLwidget.h>
#include <motorController.h>
#include <rpmmeter.h>

#include <QThread>
#include <QSettings>
#include <QLayout>
#include <QKeyEvent>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    pGLWidget     = nullptr;
    pLeftPlot     = nullptr;
    pRightPlot    = nullptr;

    pLeftSpeed    = nullptr;
    pRightSpeed   = nullptr;

    pLeftMotor    = nullptr;
    pRightMotor   = nullptr;

    pRightMotorThread = nullptr;
    pLeftMotorThread  = nullptr;

    restoreSettings();

    // Init the GUI Layout
    initLayout();

    if(!initGpio())
        exit(EXIT_FAILURE);

    // Create the two Speed Meters objects
    leftSpeedPin     = 22;
    rightSpeedPin    = 5;
    pLeftSpeed  = new RPMmeter(leftSpeedPin,  gpioHostHandle, nullptr);
    pRightSpeed = new RPMmeter(rightSpeedPin, gpioHostHandle, nullptr);

    // Create the two Motors objects
    leftForwardPin   = 24;
    leftBackwardPin  = 23;
    rightForwardPin  = 27;
    rightBackwardPin = 17;
    pLeftMotor  = new DcMotor(leftForwardPin,  leftBackwardPin,  gpioHostHandle);
    pRightMotor = new DcMotor(rightForwardPin, rightBackwardPin, gpioHostHandle);

    // Create the two Motor Controllers
    pLMotor = new MotorController(pLeftMotor,  pLeftSpeed);
    pRMotor = new MotorController(pRightMotor, pRightSpeed);

    // Create the Motor Controller Threads
    CreateLeftMotorThread();
    CreateRightMotorThread();

    // Send PID Parameters to the two Motor Controller
    emit LPvalueChanged(0.5);
    emit LIvalueChanged(0.0);
    emit LDvalueChanged(0.0);

    emit RPvalueChanged(0.5);
    emit RIvalueChanged(0.0);
    emit RDvalueChanged(0.0);

    // Create the Robot object
    pRobot = new Robot(pLMotor, pRMotor, parent);
    connect(pRobot, SIGNAL(sendOrientation(float, float, float, float)),
            this, SLOT(onUpdateOrientation(float, float, float, float)));
    // Start the two Motor Controller Threads
    emit operate();
}


MainWindow::~MainWindow() {
    if(gpioHostHandle >= 0)
        pigpio_stop(gpioHostHandle);
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    loopTimer.stop();
    saveSettings();
}


void
MainWindow::restoreSettings() {
    QSettings settings;
    // Restore Geometry and State of the window
    restoreGeometry(settings.value("Geometry").toByteArray());
}


void
MainWindow::saveSettings() {
    QSettings settings;
    // Window Position and Size
    settings.setValue("Geometry", saveGeometry());
}


bool
MainWindow::initGpio() {
    char host[sizeof("localhost")+1];
    strcpy(host, "localhost");
    char port[sizeof("8888")+1];
    strcpy(port, "8888");
    gpioHostHandle = pigpio_start(host, port);
    if(gpioHostHandle < 0)
        return false;
    return true;
}


void
MainWindow::CreateLeftMotorThread() {
    pLeftMotorThread = new QThread();
    pLMotor->moveToThread(pLeftMotorThread);
    connect(pLeftMotorThread, SIGNAL(finished()),
            this, SLOT(onLeftMotorThreadDone()));
    connect(this, SIGNAL(operate()),
            pLMotor, SLOT(go()));
    connect(this, SIGNAL(stopLMotor()),
            pLMotor, SLOT(terminate()));
    connect(this, SIGNAL(LPvalueChanged(double)),
            pLMotor, SLOT(setP(double)));
    connect(this, SIGNAL(LIvalueChanged(double)),
            pLMotor, SLOT(setI(double)));
    connect(this, SIGNAL(LDvalueChanged(double)),
            pLMotor, SLOT(setD(double)));
    connect(this, SIGNAL(LSpeedChanged(double)),
            pLMotor, SLOT(setSpeed(double)));

    // Start the Motor Controller Thread
    pLeftMotorThread->start();

    currentLspeed = 0.0;
    emit LSpeedChanged(currentLspeed);
}


void
MainWindow::CreateRightMotorThread() {
    pRightMotorThread = new QThread();
    pRMotor->moveToThread(pRightMotorThread);
    connect(pRightMotorThread, SIGNAL(finished()),
            this, SLOT(onRightMotorThreadDone()));
    connect(this, SIGNAL(operate()),
            pRMotor, SLOT(go()));
    connect(this, SIGNAL(stopRMotor()),
            pRMotor, SLOT(terminate()));
    connect(this, SIGNAL(RPvalueChanged(double)),
            pRMotor, SLOT(setP(double)));
    connect(this, SIGNAL(RIvalueChanged(double)),
            pRMotor, SLOT(setI(double)));
    connect(this, SIGNAL(RDvalueChanged(double)),
            pRMotor, SLOT(setD(double)));
    connect(this, SIGNAL(RSpeedChanged(double)),
            pRMotor, SLOT(setSpeed(double)));

    // Start the Motor Controller Thread
    pRightMotorThread->start();

    currentRspeed = 0.0;
    emit RSpeedChanged(currentRspeed);
}


void
MainWindow::onStartStopPushed() {
    if(pButtonStartStop->text()== QString("Start")) {

        pLeftPlot->ClearDataSet(1);
        pLeftPlot->ClearDataSet(2);
        pLeftPlot->ClearDataSet(3);
        nLeftPlotPoints = 0;

        pRightPlot->ClearDataSet(1);
        pRightPlot->ClearDataSet(2);
        pRightPlot->ClearDataSet(3);
        nRightPlotPoints = 0;

        connect(pLMotor, SIGNAL(MotorValues(double, double, double)),
                this, SLOT(onNewLMotorValues(double, double, double)));
        connect(pRMotor, SIGNAL(MotorValues(double, double, double)),
                this, SLOT(onNewRMotorValues(double, double, double)));

        currentLspeed = 1.0;
        currentRspeed = 1.0;
        emit LSpeedChanged(currentLspeed);
        emit RSpeedChanged(currentRspeed);

        pButtonStartStop->setText("Stop");
    }
    else {
        currentLspeed = 0.0;
        currentRspeed = 0.0;
        emit LSpeedChanged(currentLspeed);
        emit RSpeedChanged(currentRspeed);
        disconnect(pLMotor, SIGNAL(MotorValues(double, double, double)),
                this, SLOT(onNewLMotorValues(double, double, double)));
        disconnect(pRMotor, SIGNAL(MotorValues(double, double, double)),
                this, SLOT(onNewRMotorValues(double, double, double)));
        pButtonStartStop->setText("Start");
    }
}


void
MainWindow::createButtons() {
    pButtonStartStop = new QPushButton("Start", this);
    pButtonStartStop->setEnabled(true);
    connect(pButtonStartStop, SIGNAL(clicked()),
            this, SLOT(onStartStopPushed()));
}


void
MainWindow::initPlots() {
    pLeftPlot = new Plot2D(nullptr, "Left Motor");

    pLeftPlot->NewDataSet(1, 2, QColor(255,   0, 255), Plot2D::ipoint, "SetPt");
    pLeftPlot->NewDataSet(2, 2, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pLeftPlot->NewDataSet(3, 2, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pLeftPlot->SetShowTitle(1, true);
    pLeftPlot->SetShowTitle(2, true);
    pLeftPlot->SetShowTitle(3, true);

    pLeftPlot->SetShowDataSet(1, true);
    pLeftPlot->SetShowDataSet(2, true);
    pLeftPlot->SetShowDataSet(3, true);

    pLeftPlot->SetLimits(0.0, 1.0, -1.1, 1.1, true, false, false, false);
    pLeftPlot->UpdatePlot();
    pLeftPlot->show();

    nLeftPlotPoints = 0;

    pRightPlot = new Plot2D(nullptr, "Right Motor");

    pRightPlot->NewDataSet(1, 2, QColor(255,   0, 255), Plot2D::ipoint, "SetPt");
    pRightPlot->NewDataSet(2, 2, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pRightPlot->NewDataSet(3, 2, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pRightPlot->SetShowTitle(1, true);
    pRightPlot->SetShowTitle(2, true);
    pRightPlot->SetShowTitle(3, true);

    pRightPlot->SetShowDataSet(1, true);
    pRightPlot->SetShowDataSet(2, true);
    pRightPlot->SetShowDataSet(3, true);

    pRightPlot->SetLimits(0.0, 1.0, -1.0, 1.0, true, false, false, false);
    pRightPlot->UpdatePlot();
    pRightPlot->show();

    nRightPlotPoints = 0;
}


void
MainWindow::initLayout() {
    pGLWidget = new GLWidget(this);
    initPlots();
    QVBoxLayout* pPlotLayout = new (QVBoxLayout);
    pPlotLayout->addWidget(pLeftPlot);
    pPlotLayout->addWidget(pRightPlot);

    QHBoxLayout *firstRow = new QHBoxLayout;
    firstRow->addWidget(pGLWidget);
    firstRow->addLayout(pPlotLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstRow);
    setLayout(mainLayout);

    createButtons();
    QHBoxLayout *firstButtonRow = new QHBoxLayout;
    firstButtonRow->addWidget(pButtonStartStop);

    mainLayout->addLayout(firstButtonRow);
}


void
MainWindow::keyPressEvent(QKeyEvent *e) {
  if(e->key() == Qt::Key_Escape)
    close();
  else
    QWidget::keyPressEvent(e);
}


void
MainWindow::onNewLMotorValues(double wantedSpeed, double currentSpeed, double speed) {
    Q_UNUSED(wantedSpeed)
    //pLeftPlot->NewPoint(1, double(nLeftPlotPoints), wantedSpeed);
    pLeftPlot->NewPoint(2, double(nLeftPlotPoints), currentSpeed);
    pLeftPlot->NewPoint(3, double(nLeftPlotPoints), speed);
    pLeftPlot->UpdatePlot();
    nLeftPlotPoints++;
}


void
MainWindow::onNewRMotorValues(double wantedSpeed, double currentSpeed, double speed) {
    Q_UNUSED(wantedSpeed)
    //pRightPlot->NewPoint(1, double(nRightPlotPoints), wantedSpeed);
    pRightPlot->NewPoint(2, double(nRightPlotPoints), currentSpeed);
    pRightPlot->NewPoint(3, double(nRightPlotPoints), speed);
    pRightPlot->UpdatePlot();
    nRightPlotPoints++;
}


void
MainWindow::onLeftMotorThreadDone() {
    qDebug() << "onLeftMotorThreadDone()";
}


void
MainWindow::onRightMotorThreadDone() {
    qDebug() << "onRightMotorThreadDone()";
}


void
MainWindow::onUpdateOrientation(float q0, float q1, float q2, float q3) {
    pGLWidget->setRotation(q0, q1, q2, q3);
    pGLWidget->update();
}
