#include <mainwindow.h>
#include <plot2d.h>
#include <GLwidget.h>
#include "controlsdialog.h"

#include <QSettings>
#include <QLayout>
#include <QKeyEvent>
#include <QPushButton>
#include <QSlider>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , pGLWidget(nullptr)
    , pLeftPlot(nullptr)
    , pRightPlot(nullptr)
    , pPIDControlsDialog(nullptr)
{
    initLayout();
    restoreSettings();

    quat0 = QQuaternion(1, 0, 0, 0).conjugated();
    receivedData = QString();
    baudRate = 9600; // 115200;
    serialPortName = QString("/dev/ttyACM0");

    if(!serialConnect()) {
        pStatusBar->showMessage(QString("Unable to open Serial Port !"));
        disableUI();
    }
    pPIDControlsDialog = new ControlsDialog();
    connectSignals();
}


MainWindow::~MainWindow() {
}


void
MainWindow::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString("...Exiting Buggy..."));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString("Exiting Program..."));
    msgBox.setInformativeText(QString("Are you Sure ?"));
    msgBox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    if(msgBox.exec() == QDialogButtonBox::Ok) {
        saveSettings();
        if(pPIDControlsDialog) {
            pPIDControlsDialog->close();
            delete pPIDControlsDialog;
            event->accept();
        }
    }
    else {
        event->ignore();
    }
}


void
MainWindow::disableUI() {
    pButtonStartStop->setDisabled(true);
    pButtonPIDControls->setDisabled(true);
}


void
MainWindow::enableUI() {
    pButtonStartStop->setEnabled(true);
    pButtonPIDControls->setEnabled(true);
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
MainWindow::createButtons() {
    pButtonStartStop = new QPushButton("Start", this);
    pButtonStartStop->setEnabled(true);
    pButtonPIDControls = new QPushButton("PID Ctrl", this);
    connect(pButtonStartStop, SIGNAL(clicked()),
            this, SLOT(onStartStopPushed()));
    connect(pButtonPIDControls, SIGNAL(clicked()),
            this, SLOT(onPIDControlsPushed()));
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
    firstButtonRow->addWidget(pButtonPIDControls);

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
MainWindow::connectSignals() {
    connect(pPIDControlsDialog,SIGNAL(LPvalueChanged(double)),
            this, SLOT(onLPvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(LIvalueChanged(double)),
            this, SLOT(onLIvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(LDvalueChanged(double)),
            this, SLOT(onLDvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(LSpeedChanged(double)),
            this, SLOT(onLSpeedChanged(double)));

    connect(pPIDControlsDialog,SIGNAL(RPvalueChanged(double)),
            this, SLOT(onRPvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(RIvalueChanged(double)),
            this, SLOT(onRIvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(RDvalueChanged(double)),
            this, SLOT(onRDvalueChanged(double)));
    connect(pPIDControlsDialog,SIGNAL(RSpeedChanged(double)),
            this, SLOT(onRSpeedChanged(double)));
}


void
MainWindow::processData(QString sData) {
    QStringList tokens = sData.split(',');
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
        serialPort.write("Start\n");
        pButtonStartStop->setText("Stop");
    }
    else {
        serialPort.write("Stop\n");
        pButtonStartStop->setText("Start");
    }
}


void
MainWindow::onPIDControlsPushed() {
    pPIDControlsDialog->exec();
}


void
MainWindow::onNewDataAvailable() {
    receivedData += serialPort.readAll();
    QString sNewData;
    int iPos = receivedData.indexOf("\n");
    while(iPos != -1) {
        sNewData = receivedData.left(iPos);
        processData(sNewData);
        receivedData = receivedData.mid(iPos+1);
        iPos = receivedData.indexOf("\n");
    }
}


void
MainWindow::onLPvalueChanged(double value) {
    LPvalue = value;
}


void
MainWindow::onLIvalueChanged(double value) {
    LIvalue = value;
}


void
MainWindow::onLDvalueChanged(double value) {
    LDvalue = value;
}


void
MainWindow::onLSpeedChanged(double value) {
    LSpeed = value;
}


void
MainWindow::onRPvalueChanged(double value) {
    RPvalue = value;
}


void
MainWindow::onRIvalueChanged(double value) {
    RIvalue = value;
}


void
MainWindow::onRDvalueChanged(double value) {
    RDvalue = value;
}


void
MainWindow::onRSpeedChanged(double value) {
    RSpeed = value;
}

