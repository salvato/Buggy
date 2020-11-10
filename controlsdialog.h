#pragma once

#include <QDialog>
#include <QObject>


namespace Ui {
class ControlsDialog;
}


class ControlsDialog : public QDialog
{
    Q_OBJECT

public:
    ControlsDialog();
    void closeEvent(QCloseEvent *event);

protected:
    void restoreSettings();
    void saveSettings();

signals:
    void LPvalueChanged(double Pvalue);
    void LIvalueChanged(double Ivalue);
    void LDvalueChanged(double Dvalue);
    void LSpeedChanged(double speed);

    void RPvalueChanged(double Pvalue);
    void RIvalueChanged(double Ivalue);
    void RDvalueChanged(double Dvalue);

    void RSpeedChanged(double speed);

private slots:
    void on_LPslider_valueChanged(int value);
    void on_LIslider_valueChanged(int value);
    void on_LDslider_valueChanged(int value);

    void on_RPslider_valueChanged(int value);
    void on_RIslider_valueChanged(int value);
    void on_RDslider_valueChanged(int value);

    void changeSpeed();

private:
    Ui::ControlsDialog*  pUi;
};

