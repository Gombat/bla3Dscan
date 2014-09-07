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

    m_cam_show_pushbutton = NULL;
    m_cam_close_pushbutton = NULL;
    m_calibrate_pushbutton = NULL;
    m_collect_pushbutton = NULL;
    m_save_calib_pushbutton = NULL;

    m_cam_scene = new Cam_scene( this );
    this->setScene( m_cam_scene );
    m_pixmap_item = new QGraphicsPixmapItem( );
    m_cam_scene->addItem( m_pixmap_item );

    m_timer = new QTimer(this);
    connect( m_timer, SIGNAL( timeout( ) ), this, SLOT( cam_update_frame( ) ) );

    m_calib_dialog = new bla3Dscan::CalibrationDialog( this );
    connect( m_calib_dialog, SIGNAL( accepted( ) ), this, SLOT( cam_calibration( ) ) );
}

void Cam_viewer::initialize_gui( )
{
    QObjectList children = m_parent->children( );
    for ( int i = 0; i < children.size( ); i++ )
    {
        if ( children.at(i)->objectName().compare( "spinBoxCam" , Qt::CaseInsensitive ) == 0 )
        { m_cam_spinbox = qobject_cast< QSpinBox* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "spinBoxCamFPS" , Qt::CaseInsensitive ) == 0 )
        { m_cam_fps_spinbox = qobject_cast< QSpinBox* >( children.at(i) ); }

        if ( children.at(i)->objectName().compare( "pushButtonCamShow" , Qt::CaseInsensitive ) == 0 )
        { m_cam_show_pushbutton = qobject_cast< QPushButton* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "pushButtonCamClose" , Qt::CaseInsensitive ) == 0 )
        { m_cam_close_pushbutton = qobject_cast< QPushButton* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "pushButtonCamCalibration" , Qt::CaseInsensitive ) == 0 )
        { m_calibrate_pushbutton = qobject_cast< QPushButton* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "pushButtonCamCalibrationCollect" , Qt::CaseInsensitive ) == 0 )
        { m_collect_pushbutton = qobject_cast< QPushButton* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "pushButtonCamCalibrationSave" , Qt::CaseInsensitive ) == 0 )
        { m_save_calib_pushbutton = qobject_cast< QPushButton* >( children.at(i) ); }
    }
    assert( m_cam_spinbox );
    assert( m_cam_fps_spinbox );
}

bool Cam_viewer::cam_open( int cam_idx )
{
    if ( m_cam && m_cam->isOpened( ) )
    {
        if ( m_cam_current == cam_idx )
            return true;
        else
            m_cam->release( );
    }

    m_cam.reset( new cv::VideoCapture( cam_idx ) );
    if ( !m_cam->isOpened( ) )
    {
        QMessageBox::warning( this, "Camera", QString( "Could not open cam %1 !" ).arg( cam_idx ) );
        return false;
    }

    m_cam_current = cam_idx;

    return true;
}

cv::Mat Cam_viewer::cam_snapshot( int cam_idx )
{
    if ( !cam_open( cam_idx ) || !m_cam )
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

bool Cam_viewer::cam_show( )
{
    int cam_idx( m_cam_spinbox->value( ) );
    if ( !cam_open( cam_idx ) )
        return false;

    m_timer->start( 25 );
    change_fps( m_cam_fps_spinbox->value( ) );

    return true;
}

void Cam_viewer::cam_close( )
{
    m_timer->stop( );
    m_cam->release( );
    m_cam.reset( );
}

void Cam_viewer::cam_calibration_dialog( )
{
    if ( m_calibrating )
    {
        // reset gui elements

        m_cam_spinbox->setEnabled( true );
        m_cam_fps_spinbox->setEnabled( true );

        m_cam_show_pushbutton->setEnabled( true );
        m_cam_close_pushbutton->setEnabled( true );
        m_calibrate_pushbutton->setText( "Calibrate" );
        m_collect_pushbutton->setEnabled( false );
        m_save_calib_pushbutton->setEnabled( false );

        // reset internals

        m_calibrating = false;
    }
    else
    {
        m_calib_dialog->show( );
    }
}

void Cam_viewer::cam_calibration( )
{
    // try to open the cam

    if ( !cam_show( ) )
        return;

    // deactivate / activate gui elements

    m_cam_spinbox->setEnabled( false );
    m_cam_fps_spinbox->setEnabled( false );

    m_cam_show_pushbutton->setEnabled( false );
    m_cam_close_pushbutton->setEnabled( false );
    m_calibrate_pushbutton->setText( "Abort Calib" );
    m_collect_pushbutton->setEnabled( true );
    m_save_calib_pushbutton->setEnabled( true );

    // prepare internals

    m_square_size = m_calib_dialog->square_size( );
    m_num_squares_x = m_calib_dialog->num_squares_x( );
    m_num_squares_y = m_calib_dialog->num_squares_y( );

    m_calibrating = true;
    m_chessboard_data.found = false;
    m_calibration_data.clear( );

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

        m_calibrating = false;

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
    }
}

void Cam_viewer::cam_update_frame( std::vector< cv::Mat >& vec_cam_images )
{
    cam_update_frame( );
    vec_cam_images.push_back( m_frame_out );
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
