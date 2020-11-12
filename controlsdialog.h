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
    void keyPressEvent(QKeyEvent *e);

signals:
    void LPvalueChanged(int Pvalue);
    void LIvalueChanged(int Ivalue);
    void LDvalueChanged(int Dvalue);
    void LSpeedChanged(int speed);

    void RPvalueChanged(int Pvalue);
    void RIvalueChanged(int Ivalue);
    void RDvalueChanged(int Dvalue);
    void RSpeedChanged(int speed);

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

