/*
 * @file  MainWindow.cpp
 * @brief
 * @author Sergio E. Galindo <sergio.galindo@urjc.es>
 * @date
 * @remarks Copyright (c) GMRV/URJC. All rights reserved.
 *          Do not distribute without further notice.
 */

#include <visimpl/version.h>

#include "MainWindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QGridLayout>
#include <QShortcut>
#include <QMessageBox>
// #include "qt/CustomSlider.h"

#ifdef VISIMPL_USE_GMRVLEX
  #include <gmrvlex/gmrvlex.h>
#endif

#include <thread>

#include <sumrice/sumrice.h>

namespace visimpl
{

  MainWindow::MainWindow( QWidget* parent_,
                          bool updateOnIdle )
  : QMainWindow( parent_ )
  #ifdef VISIMPL_USE_GMRVLEX
  , _zeqConnection( false )
  , _subscriber( nullptr )
  , _thread( nullptr )
  #endif
  , _ui( new Ui::MainWindow )
  , _lastOpenedFileName( "" )
  , _openGLWidget( nullptr )
  , _summary( nullptr )
  , _simulationDock( nullptr )
  , _simSlider( nullptr )
  , _playButton( nullptr )
  , _startTimeLabel( nullptr )
  , _endTimeLabel( nullptr )
  , _repeatButton( nullptr )
  , _goToButton( nullptr )
  , _simConfigurationDock( nullptr )
  , _tfGroupBox( nullptr )
  , _tfEditor( nullptr )
  , _tfWidget( nullptr )
  , _autoNameGroups( false )
  , _groupsGroupBox( nullptr )
  , _groupLayout( nullptr )
  , _decayBox( nullptr )
  , _deltaTimeBox( nullptr )
  , _timeStepsPSBox( nullptr )
  , _clearSelectionButton( nullptr )
  , _selectionSizeLabel( nullptr )
  , _alphaNormalButton( nullptr )
  , _alphaAccumulativeButton( nullptr )
  {
    _ui->setupUi( this );

    _ui->actionUpdateOnIdle->setChecked( updateOnIdle );
    _ui->actionShowFPSOnIdleUpdate->setChecked( false );

    _ui->actionShowEventsActivity->setChecked( false );

  #ifdef SIMIL_USE_BRION
    _ui->actionOpenBlueConfig->setEnabled( true );
  #else
    _ui->actionOpenBlueConfig->setEnabled( false );
  #endif

  }

  void MainWindow::init( const std::string& zeqUri )
  {

    _openGLWidget = new OpenGLWidget( 0, 0, zeqUri );
    this->setCentralWidget( _openGLWidget );
    qDebug( ) << _openGLWidget->format( );

    _openGLWidget->idleUpdate( _ui->actionUpdateOnIdle->isChecked( ));

    connect( _ui->actionUpdateOnIdle, SIGNAL( triggered( )),
             _openGLWidget, SLOT( toggleUpdateOnIdle( )));

    connect( _ui->actionBackgroundColor, SIGNAL( triggered( )),
             _openGLWidget, SLOT( changeClearColor( )));

    connect( _ui->actionShowFPSOnIdleUpdate, SIGNAL( triggered( )),
             _openGLWidget, SLOT( toggleShowFPS( )));

    connect( _ui->actionShowEventsActivity, SIGNAL( triggered( bool )),
             _openGLWidget, SLOT( showEventsActivityLabels( bool )));

    connect( _ui->actionOpenBlueConfig, SIGNAL( triggered( )),
             this, SLOT( openBlueConfigThroughDialog( )));

    connect( _ui->actionQuit, SIGNAL( triggered( )),
             QApplication::instance(), SLOT( quit( )));

    // Connect about dialog
    connect( _ui->actionAbout, SIGNAL( triggered( )),
             this, SLOT( aboutDialog( )));


    connect( _ui->actionHome, SIGNAL( triggered( )),
             _openGLWidget, SLOT( updateSelection( )));

  #ifdef VISIMPL_USE_ZEROEQ
    _ui->actionShowSelection->setEnabled( true );
    _ui->actionShowSelection->setChecked( true );

    _openGLWidget->showSelection( true );

    connect( _ui->actionShowSelection, SIGNAL( toggled( bool )),
             this, SLOT( showSelection( bool )));

  #else
    _ui->actionShowSelection->setEnabled( false );
  #endif


    initPlaybackDock( );
    initSimControlDock( );

    connect( _simulationDock->toggleViewAction( ), SIGNAL( toggled( bool )),
               _ui->actionTogglePlaybackDock, SLOT( setChecked( bool )));

    connect( _ui->actionTogglePlaybackDock, SIGNAL( triggered( )),
             this, SLOT( togglePlaybackDock( )));

    connect( _simConfigurationDock->toggleViewAction( ), SIGNAL( toggled( bool )),
               _ui->actionToggleSimConfigDock, SLOT( setChecked( bool )));

    connect( _ui->actionToggleSimConfigDock, SIGNAL( triggered( )),
             this, SLOT( toggleSimConfigDock( )));



    #ifdef VISIMPL_USE_ZEROEQ

    _setZeqUri( zeqUri );

    #endif
  }

