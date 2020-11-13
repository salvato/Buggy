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
    void show();

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
    void ControlsDone();

private slots:
    void onLPslider_valueChanged(int value);
    void onLIslider_valueChanged(int value);
    void onLDslider_valueChanged(int value);

    void onRPslider_valueChanged(int value);
    void onRIslider_valueChanged(int value);
    void onRDslider_valueChanged(int value);

    void changeSpeed();

private:
    Ui::ControlsDialog*  pUi;
};

