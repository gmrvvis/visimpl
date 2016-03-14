/*
 * Histogram.cpp
 *
 *  Created on: 11 de mar. de 2016
 *      Author: sgalindo
 */

#include "Histogram.h"

#include <QPainter>
#include <QBrush>

#include <QMouseEvent>


visimpl::Histogram::Histogram( const brion::Spikes& spikes,
                               float startTime,
                               float endTime )
: QFrame( nullptr )
, _maxValueHistogramLocal( 0 )
, _spikes( spikes )
, _startTime( startTime )
, _endTime( endTime )
, _scaleFunc( nullptr )
, _colorScale( visimpl::Histogram::T_COLOR_LOGARITHMIC )
, _normRule( visimpl::Histogram::T_NORM_MAX )
, _lastMousePosition( nullptr )
{
  colorScale( _colorScale );

  QPixmap pixmap(20, 20);
  QPainter painter(&pixmap);
  painter.fillRect(0, 0, 10, 10, Qt::lightGray);
  painter.fillRect(10, 10, 10, 10, Qt::lightGray);
  painter.fillRect(0, 10, 10, 10, Qt::darkGray);
  painter.fillRect(10, 0, 10, 10, Qt::darkGray);
  painter.end();
  QPalette pal = palette();
  pal.setBrush(backgroundRole(), QBrush(pixmap));
  setPalette(pal);
  setAutoFillBackground(true);

  setMouseTracking( true );
}

visimpl::Histogram::Histogram( const brion::SpikeReport& spikeReport )
: QFrame( nullptr )
, _maxValueHistogramLocal( 0 )
, _spikes( spikeReport.getSpikes( ))
, _startTime( spikeReport.getStartTime( ))
, _endTime( spikeReport.getEndTime( ))
, _scaleFunc( nullptr )
, _colorScale( visimpl::Histogram::T_COLOR_LOGARITHMIC )
, _normRule( visimpl::Histogram::T_NORM_MAX )
, _lastMousePosition( nullptr )
{
  colorScale( _colorScale );

  QPixmap pixmap(20, 20);
  QPainter painter(&pixmap);
  painter.fillRect(0, 0, 10, 10, Qt::lightGray);
  painter.fillRect(10, 10, 10, 10, Qt::lightGray);
  painter.fillRect(0, 10, 10, 10, Qt::darkGray);
  painter.fillRect(10, 0, 10, 10, Qt::darkGray);
  painter.end();
  QPalette pal = palette();
  pal.setBrush(backgroundRole(), QBrush(pixmap));
  setPalette(pal);
  setAutoFillBackground(true);

  setMouseTracking( true );
}

void visimpl::Histogram::CreateHistogram( unsigned int binsNumber )
{

//  _histogram.clear( );
  _histogram.resize( binsNumber, 0 );
  _maxValueHistogramLocal = 0;
  _maxValueHistogramGlobal = 0;

  std::vector< unsigned int > globalHistogram;
  globalHistogram.resize( binsNumber, 0 );

  float totalTime = _endTime - _startTime ;

  bool filter = _filteredGIDs.size( ) > 0;

  std::cout << "Filtered GIDs: " << _filteredGIDs.size( ) << std::endl;

  unsigned int counter = 0;
  float invTotal = 1.0f / totalTime;
  for( auto spike : _spikes )
  {
    float perc = ( spike.first - _startTime ) * invTotal;
    perc = std::min( 1.0f, std::max( 0.0f, perc ));
    unsigned int position =  _histogram.size( ) * perc;

    globalHistogram[ position ]++;

    if( filter && _filteredGIDs.find( spike.second ) == _filteredGIDs.end( ))
    {
      continue;
    }

    assert( position < _histogram.size( ));
    _histogram[ position ]++;
    counter++;
  }

  std::cout << "Total spikes: " << counter << std::endl;

  unsigned int cont = 0;
  unsigned int maxPos = 0;
  for( auto bin : _histogram )
  {
    if( bin > _maxValueHistogramLocal )
    {
      _maxValueHistogramLocal = bin;
      maxPos = cont;
    }
    cont++;
  }

  _maxValueHistogramGlobal = _maxValueHistogramLocal;

  if( filter )
  {
    for( auto bin : globalHistogram )
    {
      if( bin > _maxValueHistogramGlobal )
      {
        _maxValueHistogramGlobal = bin;
      }
    }
  }

  std::cout << "Bin with local maximum value " << _maxValueHistogramLocal
            << " at " << maxPos
            << std::endl;

  CalculateColors( );

}

namespace visimpl
{

  // All these functions consider a maxValue = 1.0f / <calculated_maxValue >
  float linearFunc( float value, float maxValue )
  {
    return value * maxValue;
  }

  float exponentialFunc( float value, float maxValue )
  {
    return ( logf( value) * maxValue );
  }

  float logarithmicFunc( float value, float maxValue )
  {
    return ( log10f( value) * maxValue );
  }

}

