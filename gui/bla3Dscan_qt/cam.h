#ifndef CAM_H
#define CAM_H

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

namespace bla3Dscan {

    class Cam {

    private:
        Cam(){}
        ~Cam(){}

    public:

        static void test_cam(
            boost::shared_ptr< cv::VideoCapture > cam,
            const std::string& name,
            boost::shared_ptr< boost::mutex > mutex )
        {
            if ( !cam || !cam->isOpened( ) )
                return;

            cv::namedWindow( name.c_str( ) );
            cv::Mat frame;
            while( cvGetWindowHandle( name.c_str( ) ) )
            {
                boost::lock_guard< boost::mutex > lock( *mutex );
                if ( !cam ) break;
                (*cam) >> frame;
                //cv::cvtColor(frame, edges, CV_BGR2GRAY);
                cv::imshow( name, frame );
                //if(cv::waitKey(3) >= 0) break;
            }
            cv::destroyWindow( name.c_str( ) );
            // the camera will be deinitialized automatically in VideoCapture destructor
        }

        static bool run_calibration( const cv::Size image_size, cv::Mat& camera_matrix, cv::Mat& dist_coeffs,
            const cv::Size board_size, const float square_size,
            const std::vector< std::vector< cv::Point2f > >& corners,
            std::vector< cv::Mat >& rvecs, std::vector< cv::Mat >& tvecs,
            std::vector< float >& reproj_errs, double& totalAvgErr )
        {

            std::vector< std::vector< cv::Point3f > > object_points( corners.size( ) );
            for ( unsigned int i = 0; i < corners.size( ); i++ )
            {
                calc_board_corner_positions( board_size, square_size, object_points[ i ] );
                object_points.resize( corners.size(), object_points[ i ] );
            }

            camera_matrix = cv::Mat::eye(3, 3, CV_64F);
            //if( CV_CALIB_FIX_ASPECT_RATIO )
            //     cameraMatrix.at<double>(0,0) = 1.0;
            dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);

            double rms = cv::calibrateCamera( object_points, corners, image_size, camera_matrix,
                dist_coeffs, rvecs, tvecs );

            totalAvgErr = compute_reprojection_errors( object_points,
                corners, rvecs, tvecs, camera_matrix, dist_coeffs,
                reproj_errs );

            return true;
        }

        static void calc_board_corner_positions(const cv::Size board_size, const float square_size,
            std::vector< cv::Point3f >& object_points )
        {
            object_points.clear();

            for( int i = 0; i < board_size.height; ++i )
            {
                for( int j = 0; j < board_size.width; ++j )
                {
                    object_points.push_back( cv::Point3f(
                        static_cast< float >( static_cast< float >( j ) * square_size ),
                        static_cast< float >( static_cast< float >( i ) * square_size ), 0 ) );
                }
            }
        }

        static double compute_reprojection_errors(
            const std::vector< std::vector< cv::Point3f > >& object_points,
            const std::vector< std::vector< cv::Point2f > >& image_points,
            const std::vector< cv::Mat >& rvecs, const std::vector< cv::Mat >& tvecs,
            const cv::Mat& camera_matrix , const cv::Mat& dist_coeffs,
            std::vector< float >& per_view_errors)
        {
            std::vector< cv::Point2f > image_points_2;
            int i, total_points = 0;
            double total_err = 0, err;
            per_view_errors.resize( object_points.size( ) );

            for( i = 0; i < static_cast< int >( object_points.size( ) ); ++i )
            {
              cv::projectPoints( cv::Mat( object_points[ i ] ), rvecs[ i ], tvecs[ i ], camera_matrix,
                 dist_coeffs, image_points_2 ); // project
              err = cv::norm( cv::Mat( image_points[ i ] ), cv::Mat( image_points_2 ), CV_L2 ); // difference

              int n = static_cast< int >( object_points[ i ].size( ) );
              per_view_errors[ i ] = static_cast< float >( std::sqrt( err * err / n ) ); // save for this view
              total_err        += err * err; // sum it up
              total_points     += n;
            }

            return std::sqrt( total_err / total_points );              // calculate the arithmetical mean
        }

        static void save_camera_params( const cv::Size image_size,
            const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs,
            const std::vector< cv::Mat >& rvecs, const std::vector< cv::Mat >& tvecs,
            const std::vector< float >& reproj_errs,
            const std::vector< std::vector< cv::Point2f > >& corners,
            const double total_avg_err)
        {
            {
                cv::FileStorage fs( "camera_matrix.xml", cv::FileStorage::WRITE);
                if ( fs.isOpened( ) )
                {
                    fs << "camera_matrix" << camera_matrix;
                    fs.release();
                }
            }
            {
                cv::FileStorage fs( "dist_coeffs.xml", cv::FileStorage::WRITE);
                if ( fs.isOpened( ) )
                {
                    fs << "dist_coeffs" << dist_coeffs;
                    fs.release();
                }
            }


            /*
            //TO READ
            cv::FileStorage fs(filename,FileStorage::READ);
            fs["camera_matrix"] >> camera_matrix;
            fs.release();
            */
        }
    };
}

#endif // CAM_H
