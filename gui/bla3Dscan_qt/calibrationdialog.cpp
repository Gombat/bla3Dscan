#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"

namespace bla3Dscan {


    CalibrationDialog::CalibrationDialog(QWidget *parent) :
        QDialog(parent),
        ui( new Ui::CalibrationDialog )
    {
        ui->setupUi( this );
    }


    float CalibrationDialog::square_size( )
    {
        return static_cast< float >( ui->doubleSpinBoxChessboardSquareSize->value( ) );
    }

    int CalibrationDialog::num_squares_x( )
    {
        return ui->spinBoxNumSquaresX->value( );
    }

    int CalibrationDialog::num_squares_y( )
    {
        return ui->spinBoxNumSquaresY->value( );
    }

}
