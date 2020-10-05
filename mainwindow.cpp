#include <mainwindow.h>
#include <robot.h>
#include <plot2d.h>
#include <GLwidget.h>
#include <robotmove.h>

#include <QThread>
#include <QSettings>
#include <QLayout>
#include <QKeyEvent>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    // Widgets
    , pGLWidget(nullptr)
    , pPlotVal(nullptr)
{
    pMovingThread = nullptr;
    pRobotMove    = nullptr;

    restoreSettings();
    if(!initGpio())
        exit(EXIT_FAILURE);
    pRobot = new Robot(27, 17, 24, 23, gpioHostHandle, parent);
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
MainWindow::go(Robot* pRobot) {
    double speed = 1.0;
    for(int i=0; i<3; i++) {
        pRobot->forward(speed);
        QThread::sleep(3);
        pRobot->stop();
        pRobot->right(speed);
        QThread::sleep(1);
        pRobot->stop();
    }
}


void
MainWindow::onStartStopPushed() {
    pButtonStartStop->setText("Stop");
    // Create the Moving Thread
    if(!pMovingThread) {
        pMovingThread = new QThread();
        connect(pMovingThread, SIGNAL(finished()),
                this, SLOT(onMoveThreadDone()));
    }
    if(!pRobotMove) {
        pRobotMove = new RobotMove(pRobot);
        pRobotMove->moveToThread(pMovingThread);
        connect(this, SIGNAL(startMove()),
                pRobotMove, SLOT(startMove()));
        connect(pRobotMove, SIGNAL(moveDone()),
                this, SLOT(onMoveDone()));
    }
    pMovingThread->start();
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
MainWindow::initPlot() {
    pPlotVal->NewDataSet(1, 1, QColor(255,   0,   0), Plot2D::ipoint, "X");
    pPlotVal->NewDataSet(2, 1, QColor(  0, 255,   0), Plot2D::ipoint, "Y");
    pPlotVal->NewDataSet(3, 1, QColor(  0,   0, 255), Plot2D::ipoint, "Z");
    pPlotVal->NewDataSet(4, 1, QColor(255, 255, 255), Plot2D::ipoint, "PID-In");
    pPlotVal->NewDataSet(5, 1, QColor(255, 255,  64), Plot2D::ipoint, "PID-Out");

    pPlotVal->SetShowTitle(1, true);
    pPlotVal->SetShowTitle(2, true);
    pPlotVal->SetShowTitle(3, true);
    pPlotVal->SetShowTitle(4, true);
    pPlotVal->SetShowTitle(5, true);

    pPlotVal->SetLimits(-1.0, 1.0, -1.0, 1.0, true, true, false, false);
}


void
MainWindow::initLayout() {
    pGLWidget = new GLWidget(this);
    pPlotVal = new Plot2D(this, "Plot");
    initPlot();

    QHBoxLayout *firstRow = new QHBoxLayout;
    firstRow->addWidget(pGLWidget);
    firstRow->addWidget(pPlotVal);

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
