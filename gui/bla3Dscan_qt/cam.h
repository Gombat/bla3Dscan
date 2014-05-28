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
    };
}

#endif // CAM_H
