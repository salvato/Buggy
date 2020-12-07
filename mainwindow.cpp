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


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , pGLWidget(nullptr)
    , pLeftPlot(nullptr)
    , pRightPlot(nullptr)
    , pPIDControlsDialog(nullptr)
{
    setWindowIcon(QIcon(":/plot.png"));
    initLayout();
    restoreSettings();

    quat0          = QQuaternion(1, 0, 0, 0).conjugated();
    receivedData   = QString();
    baudRate       = QSerialPort::Baud38400;
    serialPortName = QString("/dev/ttyACM0");
    t0             =-1.0;
    LSpeed         = 0.0;
    RSpeed         = 0.0;
    iSign          = 1;

    connect(&keepAliveTimer, SIGNAL(timeout()),
            this, SLOT(onKeepAlive()));

    connect(&connectionTimer, SIGNAL(timeout()),
            this, SLOT(onTryToConnect()));

    connect(&changeSpeedTimer, SIGNAL(timeout()),
            this, SLOT(onTimeToChangeSpeed()));

    pPIDControlsDialog = new ControlsDialog();
    connectSignals();

    disableUI();
    pStatusBar->showMessage(QString("Wait: Connecting to Buggy..."));
    connectionTimer.start(500);
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
        }
        if(serialPort.isOpen())
            serialPort.close();
        event->accept();
    }
    else {
        event->ignore();
    }
}


void
MainWindow::disableUI() {
    pButtonConnect->setDisabled(true);
    pButtonStartStop->setDisabled(true);
    pButtonPIDControls->setDisabled(true);
}


