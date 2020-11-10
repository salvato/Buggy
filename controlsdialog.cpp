#include "controlsdialog.h"
#include "ui_controlsdialog.h"

#include <QSettings>
#include <QCloseEvent>


ControlsDialog::ControlsDialog()
    : pUi(new Ui::ControlsDialog)
{
    pUi->setupUi(this);

    pUi->LPslider->setRange(0, 700);
    pUi->LPslider->setTracking(true);
    pUi->LIslider->setTracking(true);
    pUi->LDslider->setTracking(true);

    pUi->RPslider->setRange(0, 700);
    pUi->RPslider->setTracking(true);
    pUi->RIslider->setTracking(true);
    pUi->RDslider->setTracking(true);

    restoreSettings();

    pUi->LPedit->setText(QString("%1").arg(pUi->LPslider->value()));
    pUi->LIedit->setText(QString("%1").arg(pUi->LIslider->value()));
    pUi->LDedit->setText(QString("%1").arg(pUi->LDslider->value()));

    pUi->RPedit->setText(QString("%1").arg(pUi->RPslider->value()));
    pUi->RIedit->setText(QString("%1").arg(pUi->RIslider->value()));
    pUi->RDedit->setText(QString("%1").arg(pUi->RDslider->value()));

    // Send PID Parameters to the Motor Controller
    emit LPvalueChanged(double(pUi->LPslider->value()*0.01));
    emit LIvalueChanged(double(pUi->LIslider->value()*0.01));
    emit LDvalueChanged(double(pUi->LDslider->value()*0.01));

    emit RPvalueChanged(double(pUi->RPslider->value()*0.01));
    emit RIvalueChanged(double(pUi->RIslider->value()*0.01));
    emit RDvalueChanged(double(pUi->RDslider->value()*0.01));
}


void
ControlsDialog::restoreSettings() {
    QSettings settings;
    restoreGeometry(settings.value(QString("ControlledMotor")).toByteArray());

    pUi->LPslider->setValue(settings.value(QString("LP_Value"), "0.0").toInt());
    pUi->LIslider->setValue(settings.value(QString("LI_Value"), "0.0").toInt());
    pUi->LDslider->setValue(settings.value(QString("LD_Value"), "0.0").toInt());

    pUi->RPslider->setValue(settings.value(QString("RP_Value"), "0.0").toInt());
    pUi->RIslider->setValue(settings.value(QString("RI_Value"), "0.0").toInt());
    pUi->RDslider->setValue(settings.value(QString("RD_Value"), "0.0").toInt());
}


void
ControlsDialog::saveSettings() {
    QSettings settings;
    settings.setValue(QString("ControlledMotor"), saveGeometry());
    settings.setValue(QString("LP_Value"), pUi->LPslider->value());
    settings.setValue(QString("LI_Value"), pUi->LIslider->value());
    settings.setValue(QString("LD_Value"), pUi->LDslider->value());
    settings.setValue(QString("RP_Value"), pUi->RPslider->value());
    settings.setValue(QString("RI_Value"), pUi->RIslider->value());
    settings.setValue(QString("RD_Value"), pUi->RDslider->value());
}


void
ControlsDialog::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)
    saveSettings();
    event->ignore();
}

void
ControlsDialog::keyPressEvent(QKeyEvent *e) {
    // To avoid closing the Plot upon Esc keypress
    if(e->key() != Qt::Key_Escape)
        QWidget::keyPressEvent(e);
}


void
ControlsDialog::on_LPslider_valueChanged(int value) {
    pUi->LPedit->setText(QString("%1").arg(value));
    emit LPvalueChanged(double(value*0.01));
}


void
ControlsDialog::on_LIslider_valueChanged(int value) {
    pUi->LIedit->setText(QString("%1").arg(value));
    emit LIvalueChanged(double(value*0.001));
}


void
ControlsDialog::on_LDslider_valueChanged(int value) {
    pUi->LDedit->setText(QString("%1").arg(value));
    emit LDvalueChanged(double(value*0.01));
}


void
ControlsDialog::on_RPslider_valueChanged(int value) {
    pUi->RPedit->setText(QString("%1").arg(value));
    emit RPvalueChanged(double(value*0.01));
}


void
ControlsDialog::on_RIslider_valueChanged(int value) {
    pUi->RIedit->setText(QString("%1").arg(value));
    emit RIvalueChanged(double(value*0.001));
}


void
ControlsDialog::on_RDslider_valueChanged(int value) {
    pUi->RDedit->setText(QString("%1").arg(value));
    emit RDvalueChanged(double(value*0.01));
}


void
ControlsDialog::changeSpeed() {
//    currentLspeed = 1.0 - currentLspeed;
//    currentRspeed = 1.0 - currentRspeed;
//    emit LSpeedChanged(currentLspeed);
//    emit RSpeedChanged(currentRspeed);
}

