//    QMessageBox::information( this, "", "" );

#include <windows.h>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QProcessEnvironment>

#include "dlgmain.h"
#include "ui_dlgmain.h"
#include "dlgtimer.h"

dlgMain::dlgMain(QWidget *parent) : QDialog(parent), ui(new Ui::dlgMain)
{
    ui->setupUi(this);

    m_nTimer            = 0;
    m_bMousePressed     = false;
    m_bReloadLanguage   = false;

//    setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    trayIconMenu = new QMenu(this);
    trayIcon = new QSystemTrayIcon(this);

    _setActions();
    _setMenu();

    trayIcon->setIcon( QIcon( ":/countdowntimer.png" ) );
    trayIcon->show();

    ui->ledTimerMinute->setText( "15" );
    ui->ledTimerSecond->setText( "00" );
    ui->ledTimerExtMinute->setText( "01" );
    ui->ledTimerExtSecond->setText( "00" );

    poDlgTimer = new dlgTimer();

    connect( poDlgTimer, SIGNAL(timerFinished()), this, SLOT(on_Timer_finished()) );
    connect( poDlgTimer, SIGNAL(timerExtFinished()), this, SLOT(on_ExtTimer_finished()) );
    connect( poDlgTimer, SIGNAL(timerReseted()), this, SLOT(on_pbReset_clicked()) );
    connect( poDlgTimer, SIGNAL(timerSet(int)), this, SLOT(timerSet(int)) );
    connect( poDlgTimer, SIGNAL(showSettings()), this, SLOT(on_actionShow_triggered()) );
    connect( this, SIGNAL(signalStartCounter(int,int)), poDlgTimer, SLOT(counterStart(int,int)) );
    connect( this, SIGNAL(signalStopCounter()), poDlgTimer, SLOT(counterStop()) );
    connect( this, SIGNAL(signalResetCounter(int,int)), poDlgTimer, SLOT(counterReset(int,int)) );
    connect( this, SIGNAL(signalContinueCounter()), poDlgTimer, SLOT(counterContinue()) );
    connect( this, SIGNAL(signalSetTimer(int)), poDlgTimer, SLOT(counterSetTimer(int)) );
    connect( this, SIGNAL(signalSetExtTimer(int)), poDlgTimer, SLOT(counterSetExtTimer(int)) );

    connect( ui->chkProcessExtended, SIGNAL(clicked()), this, SLOT(on_actionTimeSettings_changed()) );

    connect( ui->chkShowMainWindow, SIGNAL(clicked()), this, SLOT(on_actionSettings_changed()) );
    connect( ui->chkShowTimerWindow, SIGNAL(clicked()), this, SLOT(on_actionSettings_changed()) );
    connect( ui->chkStartTimerOnStart, SIGNAL(clicked()), this, SLOT(on_actionSettings_changed()) );
    connect( ui->chkUseSavedTimes, SIGNAL(clicked()), this, SLOT(on_actionSettings_changed()) );
    connect( ui->chkSaveTimerSettings, SIGNAL(clicked()), this, SLOT(on_actionSettings_changed()) );

    int nTimerLength;
    int nTimerExtLength;

    m_qsLang                    = obPref.value( "Lang", "en" ).toString();
    m_bShowMainWindowOnStart    = obPref.value( "ShowMainWindowOnStart", 0 ).toBool();
    m_bShowTimerPanelOnStart    = obPref.value( "ShowTimerPanelOnStart", 1 ).toBool();
    m_bStartTimerOnStart        = obPref.value( "StartTimerOnStart", 0 ).toBool();
    m_bUseSavedTimers           = obPref.value( "UseSavedTimers", 1 ).toBool();
    m_bSaveTimerSettingsOnExit  = obPref.value( "SaveTimerSettingsOnExit", 0 ).toBool();

    ui->chkShowMainWindow->setChecked( m_bShowMainWindowOnStart );
    ui->chkShowTimerWindow->setChecked( m_bShowTimerPanelOnStart );
    ui->chkStartTimerOnStart->setChecked( m_bStartTimerOnStart );
    ui->chkUseSavedTimes->setChecked( m_bUseSavedTimers );
    ui->chkSaveTimerSettings->setChecked( m_bSaveTimerSettingsOnExit );

    if( m_bStartTimerOnStart )  { ui->chkShowTimerWindow->setChecked( true );   }

    nTimerLength                = obPref.value( "Timer/TimerLength", 900 ).toInt();
    nTimerExtLength             = obPref.value( "Timer/TimerExtLength", 60 ).toInt();
    m_bContinueExtended         = obPref.value( "TimerAction/ContinueCountdown" ).toBool();
    m_nActionOnFinish           = obPref.value( "TimerAction/ActionOnFinish" ).toInt();
    m_qsPopupMessage            = obPref.value( "TimerAction/ActionMessage" ).toString();

    if( m_bUseSavedTimers )
    {
        ui->ledTimerMinute->setText( QString("%1").arg( nTimerLength/60, 2, 10, QChar( '0' ) ) );
        ui->ledTimerSecond->setText( QString("%1").arg( nTimerLength%60, 2, 10, QChar( '0' ) ) );
        ui->ledTimerExtMinute->setText( QString("%1").arg( nTimerExtLength/60, 2, 10, QChar( '0' ) ) );
        ui->ledTimerExtSecond->setText( QString("%1").arg( nTimerExtLength%60, 2, 10, QChar( '0' ) ) );
    }

    ui->chkProcessExtended->setChecked( m_bContinueExtended );

    if( m_nActionOnFinish == 1 )
    {
        ui->rbActionNothing->setChecked( true );
    }
    else if( m_nActionOnFinish == 2 )
    {
        ui->rbActionHideTimer->setChecked( true );
    }
    if( m_nActionOnFinish == 3 )
    {
        ui->rbActionMessageBox->setChecked( true );
        ui->ledMessage->setText( m_qsPopupMessage );
    }

    int nCurrentIndex   = ui->cmbLang->findText( QString("%1 (").arg(m_qsLang), Qt::MatchContains );

    ui->cmbLang->setCurrentIndex( nCurrentIndex );

    m_nTimer = startTimer( 500 );

    resize( obPref.value( "WindowPosition/Mainwindow_width", 941 ).toInt(),
            obPref.value( "WindowPosition/Mainwindow_height", 440 ).toInt() );
    move( obPref.value( "WindowPosition/Mainwindow_left", 100).toInt(),
          obPref.value( "WindowPosition/Mainwindow_top", 100).toInt() );
}

