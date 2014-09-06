#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QShowEvent>

#include "cam_viewer.h"

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

namespace Ui {
    class MainWindow;
}

namespace bla3Dscan {

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow( QWidget *parent = 0) ;
        ~MainWindow();

        void worker_scan( );

    protected:
        virtual void showEvent( QShowEvent *event );

    public slots:
        void connect_serialport( );
        void test_serialport( );
        void start_scan( );

    private:
        Ui::MainWindow *ui;

        //Cam_viewer *m_cam_viewer;

        boost::asio::io_service m_io;
        boost::asio::serial_port m_port;

        boost::shared_ptr< boost::thread > m_scan_thread;
    };
}

#endif // MAINWINDOW_H
