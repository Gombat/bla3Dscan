#ifndef CAM_DISPLAY_H
#define CAM_DISPLAY_H

#include "cam_scene.h"

#include "calibrationdialog.h"

#include <QGraphicsView>
#include <QTimer>
#include <QComboBox>
#include <QMessageBox>
#include <QSpinBox>
#include <QPushButton>
#include <QGraphicsPixmapItem>
#include <QShowEvent>

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>


class Cam_viewer : public QGraphicsView
{
    Q_OBJECT

    struct chessboard_data
    {
        chessboard_data( )
        {
            this->found = false;
        }

        chessboard_data( const std::vector< cv::Point2f >& corners, bool found )
        {
            this->corners = corners;
            this->found   = found;
        }

        std::vector< cv::Point2f > corners;
        bool found;
    };

public:
    explicit Cam_viewer(QWidget *parent = 0);

public:
    void initialize_gui( );

    bool cam_open( int cam_idx );
    cv::Mat cam_snapshot( int cam_idx );

signals:

public slots:

    void change_fps( int fps );
    bool cam_show( );
    void cam_close( );
    void cam_calibration_dialog( );
    void cam_calibration( );
    void cam_calibration_collect( );
    void cam_calibration_save( );

    void cam_update_frame( );

private:
    QWidget* m_parent;

    Cam_scene *m_cam_scene;
    cv::Mat m_frame_src, m_frame_out;
    QGraphicsPixmapItem *m_pixmap_item;

    boost::shared_ptr< cv::VideoCapture > m_cam;
    int m_cam_current;

    QTimer *m_timer;
    bool m_calibrating;
    chessboard_data m_chessboard_data;
    std::vector< std::vector< cv::Point2f > > m_calibration_data;

    QSpinBox *m_cam_spinbox;
    QSpinBox *m_cam_fps_spinbox;

    QPushButton* m_cam_show_pushbutton;
    QPushButton* m_cam_close_pushbutton;
    QPushButton* m_calibrate_pushbutton;
    QPushButton* m_collect_pushbutton;
    QPushButton* m_save_calib_pushbutton;

    bla3Dscan::CalibrationDialog* m_calib_dialog;
    float m_square_size;
    int m_num_squares_x;
    int m_num_squares_y;
};

#endif // CAM_DISPLAY_H