dlgMain::~dlgMain()
{
    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    obPref.setValue( "WindowPosition/Mainwindow_left", x() );
    obPref.setValue( "WindowPosition/Mainwindow_top", y() );
    obPref.setValue( "WindowPosition/Mainwindow_width", width() );
    obPref.setValue( "WindowPosition/Mainwindow_height", height() );

    if( m_bSaveTimerSettingsOnExit )
    {
        on_pbSaveActionValues_clicked();
    }

    delete ui;
}

void dlgMain::setTimerLength(int p_nTimerLength)
{
    ui->ledTimerMinute->setText( QString::number( p_nTimerLength/60 ) );
    ui->ledTimerSecond->setText( QString::number( p_nTimerLength%60 ) );
}

void dlgMain::setExtendedLength(int p_nExtendedLength)
{
    ui->ledTimerExtMinute->setText( QString::number( p_nExtendedLength/60 ) );
    ui->ledTimerExtSecond->setText( QString::number( p_nExtendedLength%60 ) );
}

void dlgMain::setTimerStart(int p_nTimerStart)
{
    m_nTimer = startTimer( p_nTimerStart*1000 );
}

bool dlgMain::checkTimeValues()
{
    if( atoi( ui->ledTimerMinute->text().toStdString().c_str() ) == 0 &&
        atoi( ui->ledTimerSecond->text().toStdString().c_str() ) == 0 )
    {
        QMessageBox::warning( this, tr("Warning"),
                              tr("Length of counter must be greater than zero!"));
        return false;
    }
    return true;
}

void dlgMain::timerEvent(QTimerEvent *)
{
    killTimer( m_nTimer );
    m_nTimer = 0;

    on_pbReset_clicked();

    if( m_bShowMainWindowOnStart )  { show();               }
    if( m_bShowTimerPanelOnStart )  { poDlgTimer->show();   }
    if( m_bStartTimerOnStart     )  { on_pbStart_clicked(); }
}

void dlgMain::mousePressEvent ( QMouseEvent *p_poEvent )
{
    m_bMousePressed = true;

    m_nMouseX = p_poEvent->pos().x();
    m_nMouseY = p_poEvent->pos().y();

    p_poEvent->accept();
}
void dlgMain::mouseReleaseEvent ( QMouseEvent *p_poEvent )
{
    m_bMousePressed = false;
    p_poEvent->accept();
}

