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
}


void
ControlsDialog::sendParams() {
    // Send PID Parameters to the Motor Controller
    emit LPvalueChanged(pUi->LPslider->value());
    emit LIvalueChanged(pUi->LIslider->value());
    emit LDvalueChanged(pUi->LDslider->value());
    emit RPvalueChanged(pUi->RPslider->value());
    emit RIvalueChanged(pUi->RIslider->value());
    emit RDvalueChanged(pUi->RDslider->value());
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
    emit ControlsDone();
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
    emit LPvalueChanged(value);
}


void
ControlsDialog::on_LIslider_valueChanged(int value) {
    pUi->LIedit->setText(QString("%1").arg(value));
    emit LIvalueChanged(value);
}


void
ControlsDialog::on_LDslider_valueChanged(int value) {
    pUi->LDedit->setText(QString("%1").arg(value));
    emit LDvalueChanged(value);
}


void
ControlsDialog::on_RPslider_valueChanged(int value) {
    pUi->RPedit->setText(QString("%1").arg(value));
    emit RPvalueChanged(value);
}


void
ControlsDialog::on_RIslider_valueChanged(int value) {
    pUi->RIedit->setText(QString("%1").arg(value));
    emit RIvalueChanged(value);
}


void
ControlsDialog::on_RDslider_valueChanged(int value) {
    pUi->RDedit->setText(QString("%1").arg(value));
    emit RDvalueChanged(value);
}


void
ControlsDialog::changeSpeed(double speed) {
    emit LSpeedChanged(speed);
//    emit RSpeedChanged(currentRspeed);
}

