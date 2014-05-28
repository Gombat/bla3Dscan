#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "arduino_serial.h"
#include "cam_viewer.h"

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

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

    public slots:
        void connect_serialport( );
        void test_serialport( );
        void cam_test( );

    private slots:
        void check_cam_thread( );

    private:
        Ui::MainWindow *ui;
        Cam_viewer *m_cam_viewer;

        int m_serialport;

        //boost::shared_ptr< cv::VideoCapture > m_cam, m_cam1, m_cam2;
        //std::string m_cam1_device, m_cam2_device;
        //boost::shared_ptr< boost::thread > m_cam_thread;
        //boost::shared_ptr< boost::mutex > m_cam_mutex;
        //QTimer* m_qtimer;
    };
}

#endif // MAINWINDOW_H