void dlgMain::mouseMoveEvent ( QMouseEvent *p_poEvent )
{
    if( m_bMousePressed )
    {
        move( x() + p_poEvent->pos().x() - m_nMouseX,
              y() + p_poEvent->pos().y() - m_nMouseY );
    }
    p_poEvent->accept();
}

void dlgMain::_setActions()
{
    actionSettings = new QAction(tr("&Settings"), this);
    actionSettings->setIcon( QIcon( ":/settings.png" ) );
    connect( actionSettings, SIGNAL(triggered()), this, SLOT(slotSettings()) );

    actionShow = new QAction(tr("Show counter"), this);
    actionShow->setIcon( QIcon( ":/show.png" ) );
    connect( actionShow, SIGNAL(triggered()), this, SLOT(on_actionShow_triggered()) );

    actionHide = new QAction(tr("Hide counter"), this);
    actionHide->setIcon( QIcon( ":/minimize.png" ) );
    connect( actionHide, SIGNAL(triggered()), this, SLOT(on_pbHideCounter_clicked()) );

    actionStart = new QAction(tr("&Start"), this);
    actionStart->setIcon( QIcon( ":/start.png" ) );
    connect( actionStart, SIGNAL(triggered()), this, SLOT(on_pbStart_clicked()) );

    actionStop = new QAction(tr("S&top"), this);
    actionStop->setIcon( QIcon( ":/stop.png" ) );
    connect( actionStop, SIGNAL(triggered()), this, SLOT(on_pbStop_clicked()) );

    actionReset = new QAction(tr("&Reset"), this);
    actionReset->setIcon( QIcon( ":/reset.png" ) );
    connect( actionReset, SIGNAL(triggered()), this, SLOT(on_pbReset_clicked()) );

    actionExit = new QAction(tr("&Exit application"), this);
    actionExit->setIcon( QIcon( ":/exit.png" ) );
    connect( actionExit, SIGNAL(triggered()), qApp, SLOT(quit()) );
}

void dlgMain::_setMenu()
{
    trayIconMenu->addAction( actionSettings );
    trayIconMenu->addSeparator();

    trayIconMenu->addAction( actionShow );
    trayIconMenu->addAction( actionHide );
    trayIconMenu->addSeparator();

    trayIconMenu->addAction( actionStart );
    trayIconMenu->addAction( actionStop );
    trayIconMenu->addAction( actionReset );

    trayIconMenu->addSeparator();

    trayIconMenu->addAction( actionExit );

    trayIcon->setContextMenu(trayIconMenu);
}

void dlgMain::slotSettings()
{
    show();
}

void dlgMain::on_pbStart_clicked()
{
    if( checkTimeValues() )
    {
        poDlgTimer->show();
        emit signalStartCounter( ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt(),
                                 ui->ledTimerExtMinute->text().toInt()*60+ui->ledTimerExtSecond->text().toInt() );
    }
}

void dlgMain::on_pbStop_clicked()
{
    emit signalStopCounter();
}

void dlgMain::on_pbReset_clicked()
{
    emit signalResetCounter( ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt(),
                             ui->ledTimerExtMinute->text().toInt()*60+ui->ledTimerExtSecond->text().toInt() );
}

void dlgMain::on_ExtTimer_finished()
{
    if( ui->rbActionNothing->isChecked() )
    {
        //
    }
    else if( ui->rbActionHideTimer->isChecked() )
    {
        poDlgTimer->hide();
    }
    else if( ui->rbActionMessageBox->isChecked() )
    {
        QMessageBox::information( NULL, tr("Information"), ui->ledMessage->text() );
    }
    else
    {
        QMessageBox::information( NULL, tr("Information"), tr("Your time is over!") );
    }
}

void dlgMain::on_Timer_finished()
{
    if( ui->chkProcessExtended->isChecked() )
    {
        emit signalContinueCounter();
    }
    else
    {
        on_ExtTimer_finished();
    }
}

void dlgMain::on_actionShow_triggered()
{
    poDlgTimer->show();
}

void dlgMain::on_pbExit_clicked()
{
    qApp->quit();
}

void dlgMain::timerSet( int p_nTimerLength )
{
    ui->ledTimerMinute->setText( QString::number( p_nTimerLength/60 ) );
    ui->ledTimerSecond->setText( QString::number( p_nTimerLength%60 ) );
}


void dlgMain::on_ledTimerMinute_textEdited(const QString &/*arg1*/)
{
    emit signalSetTimer( ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt() );
}

void dlgMain::on_ledTimerSecond_textEdited(const QString &/*arg1*/)
{
    emit signalSetTimer( ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt() );
}

