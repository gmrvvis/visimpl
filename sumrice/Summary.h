/*
 * SimulationSummaryWidget.h
 *
 *  Created on: 16 de feb. de 2016
 *      Author: sgalindo
 */

#ifndef __SIMULATIONSUMMARYWIDGET_H__
#define __SIMULATIONSUMMARYWIDGET_H__

#include <brion/brion.h>
#include <prefr/prefr.h>

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>

#include "FocusFrame.h"
#include "Histogram.h"
#include "SimulationData.h"

namespace visimpl
{
  class MultiLevelHistogram;


  struct Selection
  {
  public:

    Selection( void )
    {
      id = _counter;
      _counter++;
    }

    unsigned int currentID( void )
    {
      return _counter;
    }

    unsigned int id;
    std::string name;
    GIDUSet gids;

  private:
    static unsigned int _counter;
  };
}
class Summary : public QWidget
{

  Q_OBJECT;

public:

  typedef enum
  {
    T_STACK_FIXED = 0,
    T_STACK_EXPANDABLE

  } TStackType;

  Summary( QWidget* parent = 0, TStackType stackType = T_STACK_FIXED);

//  void Init( brion::SpikeReport* _spikes, brion::GIDSet gids );
  void Init( visimpl::SpikeData* spikes_, brion::GIDSet gids_ );
  void AddNewHistogram( const visimpl::Selection& selection
#ifdef VISIMPL_USE_ZEQ
                       , bool deferredInsertion = false
#endif
                       );
//  void CreateSummary( brion::CompartmentReport* _voltages );

  virtual void mouseMoveEvent( QMouseEvent* event_ );


  unsigned int bins( void );
  float zoomFactor( void );

  unsigned int histogramsNumber( void );

  void heightPerRow( unsigned int height_ );
  unsigned int heightPerRow( void );

  void showMarker( bool show_ );

  void colorScaleLocal( visimpl::TColorScale colorScale );
  visimpl::TColorScale colorScaleLocal( void );

  void colorScaleGlobal( visimpl::TColorScale colorScale );
  visimpl::TColorScale colorScaleGlobal( void );

  void regionWidth( float region );
  float regionWidth( void );

  const GIDUSet& gids( void );

signals:

  void histogramClicked( float );
  void histogramClicked( visimpl::MultiLevelHistogram* );

protected slots:

  void childHistogramClicked( float );
  void childHistogramClicked( void );

  void removeSelections( void );

  void colorScaleLocal( int value );
  void colorScaleGlobal( int value );

  void updateMouseMarker( QPoint point );

public slots:

  void bins( int bins_ );
  void zoomFactor( double zoom );

  void toggleAutoNameSelections( void )
  {
    _autoNameSelection = !_autoNameSelection;
  }
protected:

  struct StackRow
  {
  public:

    StackRow( )
    : histogram( nullptr )
    , label( nullptr )
    , checkBox( nullptr )
    { }

    ~StackRow( )
    { }

    visimpl::MultiLevelHistogram* histogram;
    QLabel* label;
    QCheckBox* checkBox;

  };

#ifdef VISIMPL_USE_ZEQ

protected slots:

  void deferredInsertion( void );

protected:

  std::list< visimpl::Selection > _pendingSelections;
  QTimer _insertionTimer;

#endif

  void Init( void );

  void CreateSummarySpikes( );
  void InsertSummarySpikes( const GIDUSet& gids );
//  void CreateSummaryVoltages( void );

  void UpdateGradientColors( bool replace = false );

  unsigned int _bins;
  float _zoomFactor;

//  brion::SpikeReport* _spikeReport;
  visimpl::SpikeData* _spikeReport;
  brion::CompartmentReport* _voltageReport;

  GIDUSet _gids;

  visimpl::MultiLevelHistogram* _mainHistogram;
  visimpl::MultiLevelHistogram* _detailHistogram;

  TStackType _stackType;

  visimpl::TColorScale _colorScaleLocal;
  visimpl::TColorScale _colorScaleGlobal;

  QColor _colorLocal;
  QColor _colorGlobal;

  std::vector< visimpl::MultiLevelHistogram* > _histograms;
  std::vector< StackRow > _rows;

  FocusFrame* _focusWidget;

  QGridLayout* _mainLayout;
  QWidget* _body;
  QWidget* _localColorWidget;
  QWidget* _globalColorWidget;
  QLabel* _currentValueLabel;
  QLabel* _globalMaxLabel;
  QLabel* _localMaxLabel;

  unsigned int _maxColumns;
  unsigned int _summaryColumns;
  unsigned int _heightPerRow;

  QPoint _lastMousePosition;
  bool _showMarker;

  bool _autoNameSelection;

  float _regionWidth;
};



#endif /* __SIMULATIONSUMMARYWIDGET_H__ */