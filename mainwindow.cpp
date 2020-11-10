#include <mainwindow.h>
#include <robot.h>
#include <plot2d.h>
#include <GLwidget.h>

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

    restoreSettings();

    // Init the GUI Layout
    initLayout();

    quat0 = QQuaternion(1, 0, 0, 0).conjugated();
}


MainWindow::~MainWindow() {
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
MainWindow::connectToMicroController() {
    serialPort.setPortName("/dev/ttyACM0");
    if(serialPort.isOpen())
        return false;
    serialPort.setBaudRate(115200);
    if(!serialPort.open(QIODevice::ReadWrite))
        return false;
    serialPort.setParent(this);
    //editHostName->setText(QString("uController connected to: %1").arg(info.portName()));
    return true;
}


void
MainWindow::onStartStopPushed() {
    if(pButtonStartStop->text()== QString("Start")) {
        if(!connectToMicroController()) {
            perror("Unable to connect to uC");
            return;
        }
        connect(&serialPort, SIGNAL(readyRead()),
                this, SLOT(onNewDataAvailable()));
        pRobot->getOrientation(&q0, &q1, &q2, &q3);
        quat0 = QQuaternion(q0, q1, q2, q3).conjugated();
        pLeftPlot->ClearDataSet(1);
        pLeftPlot->ClearDataSet(2);
        pLeftPlot->ClearDataSet(3);
        nLeftPlotPoints = 0;

        pRightPlot->ClearDataSet(1);
        pRightPlot->ClearDataSet(2);
        pRightPlot->ClearDataSet(3);
        nRightPlotPoints = 0;

        pButtonStartStop->setText("Stop");
    }
    else {
        currentLspeed = 0.0;
        currentRspeed = 0.0;
        pButtonStartStop->setText("Start");
    }
}


void
MainWindow::onNewDataAvailable() {
    receivedCommand += serialPort.readAll();
    QString sNewCommand;
    int iPos;
    iPos = receivedCommand.indexOf("#");
    while(iPos != -1) {
        sNewCommand = receivedCommand.left(iPos);
        executeCommand(sNewCommand);
        receivedCommand = receivedCommand.mid(iPos+1);
        iPos = receivedCommand.indexOf("#");
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

    pLeftPlot->NewDataSet(1, 2, QColor(255, 196, 0), Plot2D::iline, "SetPt");
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

    pRightPlot->NewDataSet(1, 2, QColor(255, 196,   0), Plot2D::iline, "SetPt");
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
MainWindow::onUpdateOrientation(float q0, float q1, float q2, float q3) {
    quat1 = QQuaternion(q0, q1, q2, q3)*quat0;
    pGLWidget->setRotation(quat1);
    pGLWidget->update();
}