  MainWindow::~MainWindow( void )
  {
      delete _ui;
  }


  void MainWindow::showStatusBarMessage ( const QString& message )
  {
    _ui->statusbar->showMessage( message );
  }

  void MainWindow::openBlueConfig( const std::string& fileName,
                                   simil::TSimulationType simulationType,
                                   const std::string& reportLabel,
                                   const std::string& subsetEventFile )
  {
    _openGLWidget->loadData( fileName,
                             simil::TDataType::TBlueConfig,
                             simulationType, reportLabel );

    openSubsetEventFile( subsetEventFile, true );

    configurePlayer( );

  }

  void MainWindow::openBlueConfigThroughDialog( void )
  {
  #ifdef SIMIL_USE_BRION

    QString path = QFileDialog::getOpenFileName(
      this, tr( "Open BlueConfig" ), _lastOpenedFileName,
      tr( "BlueConfig ( BlueConfig CircuitConfig);; All files (*)" ),
      nullptr, QFileDialog::DontUseNativeDialog );

    if( path != QString( "" ))
    {

//      bool ok;
//
      simil::TSimulationType simType = simil::TSimSpikes;
//      QString text = QInputDialog::getText(
//            this, tr( "Please type a target" ),
//            tr( "Target name:" ), QLineEdit::Normal,
//            "Mosaic", &ok );

//      bool ok1, ok2;
//      QInputDialog simTypeDialog;
//      simil::TSimulationType simType = simil::TSimSpikes;
//      QStringList items = {"Spikes", "Voltages"};



//      QString text = QInputDialog::getItem(
//        this, tr( "Please select simulation type" ),
//        tr( "Type:" ), items, 0, false, &ok1 );
//
//      if( !ok1 )
//        return;
//
//      if( text == items[0] )
//      {
//        simType = simil::TSimSpikes;
//        ok2 = true;
//      }
//      else
//      {
//        simType = simil::TSimVoltages;
//
//        text = QInputDialog::getText(
//            this, tr( "Please select report" ),
//            tr( "Report:" ), QLineEdit::Normal,
//            "soma", &ok2 );
//      }

//      if ( ok && !text.isEmpty( ))
      {
  //      std::string targetLabel = text.toStdString( );
        std::string reportLabel;// = text.toStdString( );
        _lastOpenedFileName = QFileInfo(path).path( );
        std::string fileName = path.toStdString( );
        openBlueConfig( fileName, simType, reportLabel );
      }


    }
  #endif

  }



  void MainWindow::openHDF5File( const std::string& networkFile,
                                 simil::TSimulationType simulationType,
                                 const std::string& activityFile,
                                 const std::string& subsetEventFile )
  {
    _openGLWidget->loadData( networkFile,
                             simil::TDataType::THDF5,
                             simulationType,
                             activityFile );

    openSubsetEventFile( subsetEventFile, true );

    configurePlayer( );
  }


  void MainWindow::openHDF5ThroughDialog( void )
  {
    QString path = QFileDialog::getOpenFileName(
      this, tr( "Open a H5 network file" ), _lastOpenedFileName,
      tr( "hdf5 ( *.h5);; All files (*)" ), nullptr,
      QFileDialog::DontUseNativeDialog );

    std::string networkFile;
    std::string activityFile;

    if ( path == QString( "" ))
      return;

    networkFile = path.toStdString( );

    path = QFileDialog::getOpenFileName(
    this, tr( "Open a H5 activity file" ), path,
    tr( "hdf5 ( *.h5);; All files (*)" ), nullptr,
    QFileDialog::DontUseNativeDialog );

    if( path == QString( "" ))
    {
      return;
    }

    activityFile = path.toStdString( );

    openHDF5File( networkFile, simil::TSimSpikes, activityFile );
  }

  void MainWindow::openSubsetEventFile( const std::string& filePath,
                                        bool append )
  {
    if( filePath.empty( ))
      return;

    if( !append )
      _openGLWidget->player( )->data( )->subsetsEvents( )->clear( );


    if( filePath.find( "json" ) != std::string::npos )
    {
      std::cout << "Loading JSON file: " << filePath << std::endl;
      _openGLWidget->player( )->data( )->subsetsEvents( )->loadJSON( filePath );
    }
    else if( filePath.find( "h5" ) != std::string::npos )
    {
      std::cout << "Loading H5 file: " << filePath << std::endl;
      _openGLWidget->player( )->data( )->subsetsEvents( )->loadH5( filePath );
    }
    else
    {
      std::cout << "Subset Events file not found: " << filePath << std::endl;
    }
  }


