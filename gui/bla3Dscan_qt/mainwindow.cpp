#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cam.h"

#include <QMessageBox>

namespace bla3Dscan
{

    MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        m_port( m_io ),
        m_reconstruct( this )
    {
        ui->setupUi( this );
    }

    MainWindow::~MainWindow( )
    {
        delete ui;
    }

    void MainWindow::showEvent( QShowEvent *event )
    {
        ui->graphicsViewCam->initialize_gui( );
        event->accept( );
    }

    void MainWindow::closeEvent( QCloseEvent *event )
    {
        m_reconstruct.shutdown( );
    }

    void MainWindow::connect_serialport( )
    {
        if ( !m_port.is_open( ) )
        {
            try{
                m_port.open( ui->lineEditSerialport->text( ).toStdString( ).c_str( ) );
                if ( !m_port.is_open( ) )
                {
                    QMessageBox::critical(this, "Connection Error!", "Could not open target serialport!" );
                    return;
                }

                m_port.set_option( boost::asio::serial_port_base::baud_rate(
                    atoi( ui->comboBoxBaud->currentText( ).toStdString( ).c_str( ) ) ) );

                ui->pushButtonConnectSerialport->setText( "disconnect" );

                ui->textBrowserSerialport->setText(
                    ui->textBrowserSerialport->toPlainText() + "connected\n");
            }
            catch ( std::exception &e )
            {
                QMessageBox::critical(this, "Connection Error!", e.what( ) );
            }
        }
        else
        {
            m_port.close( );
            ui->pushButtonConnectSerialport->setText( "connect" );
            ui->textBrowserSerialport->setText(
                ui->textBrowserSerialport->toPlainText( ) + "disconnected\n");
        }
    }

    void MainWindow::test_serialport( )
    {
        int ret = boost::asio::write( m_port, boost::asio::buffer( "bla", strlen("bla") ) );
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
        ui->comboBoxScanPrecision->setEnabled( false );
        ui->spinBoxScanCam1->setEnabled( false );
        ui->spinBoxScanCam2->setEnabled( false );
        ui->spinBoxScanPrecisionIdleTime->setEnabled( false );

        if ( !m_port.is_open( ) )
        { connect_serialport( ); }
        if ( !m_port.is_open( ) )
        {
            QMessageBox::critical( this, "Connection", "No serial connection to the stepper! Check your Stepper settings" );
            end_scan( );
            return;
        }

        // TODO show progress bar pop up which is modal and if closed abort scan

        ui->tabWidget->setCurrentWidget( ui->tabCam );
        QApplication::processEvents( );

        m_reconstruct.scan( m_port, ui );

        end_scan( );

        ui->tabWidget->setCurrentWidget( ui->tabWidget );
    }

    void MainWindow::end_scan( )
    {
        ui->comboBoxScanPrecision->setEnabled( true );
        ui->spinBoxScanCam1->setEnabled( true );
        ui->spinBoxScanCam2->setEnabled( true );
        ui->spinBoxScanPrecisionIdleTime->setEnabled( true );
    }

}