void dlgMain::on_ledTimerExtMinute_textEdited(const QString &/*arg1*/)
{
    emit signalSetExtTimer( ui->ledTimerExtMinute->text().toInt()*60+ui->ledTimerExtSecond->text().toInt() );
}

void dlgMain::on_ledTimerExtSecond_textEdited(const QString &/*arg1*/)
{
    emit signalSetExtTimer( ui->ledTimerExtMinute->text().toInt()*60+ui->ledTimerExtSecond->text().toInt() );
}

void dlgMain::on_actionSettings_changed()
{
    m_qsLang                    = ui->cmbLang->currentText().left(2);
    m_bShowMainWindowOnStart    = ui->chkShowMainWindow->isChecked();
    m_bShowTimerPanelOnStart    = ui->chkShowTimerWindow->isChecked();
    m_bStartTimerOnStart        = ui->chkStartTimerOnStart->isChecked();
    m_bUseSavedTimers           = ui->chkUseSavedTimes->isChecked();
    m_bSaveTimerSettingsOnExit  = ui->chkSaveTimerSettings->isChecked();

    if( m_bStartTimerOnStart )
    {
        ui->chkShowTimerWindow->setChecked( true );
        m_bShowTimerPanelOnStart = true;
    }
}

void dlgMain::on_actionTimeSettings_changed()
{
    m_bContinueExtended = ui->chkProcessExtended->isChecked();

    if( ui->rbActionNothing->isChecked() )
    {
        m_nActionOnFinish = 1;
    }
    else if( ui->rbActionHideTimer->isChecked() )
    {
        m_nActionOnFinish = 2;
    }
    else if( ui->rbActionMessageBox->isChecked() )
    {
        m_nActionOnFinish = 3;
        m_qsPopupMessage = ui->ledMessage->text();
    }
}

void dlgMain::on_pbSaveActionValues_clicked()
{
    on_actionTimeSettings_changed();

    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    int nTimerLength = ui->ledTimerMinute->text().toInt()*60 + ui->ledTimerSecond->text().toInt();
    int nTimerExtLength = ui->ledTimerExtMinute->text().toInt()*60 + ui->ledTimerExtSecond->text().toInt();

    obPref.setValue( "Timer/TimerLength",               nTimerLength );
    obPref.setValue( "Timer/TimerExtLength",            nTimerExtLength );
    obPref.setValue( "TimerAction/ContinueCountdown",   m_bContinueExtended );
    obPref.setValue( "TimerAction/ActionOnFinish",      m_nActionOnFinish );
    if( m_nActionOnFinish == 3 )
    {
        obPref.setValue( "TimerAction/ActionMessage", m_qsPopupMessage );
    }
}

void dlgMain::on_pbSaveSettings_clicked()
{
    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    obPref.setValue( "Lang",                    m_qsLang );
    obPref.setValue( "ShowMainWindowOnStart",   m_bShowMainWindowOnStart );
    obPref.setValue( "ShowTimerPanelOnStart",   m_bShowTimerPanelOnStart );
    obPref.setValue( "StartTimerOnStart",       m_bStartTimerOnStart );
    obPref.setValue( "UseSavedTimers",          m_bUseSavedTimers );
    obPref.setValue( "SaveTimerSettingsOnExit", m_bSaveTimerSettingsOnExit );
}

void dlgMain::on_pbHideCounter_clicked()
{
    poDlgTimer->hide();
}

void dlgMain::on_cmbLang_currentIndexChanged(const QString &arg1)
{
    if( !m_bReloadLanguage )
    {
        m_qsLang = arg1.left(2);
    }
}

void dlgMain::on_pbRetranslate_clicked()
{
    m_bReloadLanguage = true;

    apMainApp->removeTranslator( poTransApp );
    apMainApp->removeTranslator( poTransQT );

    poTransApp->load( QString("%1\\countdowntimer_%2.qm").arg( QDir::currentPath() ).arg(m_qsLang) );
    poTransQT->load( QString("%1\\qt_%2.qm").arg( QDir::currentPath() ).arg(m_qsLang) );

    apMainApp->installTranslator( poTransApp );
    apMainApp->installTranslator( poTransQT );

    ui->retranslateUi( this );

    int nCurrentIndex   = ui->cmbLang->findText( QString("%1 (").arg(m_qsLang), Qt::MatchContains );

    ui->cmbLang->setCurrentIndex( nCurrentIndex );

    m_bReloadLanguage = false;
}
