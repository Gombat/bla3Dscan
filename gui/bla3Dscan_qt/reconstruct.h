#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H

#include <QObject>
#include <QWidget>

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/timer.hpp>

namespace Ui {
    class MainWindow;
}

namespace bla3Dscan {

    class Reconstruct : public QObject
    {
        Q_OBJECT

    public:
        Reconstruct( ) : m_parent( NULL ) { }
        Reconstruct( QWidget* parent = NULL );
        virtual ~Reconstruct( ){ }

        void shutdown( );
        bool scan( boost::asio::serial_port& m_port, Ui::MainWindow *ui );

        void wait_idle_time( boost::timer& timer, int idle_time );

    private:
        //void worker_scan( );

        void remove_identical_keypoints( std::vector< std::vector< cv::KeyPoint > >& vec_keypoints );
        
        bool reconstruct( );

        QWidget* m_parent;
        bool m_abort_scan;
        //boost::shared_ptr< boost::thread > m_scan_thread;
        
        std::vector< cv::Mat > m_vec_cam1_images, m_vec_cam2_images;
        float m_min_kp_distance;
    };

} // namespace bla3Dscan

#endif // RECONSTRUCT_H
