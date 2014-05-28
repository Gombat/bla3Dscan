#ifndef CAM_DISPLAY_H
#define CAM_DISPLAY_H

#include "cam_scene.h"

#include <QGraphicsView>
#include <QTimer>
#include <QComboBox>
#include <QMessageBox>
#include <QSpinBox>
#include <QGraphicsPixmapItem>

#include <opencv2/opencv.hpp>

#include <boost/shared_ptr.hpp>

class Cam_viewer : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Cam_viewer(QWidget *parent = 0);

signals:

public slots:

    void cam_show( );
    void cam_close( );
    void cam_calibration( );
    void cam_calibration_collect( );
    void change_fps( int fps );

    void cam_update_frame( );

private:

    Cam_scene *m_cam_scene;
    cv::Mat m_frame_src, m_frame_out;
    QGraphicsPixmapItem *m_pixmap_item;

    boost::shared_ptr< cv::VideoCapture > m_cam;
    QString m_cam_current;

    QTimer *m_timer;

    QComboBox *m_cam_combobox;
    QSpinBox *m_cam_fps_spinbox;
};

#endif // CAM_DISPLAY_H
