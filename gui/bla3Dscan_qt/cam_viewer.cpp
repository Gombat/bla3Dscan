#include "cam_viewer.h"

#include "ui_calibrationdialog.h"

#include "cam.h"

Cam_viewer::Cam_viewer( QWidget *parent ) :
    QGraphicsView( parent ),
    m_parent( parent ),
    m_calibrating( false )
{
    m_cam_spinbox = NULL;
    m_cam_fps_spinbox = NULL;

    m_cam_scene = new Cam_scene( this );
    this->setScene( m_cam_scene );
    m_pixmap_item = new QGraphicsPixmapItem( );
    m_cam_scene->addItem( m_pixmap_item );

    m_timer = new QTimer(this);
    connect( m_timer, SIGNAL( timeout( ) ), this, SLOT( cam_update_frame( ) ) );

    m_calib_dialog = new bla3Dscan::CalibrationDialog( this );
    connect( m_calib_dialog, SIGNAL( accepted( ) ), this, SLOT( cam_calibration( ) ) );
}

void Cam_viewer::initialize_cams( )
{
    QObjectList children = m_parent->children( );
    for ( int i = 0; i < children.size( ); i++ )
    {
        if ( children.at(i)->objectName().compare( "spinBoxCam" , Qt::CaseInsensitive ) == 0 )
        { m_cam_spinbox = qobject_cast< QSpinBox* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "spinBoxCamFPS" , Qt::CaseInsensitive ) == 0 )
        { m_cam_fps_spinbox = qobject_cast< QSpinBox* >( children.at(i) ); }
    }
    assert( m_cam_spinbox );
    assert( m_cam_fps_spinbox );
}

void Cam_viewer::cam_open( int cam_idx )
{
    if ( m_cam && m_cam->isOpened( ) )
    {
        if ( m_cam_current == cam_idx )
            return;
        else
            m_cam->release( );
    }

    m_cam.reset( new cv::VideoCapture( cam_idx ) );
    if ( !m_cam->isOpened( ) )
    {
        QMessageBox::warning( this, "Camera", QString( "Could not open cam %1 !" ).arg( cam_idx ) );
        return;
    }

    m_cam_current = cam_idx;
}

cv::Mat Cam_viewer::cam_snapshot( int cam_idx )
{
    cam_open( cam_idx );

    if ( !m_cam )
    {
        QMessageBox::critical( this, "Camera",
            QString("Could not collect an source image from camera %1" ).arg( cam_idx ) );
        return cv::Mat( );
    }
    (*m_cam) >> m_frame_src;
    cv::cvtColor( m_frame_src, m_frame_out, CV_BGR2RGB );
    return m_frame_out;
}

void Cam_viewer::change_fps( int fps )
{
    if ( m_timer->isActive( ) )
    {
        m_timer->stop( );
        float ms = std::numeric_limits< float >::max( );
        if ( fps > 0 ){ ms = static_cast< int >( 1000.0f / static_cast< float >( fps ) ); }
        m_timer->start( ms );
    }
}

void Cam_viewer::cam_show( )
{
    int cam_idx( m_cam_spinbox->value( ) );
    cam_open( cam_idx );

    m_timer->start( 25 );
    change_fps( m_cam_fps_spinbox->value( ) );
}

void Cam_viewer::cam_close( )
{
    m_timer->stop( );
    m_cam->release( );
    m_cam.reset( );
}

void Cam_viewer::cam_calibration_dialog( )
{
    m_calib_dialog->show( );
}

void Cam_viewer::cam_calibration( )
{
    m_square_size = m_calib_dialog->square_size( );
    m_num_squares_x = m_calib_dialog->num_squares_x( );
    m_num_squares_y = m_calib_dialog->num_squares_y( );

    m_calibrating = true;
    m_chessboard_data.found = false;
    m_calibration_data.clear( );

    cam_show( );

    //cam_update_frame( );
    //bool found = cv::findChessboardCorners( m_frame_src,  )
}

void Cam_viewer::cam_calibration_collect( )
{
    if ( m_calibrating && m_chessboard_data.found )
    {
        cv::Mat frame_gray;
        cv::cvtColor( m_frame_out, frame_gray, CV_RGB2GRAY );
        cornerSubPix( frame_gray, m_chessboard_data.corners, cv::Size( m_num_squares_x, m_num_squares_y ),
          cv::Size( -1,-1 ), cv::TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1 ) );

        m_calibration_data.push_back( m_chessboard_data.corners );
        m_chessboard_data.found = false;
    }
}

void Cam_viewer::cam_calibration_save( )
{
    if ( m_calibrating )
    {
        if ( m_calibration_data.size( ) <= 4 )
        {
            QMessageBox::warning( this, "Calibration Data", "Collect at least 4 samples!" );
            return;
        }

        cv::Mat camera_matrix, dist_coeffs;
        std::vector< cv::Mat > rvecs, tvecs;
        std::vector< float > reproj_errs;
        double total_avg_err( 0.0 );

        bool ok = bla3Dscan::Cam::run_calibration( m_frame_out.size( ),
            camera_matrix, dist_coeffs,
            cv::Size( m_num_squares_x, m_num_squares_y ), m_square_size,
            m_calibration_data, rvecs, tvecs,
            reproj_errs, total_avg_err );

        if ( ok )
            QMessageBox::information( this, "Calibration",
                QString( "Calibration succeeded. avg reprojection error = %1" ).arg( total_avg_err ) );
        else
        {
            QMessageBox::critical( this, "Calibration",
                QString( "Calibration failed. avg reprojection error = %1" ).arg( total_avg_err ) );
        }

        if( ok )   // save only if the calibration was done with success
        {
            bla3Dscan::Cam::save_camera_params( m_frame_out.size( ),
                camera_matrix, dist_coeffs, rvecs, tvecs, reproj_errs,
                m_calibration_data, total_avg_err);
        }

        m_calibrating = false;
    }
}

void Cam_viewer::cam_update_frame( )
{
    if ( !m_cam ){ m_timer->stop( ); return; }
    (*m_cam) >> m_frame_src;
    cv::cvtColor( m_frame_src, m_frame_out, CV_BGR2RGB );
    //cv::cvtColor(frame, edges, CV_BGR2GRAY);

    if ( m_calibrating )
    {
        std::vector< cv::Point2f > corners;
        bool found = cv::findChessboardCorners( m_frame_out,
            cv::Size( m_num_squares_x, m_num_squares_y ), corners );

        cv::drawChessboardCorners( m_frame_out, cv::Size( m_num_squares_x, m_num_squares_y ),
            corners, found);

        if ( found )
        { m_chessboard_data = chessboard_data( corners, true ); }
    }

    m_pixmap_item->setPixmap( QPixmap::fromImage( QImage( (unsigned char*) m_frame_out.data, m_frame_out.cols, m_frame_out.rows, QImage::Format_RGB888 ) ) );

}
