#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

namespace Ui {
    class CalibrationDialog;
}

namespace bla3Dscan {

    class CalibrationDialog : public QDialog
    {
        Q_OBJECT
    public:
        explicit CalibrationDialog(QWidget *parent = 0);

        float square_size( );
        int num_squares_x( );
        int num_squares_y( );

    signals:

    public slots:

    private:
        Ui::CalibrationDialog* ui;

    };
}

#endif // CALIBRATIONDIALOG_H