void
MainWindow::enableUI() {
    pButtonConnect->setEnabled(true);
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


void
MainWindow::initCamera() {
  // Set(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ)
  camera.Set(0.0,  10.0, 0.0,
             0.0,  0.0,  0.0,
             0.0,  0.0, -1.0);
  camera.FieldOfView(90.0);
  camera.MouseMode(CGrCamera::PITCHYAW);
  camera.Gravity(false);
}


bool
MainWindow::serialConnect() {
    serialPort.setPortName(serialPortName);
    if(serialPort.isOpen())
        serialPort.close();
    if(!serialPort.open(QIODevice::ReadWrite)) {
        pStatusBar->showMessage(QString("No Buggy Connected Till Now..."));
        return false;
    }
    serialPort.setBaudRate(baudRate);
    serialPort.setParent(this);
    serialPort.readAll(); // Discard Input Buffer
    connect(&serialPort, SIGNAL(readyRead()),
            this, SLOT(onNewDataAvailable()));
    pStatusBar->showMessage(QString("Buggy Ready to Connect via ttyACM0"));
    bConnected = false;
    return true;
}


void
MainWindow::createButtons() {
    pButtonConnect     = new QPushButton("Connect",  this);
    pButtonStartStop   = new QPushButton("Start",    this);
    pButtonPIDControls = new QPushButton("PID Ctrl", this);

    connect(pButtonConnect, SIGNAL(clicked()),
            this, SLOT(onConnectPushed()));
    connect(pButtonStartStop, SIGNAL(clicked()),
            this, SLOT(onStartStopPushed()));
    connect(pButtonPIDControls, SIGNAL(clicked()),
            this, SLOT(onPIDControlsPushed()));
}


void
MainWindow::initPlots() {
    /////////////////////////
    // Init Left Motor Plot !
    /////////////////////////
    pLeftPlot = new Plot2D(nullptr, "Left Motor");

    pLeftPlot->NewDataSet(1, 2, QColor(128, 128, 255), Plot2D::iline, "SetPt");
    pLeftPlot->NewDataSet(2, 2, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pLeftPlot->NewDataSet(3, 2, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pLeftPlot->SetShowTitle(1, true);
    pLeftPlot->SetShowTitle(2, true);
    pLeftPlot->SetShowTitle(3, true);

    pLeftPlot->SetShowDataSet(1, true);
    pLeftPlot->SetShowDataSet(2, true);
    pLeftPlot->SetShowDataSet(3, true);

    pLeftPlot->SetLimits(0.0, 1.0, -1.1, 1.1, true, true, false, false);
    pLeftPlot->setMaxPoints(600);
    pLeftPlot->UpdatePlot();
    pLeftPlot->show();

    nLeftPlotPoints = 0;

    //////////////////////////
    // Init Right Motor Plot !
    //////////////////////////
    pRightPlot = new Plot2D(nullptr, "Right Motor");

    pRightPlot->NewDataSet(1, 2, QColor(128, 128, 255), Plot2D::iline, "SetPt");
    pRightPlot->NewDataSet(2, 2, QColor(255, 255,   0), Plot2D::iline, "Speed");
    pRightPlot->NewDataSet(3, 2, QColor(  0, 255, 255), Plot2D::iline, "PID-Out");

    pRightPlot->SetShowTitle(1, true);
    pRightPlot->SetShowTitle(2, true);
    pRightPlot->SetShowTitle(3, true);

    pRightPlot->SetShowDataSet(1, true);
    pRightPlot->SetShowDataSet(2, true);
    pRightPlot->SetShowDataSet(3, true);

    pRightPlot->SetLimits(0.0, 1.0, -1.0, 1.0, true, true, false, false);
    pRightPlot->setMaxPoints(600);
    pRightPlot->UpdatePlot();
    pRightPlot->show();

    nRightPlotPoints = 0;
}


void
MainWindow::initLayout() {
    initCamera();
    pGLWidget = new GLWidget(&camera, this);

    initPlots();
    QVBoxLayout* pPlotLayout = new QVBoxLayout();
    pPlotLayout->addWidget(pLeftPlot);
    pPlotLayout->addWidget(pRightPlot);

    QHBoxLayout *firstRow = new QHBoxLayout;
    firstRow->addWidget(pGLWidget);
    firstRow->addLayout(pPlotLayout);

    createButtons();
    QHBoxLayout *firstButtonRow = new QHBoxLayout;
    firstButtonRow->addWidget(pButtonConnect);
    firstButtonRow->addWidget(pButtonStartStop);
    firstButtonRow->addWidget(pButtonPIDControls);
    pEditObstacleDistance = new QLineEdit();
    firstButtonRow->addWidget(pEditObstacleDistance);

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
    connect(pPIDControlsDialog, SIGNAL(LPvalueChanged(int)),
            this, SLOT(onLPvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(LIvalueChanged(int)),
            this, SLOT(onLIvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(LDvalueChanged(int)),
            this, SLOT(onLDvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(LSpeedChanged(int)),
            this, SLOT(onLSpeedChanged(int)));

    connect(pPIDControlsDialog, SIGNAL(RPvalueChanged(int)),
            this, SLOT(onRPvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(RIvalueChanged(int)),
            this, SLOT(onRIvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(RDvalueChanged(int)),
            this, SLOT(onRDvalueChanged(int)));
    connect(pPIDControlsDialog, SIGNAL(RSpeedChanged(int)),
            this, SLOT(onRSpeedChanged(int)));

    connect(pPIDControlsDialog, SIGNAL(ControlsDone()),
            this, SLOT(onHidePIDControls()));
}


void
MainWindow::processData(QString sData) {
    bUpdateMotors = false;
    bUpdateObstacleDistance = false;
    QStringList tokens = sData.split(',');
    while(!tokens.isEmpty()) {
        QString sHeader = tokens.first();
        int nTokens = tokens.length();
        if(sHeader == "A" && nTokens > 4) {
            tokens.removeFirst();
            q0 = tokens.first().toDouble()/1000.0;
            tokens.removeFirst();
            q1 = tokens.first().toDouble()/1000.0;
            tokens.removeFirst();
            q2 = tokens.first().toDouble()/1000.0;
            tokens.removeFirst();
            q3 = tokens.first().toDouble()/1000.0;
            tokens.removeFirst();
            quat1 = QQuaternion(q0, q1, q2, q3)*quat0;
            pGLWidget->setRotation(quat1);
            pGLWidget->update();
        }
        else if(sHeader == "M" && nTokens > 4) {
            tokens.removeFirst();
            leftSpeed = tokens.first().toDouble()/100.0;
            tokens.removeFirst();
            leftPath = tokens.first().toDouble();
            tokens.removeFirst();
            rightSpeed = tokens.first().toDouble()/100.0;
            tokens.removeFirst();
            rightPath = tokens.first().toDouble();
            tokens.removeFirst();
            bUpdateMotors = true;
        }
        else if(sHeader == "D") {
            tokens.removeFirst();
            obstacleDistance = tokens.first().toDouble();
            tokens.removeFirst();
            pEditObstacleDistance->setText(QString("%1").arg(obstacleDistance));
            bUpdateObstacleDistance = true;
        }
        else if(sHeader == "T" && nTokens > 1) {
            tokens.removeFirst();
            dTime = tokens.first().toDouble();
            if(t0 < 0)
                t0 = dTime;
            if(bUpdateMotors) {
                pLeftPlot->NewPoint(2, (dTime-t0)/1000.0, leftSpeed);
                pLeftPlot->NewPoint(1, (dTime-t0)/1000.0, LSpeed/100.0);
                pLeftPlot->UpdatePlot();
                pRightPlot->NewPoint(2, (dTime-t0)/1000.0, rightSpeed);
                pRightPlot->NewPoint(1, (dTime-t0)/1000.0, RSpeed/100.0);
                pRightPlot->UpdatePlot();
            }
        }
        else if(sHeader == "P") { // Buggy Asked the PID Parameters
            tokens.removeFirst();
            pPIDControlsDialog->sendParams();
        }
        else if(sHeader == "Buggy Ready") { // Buggy is Ready to Start
            pButtonConnect->setEnabled(true);
            tokens.clear();
        }
        else { // Unknown token
            tokens.removeFirst();
        }
    }
}


void
MainWindow::onTryToConnect() {
    if(serialConnect()) {
        connectionTimer.stop();
    }
}


void
MainWindow::onConnectPushed() {
    if(pButtonConnect->text() == QString("Connect")) {
        serialPort.write("K\n"); // Keep Alive message
        pPIDControlsDialog->sendParams();
        enableUI();
        keepAliveTimer.start(100);
        pButtonConnect->setText("Disconnect");
        pStatusBar->showMessage(QString("Buggy Connected !"));
    }
    else {
        disableUI();
        keepAliveTimer.stop();
        pButtonConnect->setText("Connect");
        pStatusBar->showMessage(QString("Buggy Disconnected !"));
        connectionTimer.start(500);
    }
}


void
MainWindow::onKeepAlive() {
    if(bConnected) {
        serialPort.write("K\n");
    }
    else {
        keepAliveTimer.stop();
        disableUI();
        pStatusBar->showMessage(QString("Buggy Disconnected !"));
        connectionTimer.start(500);
    }
}


void
MainWindow::onTimeToChangeSpeed() {
    if((LSpeed > 255) || (LSpeed < -255)) {
        iSign = -iSign;
    }
    LSpeed += iSign;
    RSpeed += iSign;
    QString sMessage = QString("Ls%1\nRs%2\n")
                       .arg(LSpeed)
                       .arg(RSpeed);
    serialPort.write(sMessage.toLatin1().constData());
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
        changeSpeedTimer.start(20);
        QString sMessage = QString("Ls%1\nRs%2\n")
                           .arg(LSpeed)
                           .arg(RSpeed);
        serialPort.write(sMessage.toLatin1().constData());
        pButtonStartStop->setText("Stop");
    }
    else {
        changeSpeedTimer.stop();
        serialPort.write("H\n");
        pButtonStartStop->setText("Start");
    }
}


void
MainWindow::onPIDControlsPushed() {
    pPIDControlsDialog->show();
    pButtonPIDControls->setDisabled(true);
}


void
MainWindow::onHidePIDControls() {
    pButtonPIDControls->setEnabled(true);
}


void
MainWindow::onNewDataAvailable() {
    bConnected = true;
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
MainWindow::onLPvalueChanged(int value) {
    LPvalue = value;
    QString sMessage = QString("Lp%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onLIvalueChanged(int value) {
    LIvalue = value;
    QString sMessage = QString("Li%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onLDvalueChanged(int value) {
    LDvalue = value;
    QString sMessage = QString("Ld%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onLSpeedChanged(int value) {
    LSpeed = value;
    QString sMessage = QString("Ls%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onRPvalueChanged(int value) {
    RPvalue = value;
    QString sMessage = QString("Rp%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onRIvalueChanged(int value) {
    RIvalue = value;
    QString sMessage = QString("Ri%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onRDvalueChanged(int value) {
    RDvalue = value;
    QString sMessage = QString("Rd%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


void
MainWindow::onRSpeedChanged(int value) {
    RSpeed = value;
    QString sMessage = QString("Rs%1\n").arg(int(value));
    serialPort.write(sMessage.toLatin1().constData());
}