void visimpl::Histogram::CalculateColors( void )
{
  QGradientStops stops;

  float maxValue = 0.0f;
  float relativeTime = 0.0f;
  float delta = 1.0f / _histogram.size( );
  float percentage;

  maxValue = _normRule == T_NORM_GLOBAL ?
             _maxValueHistogramGlobal :
             _maxValueHistogramLocal;

  switch( _colorScale )
  {
    case visimpl::Histogram::T_COLOR_LINEAR:

      maxValue = 1.0f / maxValue;

      break;
    case visimpl::Histogram::T_COLOR_EXPONENTIAL:

      maxValue = 1.0f / logf( maxValue );

      break;
    case visimpl::Histogram::T_COLOR_LOGARITHMIC:

      maxValue = 1.0f / log10f( maxValue );

      break;
  }

  for( auto bin : _histogram )
  {
    percentage = _scaleFunc( float( bin ), maxValue );
    percentage = std::max< float >( std::min< float > (1.0f, percentage ), 0.0f);
//    std::cout << percentage << std::endl;
    glm::vec4 color = _colorMapper.GetValue( percentage );
    stops << qMakePair( relativeTime, QColor( color.r, color.g, color.b, color.a ));

    relativeTime += delta;
  }

  _gradientStops = stops;
}

void visimpl::Histogram::filteredGIDs( const GIDUSet& gids )
{
  if( gids.size( ) > 0 )
    _filteredGIDs = gids;
}

const GIDUSet& visimpl::Histogram::filteredGIDs( void )
{
  return _filteredGIDs;
}

void visimpl::Histogram::colorScale( visimpl::Histogram::TColorScale scale )
{
  _colorScale = scale;

  switch( _colorScale )
  {
    case visimpl::Histogram::T_COLOR_LINEAR:

      _scaleFunc = linearFunc;

      break;
    case visimpl::Histogram::T_COLOR_EXPONENTIAL:

      _scaleFunc = exponentialFunc;

      break;
    case visimpl::Histogram::T_COLOR_LOGARITHMIC:

      _scaleFunc = logarithmicFunc;

      break;
  }
}

visimpl::Histogram::TColorScale visimpl::Histogram::colorScale( void )
{
  return _colorScale;
}

void visimpl::Histogram::normalizeRule(
    visimpl::Histogram::TNormalize_Rule normRule )
{
  _normRule = normRule;
}

visimpl::Histogram::TNormalize_Rule visimpl::Histogram::normalizeRule( void )
{
  return _normRule;
}

const std::vector< unsigned int>& visimpl::Histogram::histogram( void )
{
  return _histogram;
}

const utils::InterpolationSet< glm::vec4 >&
visimpl::Histogram::colorMapper( void )
{
  return _colorMapper;
}

void visimpl::Histogram::colorMapper(
    const utils::InterpolationSet< glm::vec4 >& colors )
{
  _colorMapper = colors;
}

const QGradientStops& visimpl::Histogram::gradientStops( void )
{
  return _gradientStops;
}

unsigned int visimpl::Histogram::valueAt( float percentage )
{
  unsigned int position = percentage * _histogram.size( );

  if( position >= _histogram.size( ))
    position = _histogram.size( ) - 1;

  return _histogram[ position ];
}

void visimpl::Histogram::mouseMoveEvent( QMouseEvent* event_ )
{
  QFrame::mouseMoveEvent( event_ );

  QPoint position = event_->pos( );

//  position.setX( position.x( ) + x( ));
//  position.setY( position.y( ) + y( ));
//  QRect rect = geometry( );
  position = mapToGlobal( position );

  emit mousePositionChanged( position );
}

void visimpl::Histogram::mousePosition( QPoint* mousePosition_ )
{
  _lastMousePosition = mousePosition_;
}

void visimpl::Histogram::paintEvent(QPaintEvent* /*e*/)
{

//  std::cout << "Painting... " << this << std::endl;
  QPainter painter( this );
  QLinearGradient gradient( 0, 0, width( ), 0 );

  QRect area = rect();

  gradient.setStops( _gradientStops );
  QBrush brush( gradient );

  painter.fillRect( area, brush );

  unsigned int currentHeight = height( );

  QLine line( QPoint( 0, currentHeight), QPoint( width( ), currentHeight ));
  painter.drawLine( line );

  if( _lastMousePosition )
  {
    QPoint localPosition = mapFromGlobal( *_lastMousePosition );

    if( localPosition.x( ) > width( ))
      localPosition.setX( width( ));
    else if ( localPosition.x( ) < 0 )
      localPosition.setX( 0 );

    if( localPosition.y( ) > height( ))
      localPosition.setY( height( ));
    else if ( localPosition.y( ) < 0 )
      localPosition.setY( 0 );


    float percentage = float( localPosition.x( )) / float( width( ));
//    percentage = std::min( 1.0f, std::max( 0.0f, percentage));
    int positionX = localPosition.x( );
    int margin = 5;

    if( width( ) - positionX < 50 )
      margin = -50;

    QPen pen( QColor( 255, 255, 255 ));
    painter.setPen( pen );

    currentHeight = currentHeight / 2;
    QPoint position ( positionX + margin, currentHeight );

    painter.drawText( position, QString::number( valueAt( percentage )));

    QLine marker( QPoint( positionX, 0 ), QPoint( positionX, height( )));
    pen.setColor( QColor( 177, 50, 50 ));
    painter.setPen( pen );
    painter.drawLine( marker );
  }

}


