/*
 * Copyright (c) 2015-2020 VG-Lab/URJC.
 *
 * Authors: Sergio E. Galindo <sergio.galindo@urjc.es>
 *
 * This file is part of ViSimpl <https://github.com/vg-lab/visimpl>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

// Qt
#include <QPainter>
#include <QBrush>

// C++
#include <iostream>

// Sumrice
#include "Gradient.h"

Gradient::Gradient(QWidget *parent_) :
    QFrame(parent_),
    _direction(VERTICAL)
{
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
}
    
void Gradient::redGradient()
{
    QGradientStops stops;
    stops << qMakePair(0.0, QColor(255, 0, 0))
          << qMakePair(1.0, QColor(0, 0, 0));
    setGradientStops(stops);
}

void Gradient::greenGradient()
{
    QGradientStops stops;
    stops << qMakePair(0.0, QColor(0, 255, 0))
          << qMakePair(1.0, QColor(0, 0, 0));
    setGradientStops(stops);
}

void Gradient::blueGradient()
{
    QGradientStops stops;
    stops << qMakePair(0.0, QColor(0, 0, 255))
          << qMakePair(1.0, QColor(0, 0, 0));
    setGradientStops(stops);
}

void Gradient::alphaGradient()
{
    QGradientStops stops;
    stops << qMakePair(1.0, QColor(0, 0, 0, 0))
          << qMakePair(0.0, QColor(0, 0, 0, 255));
    setGradientStops(stops);
}

void Gradient::plot( const QPolygonF& plot_ )
{
  _plot = plot_;
}

QPolygonF Gradient::plot( void )
{
  return _plot;
}

void Gradient::clearPlot( void )
{
  _plot.clear( );
}

float Gradient::xPos( float x_ )
{
  return x_ * width( );
}

float Gradient::yPos( float y_ )
{
  return (1.0f - y_) * height( );
}

void Gradient::paintEvent(QPaintEvent* /*e*/)
{
    QPainter painter(this);
    QLinearGradient gradient(0, 0, _direction == HORIZONTAL ? width() : 0,
                             _direction == VERTICAL ? height() : 0);
    gradient.setStops(_stops);
    QBrush brush(gradient); 
    painter.fillRect(rect(), brush);

    if( _plot.size( ) > 0)
    {
      painter.setBrush( QBrush( QColor( 0, 0, 0 )));
      auto prev = _plot.begin( );
      QPointF prevPoint( xPos( prev->x( )), yPos( prev->y( )));
      for( auto current = prev + 1; current != _plot.end( ); current++ )
      {
        QPointF point( xPos( current->x( )), yPos( current->y( ) ) );

        painter.drawLine( prevPoint, point );

        prevPoint = point;
      }
    }
}