  void MainWindow::aboutDialog( void )
  {


    QMessageBox::about(
      this, tr("About ViSimpl"),
      tr("<p><BIG><b>ViSimpl - SimPart</b></BIG><br><br>") +
      tr( "version " ) +
      tr( visimpl::Version::getString( ).c_str( )) +
      tr( " (" ) +
      tr( std::to_string( visimpl::Version::getRevision( )).c_str( )) +
      tr( ")" ) +
      tr ("<br><br>GMRV - Universidad Rey Juan Carlos<br><br>"
          "<a href=www.gmrv.es>www.gmrv.es</a><br>"
          "<a href='mailto:gmrv@gmrv.es'>gmrv@gmrv.es</a><br><br>"
          "<br>(c) 2015. Universidad Rey Juan Carlos<br><br>"
          "<img src=':/icons/logoGMRV.png' > &nbsp; &nbsp;"
          "<img src=':/icons/logoURJC.png' >"
          "</p>"
          ""));

  }

  void MainWindow::togglePlaybackDock( void )
  {
    if( _ui->actionTogglePlaybackDock->isChecked( ))
      _simulationDock->show( );
    else
      _simulationDock->close( );

    update( );
  }

  void MainWindow::toggleSimConfigDock( void )
  {
    if( _ui->actionToggleSimConfigDock->isChecked( ))
      _simConfigurationDock->show( );
    else
      _simConfigurationDock->close( );

    update( );
  }

  void MainWindow::configurePlayer( void )
  {
    connect( _openGLWidget, SIGNAL( updateSlider( float )),
               this, SLOT( UpdateSimulationSlider( float )));


    _startTimeLabel->setText(
        QString::number( (double)_openGLWidget->player( )->startTime( )));

    _endTimeLabel->setText(
          QString::number( (double)_openGLWidget->player( )->endTime( )));

  #ifdef SIMIL_USE_ZEROEQ
    _openGLWidget->player( )->zeqEvents( )->playbackOpReceived.connect(
        boost::bind( &MainWindow::ApplyPlaybackOperation, this, _1 ));

    _openGLWidget->player( )->zeqEvents( )->frameReceived.connect(
        boost::bind( &MainWindow::requestPlayAt, this, _1 ));

  #endif

    changeEditorColorMapping( );
    changeEditorSimDeltaTime( );
    changeEditorSimTimestepsPS( );
    changeEditorSizeFunction( );
    changeEditorDecayValue( );
    initSummaryWidget( );
  }

  void MainWindow::initPlaybackDock( void )
  {
    _simulationDock = new QDockWidget( );
    _simulationDock->setMinimumHeight( 100 );
    _simulationDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                                    QSizePolicy::MinimumExpanding );

    unsigned int totalHSpan = 20;

    QWidget* content = new QWidget( );
    QGridLayout* dockLayout = new QGridLayout( );
    content->setLayout( dockLayout );

    _simSlider = new CustomSlider( Qt::Horizontal );
    _simSlider->setMinimum( 0 );
    _simSlider->setMaximum( 1000 );
    _simSlider->setSizePolicy( QSizePolicy::Preferred,
                               QSizePolicy::Preferred );

  //  QPushButton* playButton = new QPushButton( );
    _playButton = new QPushButton( );
    _playButton->setSizePolicy( QSizePolicy::MinimumExpanding,
                                QSizePolicy::MinimumExpanding );
    QPushButton* stopButton = new QPushButton( );
    QPushButton* nextButton = new QPushButton( );
    QPushButton* prevButton = new QPushButton( );
  //  QPushButton* repeatButton = new QPushButton( );
    _repeatButton = new QPushButton( );
    _repeatButton->setCheckable( true );
    _repeatButton->setChecked( false );

    _goToButton = new QPushButton( );
    _goToButton->setText( QString( "Play at..." ));

  //  QIcon playIcon;
  //  QIcon pauseIcon;
    QIcon stopIcon;
    QIcon nextIcon;
    QIcon prevIcon;
    QIcon repeatIcon;

    _playIcon.addFile( QStringLiteral( ":/icons/play.png" ), QSize( ),
                      QIcon::Normal, QIcon::Off );
    _pauseIcon.addFile( QStringLiteral( ":/icons/pause.png" ), QSize( ),
                       QIcon::Normal, QIcon::Off) ;
    stopIcon.addFile( QStringLiteral( ":/icons/stop.png" ), QSize( ),
                      QIcon::Normal, QIcon::Off );
    nextIcon.addFile( QStringLiteral( ":/icons/next.png" ), QSize( ),
                      QIcon::Normal, QIcon::Off );
    prevIcon.addFile( QStringLiteral( ":/icons/previous.png" ), QSize( ),
                      QIcon::Normal, QIcon::Off );
    repeatIcon.addFile( QStringLiteral( ":/icons/repeat.png" ), QSize( ),
                        QIcon::Normal, QIcon::Off );

    _playButton->setIcon( _playIcon );
    stopButton->setIcon( stopIcon );
    nextButton->setIcon( nextIcon );
    prevButton->setIcon( prevIcon );
    _repeatButton->setIcon( repeatIcon );

