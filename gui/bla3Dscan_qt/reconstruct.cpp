#include "reconstruct.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include "cam.h"

#include <QMessageBox>
#include <opencv2/nonfree/features2d.hpp>


//#include <boost/bind.hpp>

namespace bla3Dscan
{
    Reconstruct::Reconstruct( QWidget* parent ) :
        m_parent( parent ),
        m_abort_scan( false ),
        m_min_kp_distance( 0.5f )
    {
    }

    void Reconstruct::shutdown( )
    {
        m_abort_scan = true;
    }
    
    void Reconstruct::wait_idle_time( boost::timer& timer, int idle_time )
    {
        timer.restart( );
        while( boost::posix_time::seconds( timer.elapsed( ) ) < boost::posix_time::milliseconds( idle_time ) && !m_abort_scan )
        {
            boost::this_thread::sleep( boost::posix_time::milliseconds( 1 ) );
            QApplication::processEvents( );
        }
    }
    
    void Reconstruct::remove_identical_keypoints( std::vector< std::vector< cv::KeyPoint > >& vec_keypoints )
    {
        for ( unsigned int img1 = 0; img1 < vec_keypoints.size( ); img1++ )
        {
            for ( unsigned int kp1 = 0; kp1 < vec_keypoints[ img1 ].size( ); kp1++ )
            {
                bool bErase = false;
                const cv::KeyPoint& keypoint1 = vec_keypoints[ img1 ][ kp1 ];
                
                for ( unsigned int img2 = img1+1; img2 < vec_keypoints.size( ); img2++ )
                {
                    for ( unsigned int kp2 = 0; kp2 < vec_keypoints[ img2 ].size( ); kp2++ )
                    {
                        const cv::KeyPoint& keypoint2 = vec_keypoints[ img2 ][ kp2 ];
                        if ( cv::norm( keypoint1.pt - keypoint2.pt ) < m_min_kp_distance )
                        {
                            vec_keypoints[ img2 ].erase( vec_keypoints[ img2 ].begin( ) + kp2-- );
                        }
                    }
                }
                
                if ( bErase )
                {
                    vec_keypoints[ img1 ].erase( vec_keypoints[ img1 ].begin( ) + kp1-- );
                }
            }
        }
    }

    bool Reconstruct::scan( boost::asio::serial_port& port, Ui::MainWindow *ui )
    {
    //    m_scan_thread.reset( new boost::thread( &Reconstruct::worker_scan, this) );
    //}
    //
    //void Reconstruct::worker_scan( void )
    //{
        unsigned int scan_precision =
            Utils::translate_scan_precision( ui->comboBoxScanPrecision->currentText( ) );
        if ( static_cast< int >( scan_precision ) == -1 )
        {
            QMessageBox::critical( m_parent, "Scan Precision", "Unknown Scan Precision!" );
            return false;
        }

        int idle_time = ui->spinBoxScanPrecisionIdleTime->value( );

        int cam1_id = ui->spinBoxScanCam1->value( );
        int cam2_id = ui->spinBoxScanCam2->value( );

        std::vector< int8_t > steps( 1 );
        steps[ 0 ] = static_cast< int8_t >( 200.0f / static_cast< float >( scan_precision ) );
        //int8_t steps = static_cast< int8_t >( 200.0f / static_cast< float >( scan_precision ) );

        //cv::namedWindow( "frame_cam1" );
        //cv::namedWindow( "frame_cam2" );

        boost::timer timer;
        m_vec_cam1_images.clear( );
        m_vec_cam2_images.clear( );

        for ( unsigned int i = 0; i < scan_precision && !m_abort_scan; i++ )
        {
            //int ret = serialport_writebyte( m_serialport, steps );
            int ret = boost::asio::write( port, boost::asio::buffer(steps) );
            if ( ret == -1 )
            {
                QMessageBox::critical( m_parent, "Stepper",
                    QString( "Could not send a step command to the stepper! (%1 steps)" ).arg( steps[0] ) );
                return false;
            }

            uint8_t byte_read;
            boost::asio::read( port, boost::asio::buffer( &byte_read, 1 ) );
            std::cout << "arduino says: " << static_cast<int>(byte_read);

            wait_idle_time( timer, idle_time );

            ui->graphicsViewCam->cam_open( cam1_id );
            ui->graphicsViewCam->cam_update_frame( m_vec_cam1_images );
            QApplication::processEvents( );

            wait_idle_time( timer, idle_time );

            ui->graphicsViewCam->cam_open( cam2_id );
            ui->graphicsViewCam->cam_update_frame( m_vec_cam2_images );
            QApplication::processEvents( );

            wait_idle_time( timer, idle_time );

            //cv::Mat frame_cam1 = ui->graphicsViewCam->cam_snapshot( ui->spinBoxScanCam1->value( ) );
            //cv::imshow( "frame_cam1", frame_cam1 );

            //cv::Mat frame_cam2 = ui->graphicsViewCam->cam_snapshot( ui->spinBoxScanCam2->value( ) );
            //cv::imshow( "frame_cam2", frame_cam2 );
        }

        if ( !reconstruct( ) )
        {
            m_vec_cam1_images.clear( );
            m_vec_cam2_images.clear( );
            return false;
        }


        m_vec_cam1_images.clear( );
        m_vec_cam2_images.clear( );
        return true;
    }

    bool Reconstruct::reconstruct( )
    {
        cv::Mat camera_matrix, dist_coeffs;
        if ( !Cam::load_camera_params( camera_matrix, dist_coeffs, "camera_matrix.xml", "dist_coeffs.xml" ) )
            return false;

        cv::SIFT sift_detector;
        std::vector< std::vector< cv::KeyPoint > > vec_cam1_keypoints, vec_cam2_keypoints;
        sift_detector.detect( m_vec_cam1_images, vec_cam1_keypoints );
        sift_detector.detect( m_vec_cam2_images, vec_cam2_keypoints );

        // preprocess the detected points and remove the ones which are at the same location
        remove_identical_keypoints( vec_cam1_keypoints );
        remove_identical_keypoints( vec_cam2_keypoints );

        // camera 1 triangulation
        std::vector< cv::Mat > vec_cam1_points4D( vec_cam1_keypoints.size( ) - 1 );
        for ( unsigned int i = 0; i < vec_cam1_keypoints.size( ) - 1; i++ )
        {
            cv::triangulatePoints( camera_matrix, camera_matrix,
                vec_cam1_keypoints[ i ], vec_cam1_keypoints[ i+1 ], vec_cam1_points4D[ i ] );
        }
        
        // camera 2 triangulation
        std::vector< cv::Mat > vec_cam2_points4D( vec_cam2_keypoints.size( ) - 1 );
        for ( unsigned int i = 0; i < vec_cam2_keypoints.size( ) - 1; i++ )
        {
            cv::triangulatePoints( camera_matrix, camera_matrix,
                vec_cam2_keypoints[ i ], vec_cam2_keypoints[ i+1 ], vec_cam2_points4D[ i ] );
        }

        return true;
    }
    
}

