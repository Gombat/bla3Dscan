#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QShowEvent>
#include <QCloseEvent>

#include "cam_viewer.h"
#include "reconstruct.h"

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

namespace Ui {
    class MainWindow;
}

namespace bla3Dscan
{

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit MainWindow( QWidget *parent = 0) ;
        ~MainWindow();

    protected:
        virtual void showEvent( QShowEvent *event );
        virtual void closeEvent( QCloseEvent *event );

    public slots:
        void connect_serialport( );
        void test_serialport( );
        void start_scan( );
        void end_scan( );

    private:
        Ui::MainWindow *ui;

        //Cam_viewer *m_cam_viewer;

        boost::asio::io_service m_io;
        boost::asio::serial_port m_port;

        Reconstruct m_reconstruct;

    };
}

#endif // MAINWINDOW_H
