#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "cam.h"

#include <QMessageBox>

namespace bla3Dscan {

    MainWindow::MainWindow( QWidget *parent ) :
        QMainWindow( parent ),
        ui( new Ui::MainWindow ),
        m_serialport( -1 )
    {
        ui->setupUi(this);

        //m_qtimer = new QTimer( this );
        //m_qtimer->start( 100 );
        //connect( m_qtimer, SIGNAL( timeout( ) ), this, SLOT( check_cam_thread( ) ) );
    }

    MainWindow::~MainWindow( )
    {
        serialport_close( m_serialport );

        /*
        if ( m_cam_mutex ){
            boost::lock_guard< boost::mutex > lock( *m_cam_mutex );
            m_cam.reset( );
            if ( m_cam_thread ){ m_cam_thread->join( ); }
        }
        */

        delete ui;
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
        int ret = serialport_writebyte( m_serialport, 1 );
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

    void MainWindow::cam_test( )
    {
        // check whether the cam is already running
        //if ( m_cam_thread && !m_cam_thread->timed_join( boost::posix_time::milliseconds( 0 ) ) )
        //{
        //    QMessageBox::information( this, "Conflict", QString( "A cam is already running! Close it first." ) );
        //    return;
        //}

        /*
        if ( m_cam && m_cam->isOpened( ) )
        { m_cam->release( ); }

        std::string name( ui->comboBoxCamTest->currentText( ).toStdString( ) );
        m_cam.reset( new cv::VideoCapture( atoi( name.c_str( ) ) ) );
        if ( !m_cam->isOpened( ) )
        {
            QMessageBox::warning( this, name.c_str( ), QString( "Could not open cam %s !" ).arg( name.c_str( ) ) );
            return;
        }
        */

        // change the capture image format
        //cam->set( CV_CAP_PROP_FOURCC, CV_FOURCC('M','J','P','G') );
        //int ex = static_cast< int >( cam->get( CV_CAP_PROP_FOURCC ) );
        //char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
        //std::cout << "codec: " << std::string(EXT) << std::endl;
        //cam->set( CV_CAP_PROP_FPS, 15 );

        // create a video capture thread
        //m_cam_mutex.reset( new boost::mutex( ) );
        //m_cam_thread.reset( new boost::thread( boost::bind( &Cam::test_cam, m_cam, name, m_cam_mutex ) ) );

        //m_cam_viewer->cam_show( );
    }

    void MainWindow::check_cam_thread( )
    {
        /*
        if ( m_cam && m_cam_thread && m_cam_thread->timed_join( boost::posix_time::milliseconds( 0 ) ) )
        {
            m_cam.reset( );
            m_cam_thread.reset( );
            if ( m_cam_mutex ){ m_cam_mutex.reset( ); }
        }
        */
    }
}
