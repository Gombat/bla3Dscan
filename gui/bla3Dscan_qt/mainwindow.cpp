#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cam.h"
#include "utils.h"

#include <QMessageBox>

namespace bla3Dscan {

    MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        m_serialport( -1 )
    {
        ui->setupUi( this );
    }

    MainWindow::~MainWindow( )
    {
        serialport_close( m_serialport );

        delete ui;
    }

    void MainWindow::showEvent( QShowEvent *event )
    {
        ui->graphicsViewCam->initialize_cams( );
        event->accept( );
    }

    void MainWindow::connect_serialport( )
    {
        if ( m_serialport == -1 )
        {
            m_serialport = serialport_init(
                ui->lineEditSerialport->text( ).toStdString( ).c_str( ),
                atoi( ui->comboBoxBaud->currentText( ).toStdString( ).c_str( ) ) );
            if ( m_serialport != -1 )
            {
                ui->pushButtonConnectSerialport->setText( "disconnect" );

                ui->textBrowserSerialport->setText(
                    ui->textBrowserSerialport->toPlainText() + "connected\n");
            }
        }
        else
        {
            serialport_close( m_serialport );
            m_serialport = -1;
            ui->pushButtonConnectSerialport->setText( "connect" );
            ui->textBrowserSerialport->setText(
                ui->textBrowserSerialport->toPlainText( ) + "disconnected\n");
        }
        //std::cout << "serialport = " << serialport << std::endl;
    }

    void MainWindow::test_serialport( )
    {
        int ret = serialport_writebyte( m_serialport, 2 );
        if ( ret != -1 )
        {
            ui->textBrowserSerialport->setText(
                ui->textBrowserSerialport->toPlainText( ) + "test successful\n" );
        }
        else
        {
            ui->textBrowserSerialport->setText(
                ui->textBrowserSerialport->toPlainText( ) + "test failed\n") ;
        }

    }

    void MainWindow::start_scan( )
    {
        m_scan_thread.reset( new boost::thread( &MainWindow::worker_scan, this) );
    }

    void MainWindow::worker_scan( )
    {
        if ( m_serialport == -1 )
        { connect_serialport( ); }
        if ( m_serialport == -1 )
        {
            QMessageBox::critical( this, "Connection", "No serial connection to the stepper! Check your Stepper settings" );
            return;
        }

        unsigned int scan_precision =
                Utils::translate_scan_precision( ui->comboBoxScanPrecision->currentText( ) );
        if ( static_cast< int >( scan_precision ) == -1 )
        {
            QMessageBox::critical( this, "Scan Precision", "Unknown Scan Precision!" );
            return;
        }

        uint8_t steps = static_cast< uint8_t >(
            200.0f / static_cast< float >( scan_precision ) );

        ui->tabWidget->setCurrentWidget( ui->tabCam );
        //cv::namedWindow( "frame_cam1" );
        //cv::namedWindow( "frame_cam2" );
        for ( unsigned int i = 0; i < scan_precision; i++ )
        {
            int ret = serialport_writebyte( m_serialport, steps );
            if ( ret == -1 )
            {
                QMessageBox::critical( this, "Stepper",
                    QString( "Could not send a step command to the stepper! (%1 steps)" ).arg( steps ) );
                return;
            }
            usleep( 1000 * ui->spinBoxScanPrecisionIdleTime->value( ) );

            ui->graphicsViewCam->cam_open( ui->spinBoxScanCam1->value( ) );
            ui->graphicsViewCam->cam_update_frame( );

            usleep( 1000 * ui->spinBoxScanPrecisionIdleTime->value( ) );

            ui->graphicsViewCam->cam_open( ui->spinBoxScanCam2->value( ) );
            ui->graphicsViewCam->cam_update_frame( );

            usleep( 1000 * ui->spinBoxScanPrecisionIdleTime->value( ) );

            //cv::Mat frame_cam1 = ui->graphicsViewCam->cam_snapshot( ui->spinBoxScanCam1->value( ) );
            //cv::imshow( "frame_cam1", frame_cam1 );

            //cv::Mat frame_cam2 = ui->graphicsViewCam->cam_snapshot( ui->spinBoxScanCam2->value( ) );
            //cv::imshow( "frame_cam2", frame_cam2 );
        }

        ui->tabWidget->setCurrentWidget( ui->tabModel );
    }

}
