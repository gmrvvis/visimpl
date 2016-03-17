/*
 * @file	FocusFrame.h
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es> 
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *					Do not distribute without further notice.
 */

#ifndef FOCUSFRAME_H_
#define FOCUSFRAME_H_

#include "Histogram.h"

class FocusFrame : public QFrame
{

  Q_OBJECT;

public:

  FocusFrame( QWidget* parent = 0 );

  void viewRegion( const visimpl::Histogram& histogram,
                   float marker,
                   float regionWidth = 0.1f);

  virtual void paintEvent(QPaintEvent* event);

protected:

  QPolygonF _curveLocal;
  QPolygonF _curveGlobal;
  float _visibleStart;
  float _width;

  unsigned int _firstPointLocal;
  unsigned int _lastPointLocal;

  unsigned int _firstPointGlobal;
  unsigned int _lastPointGlobal;


};



#endif /* FOCUSFRAME_H_ */
