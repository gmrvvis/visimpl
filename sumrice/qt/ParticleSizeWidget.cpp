/*
 * ParticleSizeWidget.cpp
 *
 *  Created on: 4 de mar. de 2016
 *      Author: sgalindo
 */

#include "ParticleSizeWidget.h"

#include <QGridLayout>

ParticleSizeWidget::ParticleSizeWidget( QWidget* parent_ )
: QWidget( parent_ )
{
  _result = new Gradient( );
  _result->setDirection( Gradient::HORIZONTAL );

  InitDialog( );

  connect( this, SIGNAL( clicked( void )),
           this, SLOT( gradientClicked( void )));

  QGridLayout* mainLayout = new QGridLayout( );
  mainLayout->addWidget( _result );

  this->setLayout( mainLayout );
}



void ParticleSizeWidget::InitDialog( void )
{
  _dialog = new QWidget( );
  _dialog->setWindowModality( Qt::ApplicationModal );
  _dialog->setMinimumSize( 800, 200 );

  _acceptButton = new QPushButton( "Accept" );
  _cancelButton = new QPushButton( "Cancel" );
  _previewButton = new QPushButton( "Preview" );

  _maxSizeBox = new QDoubleSpinBox( );
  _minSizeBox = new QDoubleSpinBox( );

  _minValueLabel = new QLabel( );
  _maxValueLabel = new QLabel( );

  _sizeFrame = new Gradient( );

  unsigned int row = 0;
  unsigned int totalColumns = 6;
  QGridLayout* dialogLayout = new QGridLayout( );

  dialogLayout->addWidget( new QLabel( "Size" ), row, 0, 1, 1 );
  row++;
  dialogLayout->addWidget( _sizeFrame, row, 0, 2, totalColumns - 1 );
  dialogLayout->addWidget( _maxSizeBox, row, totalColumns, 1, 1 );
  row++;
  dialogLayout->addWidget( _minSizeBox, row, totalColumns, 1, 1 );

  row++;
  dialogLayout->addWidget( _cancelButton, row, 1, 1, 1 );
  dialogLayout->addWidget( _previewButton, row, 3, 1, 1 );
  dialogLayout->addWidget( _acceptButton, row, 5, 1, 1 );

  _dialog->setLayout( dialogLayout );

  /* Setting gradient frames */
  QGradientStops stops;
  stops << qMakePair(0.0, QColor(127, 127, 127, 127))
        << qMakePair(1.0, QColor(127, 127, 127, 127));
  _sizeFrame->setDirection(Gradient::HORIZONTAL);
  _sizeFrame->setGradientStops(stops);

  QPolygonF points;
  points << QPointF(0, 0) << QPointF(1, 1);

  sizePoints = new ColorPoints(_sizeFrame);
  sizePoints->setPoints(points);

  /* Connecting slots */
  connect( _acceptButton, SIGNAL( clicked( void )),
           this, SLOT( acceptClicked( void )));
  connect( _cancelButton, SIGNAL( clicked( void )),
           this, SLOT( cancelClicked( void )));

  connect( _previewButton, SIGNAL( clicked( void )),
           this, SLOT( previewClicked( void) ));

  connect( sizePoints, SIGNAL(pointsChanged(const QPolygonF &)),
          this, SLOT(colorPointsChanged(const QPolygonF &)));
}

TSizeFunction ParticleSizeWidget::getSizeFunction( void )
{
  return _sizeFunction;
}


TSizeFunction ParticleSizeWidget::pointsToSizeFunc( const QPolygonF &points )
{

  TSizeFunction result;
  float time;
  float value;
//  float invHeight = 1.0f / float( height( ));
//  float invWidth = 1.0f / float( width( ));

  for( auto point : points)
  {
    time = point.x( );
    value = point.y( ) * ( _maxSize - _minSize) + _minSize;
    result.push_back( std::make_pair( time, value ));
  }

  return result;
}

void ParticleSizeWidget::setSizeFunction( const TSizeFunction& sizeFunc )
{

  _sizeFunction = sizeFunc;
  QPolygonF result;

  _minSize = std::numeric_limits< float >::max( );
  _maxSize = std::numeric_limits< float >::min( );
  for( auto value : sizeFunc )
  {
    if( value.second < _minSize )
      _minSize = value.second;

    if( value.second > _maxSize )
      _maxSize = value.second;
  }

  float invTotal = 1.0f / ( _maxSize - _minSize ) ;

//  auto valueIt = sizeFunc.values.begin( );
  for( auto time : sizeFunc )
  {
    result.append( QPointF( time.first, ( time.second - _minSize ) * invTotal ));

//    valueIt++;
  }

  sizePoints->setPoints( result, true );
  _minValueLabel->setText( QString::number( double( _minSize ) ));
  _maxValueLabel->setText( QString::number( double( _maxSize ) ));

  _minSizeBox->setValue( _minSize );
  _maxSizeBox->setValue( _maxSize );

  //TODO draw plot on gradient
  _sizeFrame->plot( result );
}

void ParticleSizeWidget::gradientClicked( void )
{
  if( _dialog && !_dialog->isVisible( ))
  {
//    setColorPoints( getColors( ), false);

    _dialog->show( );
  }
}

void ParticleSizeWidget::acceptClicked( void )
{
  _dialog->close( );

//  _result->setGradientStops( nTGradientFrame->getGradientStops( ));
//  _tResult = gradientFrame->getGradientStops( );
  emit sizeChanged( );
}

void ParticleSizeWidget::cancelClicked( void )
{
  _dialog->close( );

  if( previewed )
    emit sizeChanged( );
}

void ParticleSizeWidget::previewClicked( void )
{
  previewed = true;
  emit sizePreview( );
}

void ParticleSizeWidget::mousePressEvent(QMouseEvent * event_ )
{
  if( event_->button( ) == Qt::LeftButton )
  {
    emit gradientClicked( );
  }
}