    _startTimeLabel = new QLabel( "" );
    _startTimeLabel->setSizePolicy( QSizePolicy::MinimumExpanding,
                                    QSizePolicy::Preferred );
    _endTimeLabel = new QLabel( "" );
    _endTimeLabel->setSizePolicy( QSizePolicy::Preferred,
                                    QSizePolicy::Preferred );

    unsigned int row = 2;
    dockLayout->addWidget( _startTimeLabel, row, 0, 1, 2 );
    dockLayout->addWidget( _simSlider, row, 1, 1, totalHSpan - 3 );
    dockLayout->addWidget( _endTimeLabel, row, totalHSpan - 2, 1, 1, Qt::AlignRight );

    row++;
    dockLayout->addWidget( _repeatButton, row, 7, 1, 1 );
    dockLayout->addWidget( prevButton, row, 8, 1, 1 );
    dockLayout->addWidget( _playButton, row, 9, 2, 2 );
    dockLayout->addWidget( stopButton, row, 11, 1, 1 );
    dockLayout->addWidget( nextButton, row, 12, 1, 1 );
    dockLayout->addWidget( _goToButton, row, 13, 1, 1 );

    connect( _playButton, SIGNAL( clicked( )),
             this, SLOT( PlayPause( )));

    connect( stopButton, SIGNAL( clicked( )),
               this, SLOT( Stop( )));

    connect( nextButton, SIGNAL( clicked( )),
               this, SLOT( NextStep( )));

    connect( prevButton, SIGNAL( clicked( )),
               this, SLOT( PreviousStep( )));

    connect( _repeatButton, SIGNAL( clicked( )),
               this, SLOT( Repeat( )));

    connect( _simSlider, SIGNAL( sliderPressed( )),
             this, SLOT( PlayAt( )));

    connect( _goToButton, SIGNAL( clicked( )),
             this, SLOT( playAtButtonClicked( )));

  //  connect( _simSlider, SIGNAL( sliderMoved( )),
  //             this, SLOT( PlayAt( )));

    _summary = new visimpl::Summary( nullptr, visimpl::T_STACK_FIXED );
  //  _summary->setVisible( false );
    _summary->setMinimumHeight( 50 );

    dockLayout->addWidget( _summary, 0, 1, 2, totalHSpan - 3 );

    _simulationDock->setWidget( content );
    this->addDockWidget( Qt::/*DockWidgetAreas::enum_type::*/BottomDockWidgetArea,
                         _simulationDock );

