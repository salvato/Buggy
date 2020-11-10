#include <mainwindow.h>
#include <plot2d.h>
#include <GLwidget.h>
#include "controlsdialog.h"

#include <QSettings>
#include <QLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QSlider>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , pGLWidget(nullptr)
    , pLeftPlot(nullptr)
    , pRightPlot(nullptr)
    , pControlsDialog(nullptr)
{
    initLayout();
    restoreSettings();

    quat0 = QQuaternion(1, 0, 0, 0).conjugated();
    receivedCommand = QString();
    baudRate = 9600; // 115200;
    serialPortName = QString("/dev/ttyACM0");

    if(!serialConnect()) {
        pStatusBar->showMessage(QString("Unable to open Serial Port !"));
        pButtonStartStop->setDisabled(true);
    }
    pControlsDialog = new ControlsDialog();
    pControlsDialog->show();
}


MainWindow::~MainWindow() {
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    saveSettings();
    if(pControlsDialog) {
        pControlsDialog->close();
        delete pControlsDialog;
    }
}


void
MainWindow::restoreSettings() {
    QSettings settings;
    restoreGeometry(settings.value("Geometry").toByteArray());
}


void
MainWindow::saveSettings() {
    QSettings settings;
    // Window Position and Size
    settings.setValue("Geometry", saveGeometry());
}


bool
MainWindow::serialConnect() {
    serialPort.setPortName(serialPortName);
    if(serialPort.isOpen()) {
        pStatusBar->showMessage(QString("Serial Port Already Opened !"));
        return false;
    }
    serialPort.setBaudRate(baudRate);
    if(!serialPort.open(QIODevice::ReadWrite)) {
        pStatusBar->showMessage(QString("Unable to open Serial Port"));
        return false;
    }
    serialPort.setParent(this);
    pStatusBar->showMessage(QString("uController connected to: ttyACM0"));
    connect(&serialPort, SIGNAL(readyRead()),
            this, SLOT(onNewDataAvailable()));
    return true;
}


void
MainWindow::onStartStopPushed() {
    if(pButtonStartStop->text()== QString("Start")) {
        quat0 = QQuaternion(q0, q1, q2, q3).conjugated();
        t0 = dTime;
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

    pLeftPlot->NewDataSet(1, 2, QColor(255, 196,   0), Plot2D::iline, "SetPt");
    pLeftPlot->NewDataSet(2, 2, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pLeftPlot->NewDataSet(3, 2, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pLeftPlot->SetShowTitle(1, true);
    pLeftPlot->SetShowTitle(2, true);
    pLeftPlot->SetShowTitle(3, true);

    pLeftPlot->SetShowDataSet(1, true);
    pLeftPlot->SetShowDataSet(2, true);
    pLeftPlot->SetShowDataSet(3, true);

    pLeftPlot->SetLimits(0.0, 1.0, -1.1, 1.1, true, true, false, false);
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

    pRightPlot->SetLimits(0.0, 1.0, -1.0, 1.0, true, true, false, false);
    pRightPlot->UpdatePlot();
    pRightPlot->show();

    nRightPlotPoints = 0;
}


void
MainWindow::initLayout() {
    pGLWidget = new GLWidget(this);

    initPlots();
    QVBoxLayout* pPlotLayout = new QVBoxLayout();
    pPlotLayout->addWidget(pLeftPlot);
    pPlotLayout->addWidget(pRightPlot);

    QHBoxLayout *firstRow = new QHBoxLayout;
    firstRow->addWidget(pGLWidget);
    firstRow->addLayout(pPlotLayout);

    createButtons();
    QHBoxLayout *firstButtonRow = new QHBoxLayout;
    firstButtonRow->addWidget(pButtonStartStop);

    pStatusBar = new QStatusBar();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(firstRow);
    mainLayout->addLayout(firstButtonRow);
    mainLayout->addWidget(pStatusBar);
    setLayout(mainLayout);
}


void
MainWindow::keyPressEvent(QKeyEvent *e) {
  if(e->key() == Qt::Key_Escape)
    close();
  else
    QWidget::keyPressEvent(e);
}


void
MainWindow::onNewDataAvailable() {
    receivedCommand += serialPort.readAll();
    QString sNewCommand;
    int iPos;
    iPos = receivedCommand.indexOf("\n");
    while(iPos != -1) {
        sNewCommand = receivedCommand.left(iPos);
        executeCommand(sNewCommand);
        receivedCommand = receivedCommand.mid(iPos+1);
        iPos = receivedCommand.indexOf("#");
    }
}


void
MainWindow::executeCommand(QString command) {
    QStringList tokens = command.split(',');
    if(tokens.length() == 7) {
        q0        = tokens.at(0).toDouble()/1000.0;
        q1        = tokens.at(1).toDouble()/1000.0;
        q2        = tokens.at(2).toDouble()/1000.0;
        q3        = tokens.at(3).toDouble()/1000.0;
        leftSpeed = tokens.at(4).toDouble()/100.0;
        leftPath  = tokens.at(5).toDouble();
        dTime     = tokens.at(6).toDouble();

        quat1 = QQuaternion(q0, q1, q2, q3)*quat0;
        pGLWidget->setRotation(quat1);
        pLeftPlot->NewPoint(1, dTime-t0, leftSpeed);
        pGLWidget->update();
        pLeftPlot->UpdatePlot();
    }
}

