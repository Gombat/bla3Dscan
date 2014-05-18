#ifndef CAM_H
#define CAM_H

#include <opencv2/opencv.hpp>

void test_cam( cv::VideoCapture* cam, const std::string& name )
{
    if ( !cam->isOpened() )
        return;

    cv::namedWindow(name.c_str(),1);
    cv::Mat frame;
    for(;;)
    {
        (*cam) >> frame;
        //cv::cvtColor(frame, edges, CV_BGR2GRAY);
        cv::imshow("cam1", frame);
        if(cv::waitKey(30) >= 0) break;
    }
    // the camera will be deinitialized automatically in VideoCapture destructor
}

#endif // CAM_H
