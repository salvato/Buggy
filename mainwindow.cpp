#include <mainwindow.h>
#include <dcmotor.h>
#include <robot.h>
#include <plot2d.h>
#include <GLwidget.h>
#include <robotmove.h>
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
    pMoveThread   = nullptr;
    pRobotMove    = nullptr;
    pLeftSpeed    = nullptr;
    pRightSpeed   = nullptr;

    restoreSettings();

    if(!initGpio())
        exit(EXIT_FAILURE);

    leftSpeedPin     = 5;
    rightSpeedPin    = 22;
    pLeftSpeed  = new RPMmeter(leftSpeedPin,  gpioHostHandle, nullptr);
    pRightSpeed = new RPMmeter(rightSpeedPin, gpioHostHandle, nullptr);

    leftForwardPin   = 27;
    leftBackwardPin  = 17;
    rightForwardPin  = 24;
    rightBackwardPin = 23;
    pLeftMotor  = new DcMotor(leftForwardPin,  leftBackwardPin,  gpioHostHandle, parent);
    pRightMotor = new DcMotor(rightForwardPin, rightBackwardPin, gpioHostHandle, parent);

    pRobot = new Robot(pLeftMotor, pRightMotor, parent);

    initLayout();

    loopTimer.setTimerType(Qt::PreciseTimer);
    connect(&loopTimer, SIGNAL(timeout()),
            this, SLOT(onLoopTimeElapsed()));
    loopTimer.start(100);
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
MainWindow::onStartStopPushed() {
    pButtonStartStop->setText("Stop");
    // Create the Moving Thread
    if(!pMoveThread) {
        pMoveThread = new QThread();
        connect(pMoveThread, SIGNAL(finished()),
                this, SLOT(onMoveThreadDone()));
    }
    if(!pRobotMove) {
        pRobotMove = new RobotMove(pRobot);
        pRobotMove->moveToThread(pMoveThread);
        connect(this, SIGNAL(startMove()),
                pRobotMove, SLOT(startMove()));
        connect(pRobotMove, SIGNAL(moveDone()),
                this, SLOT(onMoveDone()));
    }
    pMoveThread->start();
    emit startMove();
}


void
MainWindow::onMoveDone() {
    pButtonStartStop->setText("Start");
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

    pLeftPlot->NewDataSet(1, 3, QColor(255,   0, 255), Plot2D::ipoint, "SetPt");
    pLeftPlot->NewDataSet(2, 1, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pLeftPlot->NewDataSet(3, 1, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pLeftPlot->SetShowTitle(1, true);
    pLeftPlot->SetShowTitle(2, true);
    pLeftPlot->SetShowTitle(3, true);

    pLeftPlot->SetShowDataSet(1, true);
    pLeftPlot->SetShowDataSet(2, true);
    pLeftPlot->SetShowDataSet(3, true);

    pLeftPlot->SetLimits(0.0, 1.0, -1.0, 1.0, true, true, false, false);
    pLeftPlot->UpdatePlot();
    pLeftPlot->show();

    nLeftPlotPoints = 0;

    pRightPlot = new Plot2D(nullptr, "Right Motor");

    pRightPlot->NewDataSet(1, 3, QColor(255,   0, 255), Plot2D::ipoint, "SetPt");
    pRightPlot->NewDataSet(2, 1, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pRightPlot->NewDataSet(3, 1, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pRightPlot->SetShowTitle(1, true);
    pRightPlot->SetShowTitle(2, true);
    pRightPlot->SetShowTitle(3, true);

    pRightPlot->SetShowDataSet(1, true);
    pRightPlot->SetShowDataSet(2, true);
    pRightPlot->SetShowDataSet(3, true);

    pRightPlot->SetLimits(0.0, 1.0, -1.0, 1.0, true, true, false, false);
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
MainWindow::onLoopTimeElapsed() {
    pRobot->getOrientation(&q0, &q1, &q2, &q3);
    pGLWidget->setRotation(q0, q1, q2, q3);
    pGLWidget->update();
}


void
MainWindow::onMoveThreadDone() {

}
