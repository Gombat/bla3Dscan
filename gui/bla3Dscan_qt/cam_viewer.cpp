#include "cam_viewer.h"

Cam_viewer::Cam_viewer(QWidget *parent) :
    QGraphicsView(parent)
{
    m_cam_scene = new Cam_scene( this );
    this->setScene( m_cam_scene );
    m_pixmap_item = new QGraphicsPixmapItem( );

    m_timer = new QTimer(this);
    connect( m_timer, SIGNAL( timeout( ) ), this, SLOT( cam_update_frame( ) ) );

    m_cam_combobox = NULL;
    m_cam_fps_spinbox = NULL;
    QObjectList children = parent->children( );
    for ( int i = 0; i < children.size( ); i++ )
    {
        if ( children.at(i)->objectName().compare( "comboBoxCam" , Qt::CaseInsensitive ) == 0 )
        { m_cam_combobox = qobject_cast< QComboBox* >( children.at(i) ); }
        if ( children.at(i)->objectName().compare( "spinBoxCamFPS" , Qt::CaseInsensitive ) == 0 )
        { m_cam_fps_spinbox = qobject_cast< QSpinBox* >( children.at(i) ); }
    }
    assert( m_cam_combobox );
    assert( m_cam_fps_spinbox );
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
    QString name( m_cam_combobox->currentText( ) );
    if ( m_cam && m_cam->isOpened( ) )
    {
        if ( m_cam_current.compare(name) == 0 )
            return;
        else
            m_cam->release( );
    }

    m_cam.reset( new cv::VideoCapture( atoi( name.toStdString( ).c_str( ) ) ) );
    if ( !m_cam->isOpened( ) )
    {
        QMessageBox::warning( this, name, QString( "Could not open cam %1 !" ).arg( name ) );
        return;
    }

    m_cam_scene->addItem( m_pixmap_item );

    m_timer->start( 25 );
    change_fps( m_cam_fps_spinbox->value( ) );
}

void Cam_viewer::cam_close( )
{
    m_timer->stop( );
    m_cam->release( );
    m_cam.reset( );
}

void Cam_viewer::cam_calibration( )
{


}

void Cam_viewer::cam_calibration_collect( )
{

}

void Cam_viewer::cam_update_frame( )
{
    if ( !m_cam ){ m_timer->stop( ); return; }
    (*m_cam) >> m_frame_src;
    cv::cvtColor( m_frame_src, m_frame_out, CV_BGR2RGB );
    //cv::cvtColor(frame, edges, CV_BGR2GRAY);
    m_pixmap_item->setPixmap( QPixmap::fromImage( QImage( (unsigned char*) m_frame_out.data, m_frame_out.cols, m_frame_out.rows, QImage::Format_RGB888 ) ) );


}