    connect( _summary, SIGNAL( histogramClicked( float )),
             this, SLOT( PlayAt( float )));
  }

  void MainWindow::initSimControlDock( void )
  {
    _simConfigurationDock = new QDockWidget( );
    _simConfigurationDock->setMinimumHeight( 100 );
    _simConfigurationDock->setMinimumWidth( 300 );
  //  _simConfigurationDock->setMaximumHeight( 400 );
    _simConfigurationDock->setSizePolicy( QSizePolicy::MinimumExpanding,
                                    QSizePolicy::MinimumExpanding );

  //  _tfEditor = new TransferFunctionEditor( );
    _tfWidget = new TransferFunctionWidget( );
    _tfWidget->setMinimumHeight( 150 );

  //  _psWidget = new ParticleSizeWidget( );
  //  _psWidget->setMinimumHeight( 150 );

    _deltaTimeBox = new QDoubleSpinBox( );
    _deltaTimeBox->setMinimum( 0.00000001 );
    _deltaTimeBox->setMaximum( 50 );
    _deltaTimeBox->setSingleStep( 0.05 );
    _deltaTimeBox->setDecimals( 5 );
    _deltaTimeBox->setMaximumWidth( 100 );

    _timeStepsPSBox = new QDoubleSpinBox( );
    _timeStepsPSBox->setMinimum( 0.00000001 );
    _timeStepsPSBox->setMaximum( 50 );
    _timeStepsPSBox->setSingleStep( 1.0 );
    _timeStepsPSBox->setDecimals( 5 );
    _timeStepsPSBox->setMaximumWidth( 100 );

    _decayBox = new QDoubleSpinBox( );
    _decayBox->setMinimum( 0.01 );
    _decayBox->setMaximum( 600.0 );
    _decayBox->setDecimals( 5 );
    _decayBox->setMaximumWidth( 100 );

    _alphaNormalButton = new QRadioButton( "Normal" );
    _alphaAccumulativeButton = new QRadioButton( "Accumulative" );
    _openGLWidget->SetAlphaBlendingAccumulative( false );

    _clearSelectionButton = new QPushButton( "Discard" );
    _clearSelectionButton->setEnabled( false );
    _selectionSizeLabel = new QLabel( "0" );

    _addGroupButton = new QPushButton( "Add group" );
    _addGroupButton->setEnabled( false );
    _addGroupButton->setToolTip( "Click to create a group from current selection.");

    QWidget* container = new QWidget( );
    QVBoxLayout* verticalLayout = new QVBoxLayout( );
  //  QPushButton* applyColorButton = new QPushButton( QString( "Apply" ));

    _tfGroupBox = new QGroupBox( "Color and Size transfer function" );
    QVBoxLayout* tfLayout = new QVBoxLayout( );
    tfLayout->addWidget( _tfWidget );
    _tfGroupBox->setLayout( tfLayout );
    _tfGroupBox->setMaximumHeight( 250 );

    QGroupBox* tSpeedGB = new QGroupBox( "Simulation playback speed" );
    QGridLayout* sfLayout = new QGridLayout( );
    sfLayout->addWidget( new QLabel( "Simulation timestep:" ), 0, 0, 1, 1 );
    sfLayout->addWidget( _deltaTimeBox, 0, 1, 1, 1  );
    sfLayout->addWidget( new QLabel( "Timesteps per second:" ), 1, 0, 1, 1 );
    sfLayout->addWidget( _timeStepsPSBox, 1, 1, 1, 1  );
    tSpeedGB->setLayout( sfLayout );
    tSpeedGB->setMaximumHeight( 200 );

    QGroupBox* dFunctionGB = new QGroupBox( "Decay function" );
    QHBoxLayout* dfLayout = new QHBoxLayout( );
    dfLayout->addWidget( new QLabel( "Decay \n(simulation time): " ));
    dfLayout->addWidget( _decayBox );
    dFunctionGB->setLayout( dfLayout );
    dFunctionGB->setMaximumHeight( 200 );

    QGroupBox* rFunctionGB = new QGroupBox( "Alpha blending function" );
    QHBoxLayout* rfLayout = new QHBoxLayout( );
    rfLayout->addWidget( new QLabel( "Alpha\nBlending: " ));
    rfLayout->addWidget( _alphaNormalButton );
    rfLayout->addWidget( _alphaAccumulativeButton );
    rFunctionGB->setLayout( rfLayout );
    rFunctionGB->setMaximumHeight( 200 );

    QGroupBox* selFunctionGB = new QGroupBox( "Current selection");
    QHBoxLayout* selLayout = new QHBoxLayout( );
    selLayout->addWidget( new QLabel( "Selection size: " ));
    selLayout->addWidget( _selectionSizeLabel );
    selLayout->addWidget( _addGroupButton );
    selLayout->addWidget( _clearSelectionButton );
    selFunctionGB->setLayout( selLayout );
    selFunctionGB->setMaximumHeight( 200 );


    _groupsGroupBox = new QGroupBox( "Current visualization groups" );
    _groupsGroupBox->setMaximumHeight( 200 );
    _groupLayout = new QGridLayout( );
    _groupLayout->setAlignment( Qt::AlignTop );


    QWidget* groupContainer = new QWidget( );
    groupContainer->setLayout( _groupLayout );

    QScrollArea* groupScroll = new QScrollArea( );
    groupScroll->setWidget( groupContainer );
    groupScroll->setWidgetResizable( true );
    groupScroll->setFrameShape( QFrame::Shape::NoFrame );
    groupScroll->setFrameShadow( QFrame::Shadow::Plain );
    groupScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    groupScroll->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );

    QGridLayout* groupOuterLayout = new QGridLayout( );
    groupOuterLayout->setMargin( 0 );
    groupOuterLayout->addWidget( groupScroll );

    _groupsGroupBox->setLayout( groupOuterLayout );
    _groupsGroupBox->setVisible( false );

    verticalLayout->setAlignment( Qt::AlignTop );
    verticalLayout->addWidget( _tfGroupBox );
    verticalLayout->addWidget( _groupsGroupBox );
    verticalLayout->addWidget( tSpeedGB );
    verticalLayout->addWidget( dFunctionGB );
    verticalLayout->addWidget( rFunctionGB );
    verticalLayout->addWidget( selFunctionGB  );

    container->setLayout( verticalLayout );
    _simConfigurationDock->setWidget( container );

    this->addDockWidget( Qt::/*DockWidgetAreas::enum_type::*/RightDockWidgetArea,
                         _simConfigurationDock );

  //  connect( applyColorButton, SIGNAL( clicked( void )),
  //             this, SLOT( UpdateSimulationColorMapping( void )));

    connect( _tfWidget, SIGNAL( colorChanged( void )),
             this, SLOT( UpdateSimulationColorMapping( void )));
    connect( _tfWidget, SIGNAL( colorChanged( void )),
             this, SLOT( UpdateSimulationSizeFunction( void )));
    connect( _tfWidget, SIGNAL( previewColor( void )),
             this, SLOT( PreviewSimulationColorMapping( void )));
    connect( _tfWidget, SIGNAL( previewColor( void )),
             this, SLOT( PreviewSimulationSizeFunction( void )));

    connect( _deltaTimeBox, SIGNAL( valueChanged( double )),
               this, SLOT( updateSimDeltaTime( void )));

    connect( _timeStepsPSBox, SIGNAL( valueChanged( double )),
               this, SLOT( updateSimTimestepsPS( void )));

    connect( _decayBox, SIGNAL( valueChanged( double )),
             this, SLOT( updateSimulationDecayValue( void )));

    connect( _alphaNormalButton, SIGNAL( toggled( bool )),
             this, SLOT( AlphaBlendingToggled( void ) ));

    connect( _clearSelectionButton, SIGNAL( clicked( void )),
             this, SLOT( ClearSelection( void )));

    connect( _addGroupButton, SIGNAL( clicked( void )),
             this, SLOT( addGroupFromSelection( )));

  //  connect( _alphaAccumulativeButton, SIGNAL( toggled( bool )),
  //           this, SLOT( AlphaBlendingToggled( void ) ));

    _alphaNormalButton->setChecked( true );
  }

  void MainWindow::initSummaryWidget( void )
  {
    simil::TSimulationType simType =
        _openGLWidget->player( )->simulationType( );

    if( simType == simil::TSimSpikes )
    {
      simil::SpikesPlayer* spikesPlayer =
          dynamic_cast< simil::SpikesPlayer* >( _openGLWidget->player( ));

      std::cout << "Creating summary..." << std::endl;

      _summary->Init( spikesPlayer->data( ));

      _summary->simulationPlayer( _openGLWidget->player( ));

      _openGLWidget->subsetEventsManager( spikesPlayer->data( )->subsetsEvents( ));

      _openGLWidget->showEventsActivityLabels( _ui->actionShowEventsActivity->isChecked( ));
    }

  }

  void MainWindow::PlayPause( bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( !_openGLWidget->player( )->isPlaying( ))
      Play( notify );
    else
      Pause( notify );
  }

  void MainWindow::Play( bool notify )
  {
  //  playIcon.swap( pauseIcon );
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      _openGLWidget->Play( );

      _playButton->setIcon( _pauseIcon );

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
        _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
  #endif
      }
    }
  }

  void MainWindow::Pause( bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      _openGLWidget->Pause( );
      _playButton->setIcon( _playIcon );

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
      _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PAUSE );
  #endif
      }
    }
  }

  void MainWindow::Stop( bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      _openGLWidget->Stop( );
      _playButton->setIcon( _playIcon );
      _startTimeLabel->setText(
            QString::number( (double)_openGLWidget->player( )->startTime( )));

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
        _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::STOP );
  #endif
      }

    }
  }

  void MainWindow::Repeat( bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      bool repeat = _repeatButton->isChecked( );
  //    std::cout << "Repeat " << boolalpha << repeat << std::endl;
      _openGLWidget->Repeat( repeat );

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
        _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( repeat ?
                                    zeroeq::gmrv::ENABLE_LOOP :
                                    zeroeq::gmrv::DISABLE_LOOP );
  #endif
      }

    }
  }

  void MainWindow::requestPlayAt( float percentage )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    PlayAt( percentage, false );
  }

  void MainWindow::PlayAt( bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      PlayAt( _simSlider->sliderPosition( ), notify );
    }
  }

  void MainWindow::PlayAt( float percentage, bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      int sliderPosition = percentage *
                      ( _simSlider->maximum( ) - _simSlider->minimum( )) +
                      _simSlider->minimum( );

      _simSlider->setSliderPosition( sliderPosition );

  //    _openGLWidget->resetParticles( );

      _playButton->setIcon( _pauseIcon );

      _openGLWidget->PlayAt( percentage );

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
      // Send event
      _openGLWidget->player( )->zeqEvents( )->sendFrame( _simSlider->minimum( ),
                             _simSlider->maximum( ),
                             sliderPosition );

      _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
  #endif
      }

    }
  }

  void MainWindow::PlayAt( int sliderPosition, bool notify )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {

  //    _openGLWidget->Pause( );


      int value = _simSlider->value( );
      float percentage = float( value - _simSlider->minimum( )) /
                         float( _simSlider->maximum( ) - _simSlider->minimum( ));

      _simSlider->setSliderPosition( sliderPosition );

  //    _openGLWidget->resetParticles( );

      _playButton->setIcon( _pauseIcon );

      _openGLWidget->PlayAt( percentage );

      if( notify )
      {
  #ifdef SIMIL_USE_ZEROEQ
      // Send event
      _openGLWidget->player( )->zeqEvents( )->sendFrame( _simSlider->minimum( ),
                             _simSlider->maximum( ),
                             sliderPosition );

      _openGLWidget->player( )->zeqEvents( )->sendPlaybackOp( zeroeq::gmrv::PLAY );
  #endif
      }
    }
  }

  void MainWindow::PreviousStep( bool /*notify*/ )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      _playButton->setIcon( _pauseIcon );
      _openGLWidget->player( )->Play( );
      _openGLWidget->NextStep( );
    }
  }

  void MainWindow::NextStep( bool /*notify*/ )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    if( _openGLWidget )
    {
      _playButton->setIcon( _pauseIcon );
      _openGLWidget->player( )->Play( );
      _openGLWidget->NextStep( );

    }
  }

  void MainWindow::UpdateSimulationSlider( float percentage )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    _startTimeLabel->setText(
          QString::number( (double)_openGLWidget->currentTime( )));

    int total = _simSlider->maximum( ) - _simSlider->minimum( );

    int position = percentage * total;

    _simSlider->setSliderPosition( position );

    if( _summary )
      _summary->repaintHistograms( );
  }

  void MainWindow::UpdateSimulationColorMapping( void )
  {
    _openGLWidget->changeSimulationColorMapping( _tfWidget->getColors( ));
  }

  void MainWindow::PreviewSimulationColorMapping( void )
  {
    _openGLWidget->changeSimulationColorMapping( _tfWidget->getPreviewColors( ));
  }

  void MainWindow::changeEditorColorMapping( void )
  {
    _tfWidget->setColorPoints( _openGLWidget->getSimulationColorMapping( ));
  }

  void MainWindow::changeEditorSizeFunction( void )
  {
    _tfWidget->setSizeFunction( _openGLWidget->getSimulationSizeFunction( ));
  }

  void MainWindow::UpdateSimulationSizeFunction( void )
  {
    _openGLWidget->changeSimulationSizeFunction( _tfWidget->getSizeFunction( ));
  }

  void MainWindow::PreviewSimulationSizeFunction( void )
  {
    _openGLWidget->changeSimulationSizeFunction( _tfWidget->getSizePreview( ));
  }

  void MainWindow::changeEditorSimDeltaTime( void )
  {
    _deltaTimeBox->setValue( _openGLWidget->simulationDeltaTime( ));
  }

  void MainWindow::updateSimDeltaTime( void )
  {
    _openGLWidget->simulationDeltaTime( _deltaTimeBox->value( ));
  }

  void MainWindow::changeEditorSimTimestepsPS( void )
  {
    _timeStepsPSBox->setValue( _openGLWidget->simulationStepsPerSecond( ));
  }

  void MainWindow::updateSimTimestepsPS( void )
  {
    _openGLWidget->simulationStepsPerSecond( _timeStepsPSBox->value( ));
  }

  void MainWindow::changeEditorDecayValue( void )
  {
    _decayBox->setValue( _openGLWidget->getSimulationDecayValue( ));
  }

  void MainWindow::updateSimulationDecayValue( void )
  {
    _openGLWidget->changeSimulationDecayValue( _decayBox->value( ));
  }

  void MainWindow::AlphaBlendingToggled( void )
  {
    std::cout << "Changing alpha blending... ";
    if( _alphaNormalButton->isChecked( ))
    {
      std::cout << "Normal" << std::endl;
      _openGLWidget->SetAlphaBlendingAccumulative( false );
    }
    else
    {
      std::cout << "Accumulative" << std::endl;
      _openGLWidget->SetAlphaBlendingAccumulative( true );
    }
  }

  void MainWindow::showSelection( bool show )
  {
    _tfGroupBox->setVisible( show );
    _groupsGroupBox->setVisible( !show );
//    _tfWidget->setEnabled( show );


    _openGLWidget->showSelection( show );
  }


  #ifdef VISIMPL_USE_ZEROEQ

  #ifdef VISIMPL_USE_GMRVLEX

    void MainWindow::ApplyPlaybackOperation( unsigned int playbackOp )
    {
      zeroeq::gmrv::PlaybackOperation operation =
          ( zeroeq::gmrv::PlaybackOperation ) playbackOp;

      switch( operation )
      {
        case zeroeq::gmrv::PLAY:
  //        std::cout << "Received play" << std::endl;
          Play( false );
          break;
        case zeroeq::gmrv::PAUSE:
          Pause( false );
  //        std::cout << "Received pause" << std::endl;
          break;
        case zeroeq::gmrv::STOP:
  //        std::cout << "Received stop" << std::endl;
          Stop( false );
          break;
        case zeroeq::gmrv::BEGIN:
  //        std::cout << "Received begin" << std::endl;
          PreviousStep( false );
          break;
        case zeroeq::gmrv::END:
  //        std::cout << "Received end" << std::endl;
          NextStep( false );
          break;
        case zeroeq::gmrv::ENABLE_LOOP:
  //        std::cout << "Received enable loop" << std::endl;
          _zeqEventRepeat( true );
          break;
        case zeroeq::gmrv::DISABLE_LOOP:
  //        std::cout << "Received disable loop" << std::endl;
          _zeqEventRepeat( false );
          break;
        default:
          break;
      }

    }

    void MainWindow::_zeqEventRepeat( bool repeat )
    {
      _repeatButton->setChecked( repeat );
      Repeat( false );
    }

  #endif

  void MainWindow::_setZeqUri( const std::string& uri_ )
  {
    _zeqConnection = true;
    _uri = uri_.empty( ) ? zeroeq::DEFAULT_SESSION : uri_;

    _subscriber = new zeroeq::Subscriber( _uri );

    _subscriber->subscribe(
        lexis::data::SelectedIDs::ZEROBUF_TYPE_IDENTIFIER( ),
        [&]( const void* data_, const size_t size_ )
        { _onSelectionEvent( lexis::data::SelectedIDs::create( data_, size_ ));});

    _thread = new std::thread( [&]() { while( true ) _subscriber->receive( 10000 );});
  }

  //void* MainWindow::_Subscriber( void* subs )
  //{
  //  zeroeq::Subscriber* subscriber = static_cast< zeroeq::Subscriber* >( subs );
  //  while ( true )
  //  {
  //    subscriber->receive( 10000 );
  //  }
  //  pthread_exit( NULL );
  //}

  void MainWindow::ClearSelection( void )
  {
    if( _openGLWidget )
    {
      _openGLWidget->clearSelection( );

      _addGroupButton->setEnabled( false );
      _clearSelectionButton->setEnabled( false );
      _selectionSizeLabel->setText( "0" );
    }
  }

  void MainWindow::playAtButtonClicked( void )
  {
    if( !_openGLWidget || !_openGLWidget->player( ))
      return;

    bool ok;
    double result =
        QInputDialog::getDouble( this, tr( "Set simulation time to play:"),
                                 tr( "Simulation time" ),
                                 ( double )_openGLWidget->currentTime( ),
                                 ( double )_openGLWidget->player( )->data( )->startTime( ),
                                 ( double )_openGLWidget->player( )->data( )->endTime( ),
                                 3, &ok, Qt::Popup );

    if( ok )
    {
      float percentage = ( result - _openGLWidget->player( )->startTime( )) /
          ( _openGLWidget->player( )->endTime( ) -
              _openGLWidget->player( )->startTime( ));

      percentage = std::max( 0.0f, std::min( 1.0f, percentage ));

      PlayAt( percentage, true );
    }
  }

  void MainWindow::_onSelectionEvent( lexis::data::ConstSelectedIDsPtr selected )
  {

    std::cout << "Received selection" << std::endl;
    if( _openGLWidget )
    {
  //    std::vector< unsigned int > selected =
  //        zeq::hbp::deserializeSelectedIDs( selected );

      std::vector< uint32_t > ids = selected->getIdsVector( );



      visimpl::GIDUSet selectedSet( ids.begin( ), ids.end( ));

      if( selectedSet.size( ) == 0 )
        return;

      _openGLWidget->setSelectedGIDs( selectedSet );

      _addGroupButton->setEnabled( true );
      _clearSelectionButton->setEnabled( true );
      _selectionSizeLabel->setText( QString::number( selectedSet.size( )));
    }

  }

#endif

  void MainWindow::addGroupFromSelection( void )
  {
    InputMultiplexer* inputMux = _openGLWidget->inputMultiplexer( );

    unsigned int currentIndex = inputMux->groups( ).size( );

    QString groupName( "Group " + QString::number( currentIndex ));

    if( !_autoNameGroups )
    {
      bool ok;
      groupName =
          QInputDialog::getText( this, tr( "Group Name" ),
                                 tr( "Please, introduce group name: "),
                                 QLineEdit::Normal,
                                 groupName,
                                 &ok );

      if( !ok )
        return;
    }

    inputMux->addVisualGroup( inputMux->selection( ),
                              groupName.toStdString( ),
                              false );

    if( inputMux->showGroups( ))
      inputMux->showGroups( true );

    QFrame* frame = new QFrame( );
    auto colors = _openGLWidget->colorPalette( ).colors( );

    frame->setStyleSheet( "background-color: " + colors[ currentIndex ].name( ) );
    frame->setMinimumSize( 20, 20 );
    frame->setMaximumSize( 20, 20 );

//    QIcon* eye = new QIcon( ":/icons/show.png" );
    QCheckBox* buttonVisibility = new QCheckBox( "active" );
    buttonVisibility->setChecked( true );
//    buttonVisibility->setMinimumSize( 60, 50 );
//    buttonVisibility->setMaximumSize( 60, 50 );

    _groupsVisButtons.push_back( buttonVisibility );

    connect( buttonVisibility, SIGNAL( clicked( )),
             this, SLOT( checkGroupsVisibility( )));

    _groupLayout->addWidget( frame, currentIndex, 0, 1, 1 );
    _groupLayout->addWidget( new QLabel( groupName ), currentIndex, 1, 1, 1 );
    _groupLayout->addWidget( buttonVisibility, currentIndex, 2, 1, 1 );

    _openGLWidget->setUpdateSelection( );
    _openGLWidget->update( );
  }

  void MainWindow::checkGroupsVisibility( void )
  {
    unsigned int counter = 0;
    auto group = _openGLWidget->inputMultiplexer( )->groups( ).begin( );
    for( auto button : _groupsVisButtons )
    {
      if( button->isChecked( ) != ( *group)->active( ) )
      {
        _openGLWidget->inputMultiplexer( )->setVisualGroupState( counter, button->isChecked( ));

      }
      ++group;
      ++counter;
    }

    _openGLWidget->setUpdateSelection( );
    _openGLWidget->update( );
  }



} // namespace visimpl
