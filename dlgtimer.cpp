
#include <QSettings>
#include <QDir>

#include "dlgtimer.h"
#include "ui_dlgtimer.h"

dlgTimer::dlgTimer(QWidget *parent) : QDialog(parent), ui(new Ui::dlgTimer)
{
    ui->setupUi(this);

    setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint );

    m_bMousePressed     = false;
    m_nTimerLength      = 0;
    m_nExtendedLength   = 0;
    m_nTimer            = 0;

    setStyleSheet( "background-color: rgb(0, 170, 0);" );

    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    m_pTimerMinute = new QSound( QString( "%1/timerbeep.wav" ).arg( QDir::currentPath() ) );

    move( obPref.value( "WindowPosition/TimerPanel_left", 0).toInt(),
          obPref.value( "WindowPosition/TimerPanel_top", 0).toInt() );
}

dlgTimer::~dlgTimer()
{
    delete m_pTimerMinute;
    delete ui;
}

void dlgTimer::counterStop()
{
    killTimer( m_nTimer );
    m_nTimer = 0;
    ui->lblSeparator->setFocus();
}

void dlgTimer::counterReset( int p_nTimerLength, int p_nExtendedLength )
{
    m_nTimerLength      = p_nTimerLength;
    m_nExtendedLength   = p_nExtendedLength;
    formatTimerString( m_nTimerLength );
}

void dlgTimer::counterStart( int p_nTimerLength, int p_nExtendedLength )
{
    if( m_nTimerLength == 0 && m_nExtendedLength == 0 )
    {
        m_nTimerLength      = p_nTimerLength;
        m_nExtendedLength   = p_nExtendedLength;
        formatTimerString( m_nTimerLength );
    }
    if( m_nTimer == 0 )
    {
        m_nTimer = startTimer( 1000 );
    }
}

void dlgTimer::counterContinue()
{
    formatTimerString( m_nExtendedLength );
    m_nTimer = startTimer( 1000 );
}

void dlgTimer::counterSetTimer(int p_nTimerLength)
{
    m_nTimerLength      = p_nTimerLength;
    formatTimerString( m_nTimerLength );
}

void dlgTimer::counterSetExtTimer(int p_nExtendedLength)
{
    m_nExtendedLength   = p_nExtendedLength;
}

void dlgTimer::keyPressEvent( QKeyEvent *p_poEvent )
{
    if( p_poEvent->key() == Qt::Key_Enter || p_poEvent->key() == Qt::Key_Return )
    {
        counterStart();
    }
    else if( p_poEvent->key() == Qt::Key_Space )
    {
        counterStop();
    }
    else if( p_poEvent->key() == Qt::Key_Escape )
    {
        emit timerReseted();
    }
    else if( p_poEvent->key() == Qt::Key_Backspace )
    {
        emit showSettings();
    }
}

void dlgTimer::timerEvent(QTimerEvent *)
{
    if( m_nTimerLength%60 == 1 && m_nTimerLength/60 > 0 )
    {
        m_pTimerMinute->play();
    }

    if( m_nTimerLength > 1 )
    {
        m_nTimerLength--;
        formatTimerString( m_nTimerLength );

    }
    else if( m_nTimerLength == 1 )
    {
        m_nTimerLength--;
        formatTimerString( m_nTimerLength );
        setStyleSheet( "background-color: rgb(170, 0, 0);" );
        killTimer( m_nTimer );
        m_nTimer = 0;
        emit timerFinished();
    }
    else
    {
        if( m_nExtendedLength > 0 )
        {
            m_nExtendedLength--;
            formatTimerString( m_nExtendedLength );
        }
        else
        {
            killTimer( m_nTimer );
            m_nTimer = 0;
            emit timerExtFinished();
        }
    }
}

void dlgTimer::mousePressEvent ( QMouseEvent *p_poEvent )
{
    m_bMousePressed = true;

    m_nMouseX = p_poEvent->pos().x();
    m_nMouseY = p_poEvent->pos().y();

    p_poEvent->accept();
}
void dlgTimer::mouseReleaseEvent ( QMouseEvent *p_poEvent )
{
    m_bMousePressed = false;

    QSettings   obPref( QString( "%1/countdowntimer.inf" ).arg( QDir::currentPath() ), QSettings::IniFormat );

    obPref.setValue( "WindowPosition/TimerPanel_left", x() );
    obPref.setValue( "WindowPosition/TimerPanel_top", y() );

    p_poEvent->accept();
}

void dlgTimer::mouseMoveEvent ( QMouseEvent *p_poEvent )
{
    if( m_bMousePressed )
    {
        move( x() + p_poEvent->pos().x() - m_nMouseX,
              y() + p_poEvent->pos().y() - m_nMouseY );
    }
    p_poEvent->accept();
}

void dlgTimer::mouseDoubleClickEvent ( QMouseEvent *p_poEvent )
{
    emit showSettings();
    p_poEvent->accept();
}

void dlgTimer::formatTimerString(int timer)
{
    if( m_nTimerLength > 0 )
    {
        if( m_nTimerLength > 59 )
        {
            setStyleSheet( "background-color: rgb(0, 170, 0);" );
        }
        else
        {
            setStyleSheet( "background-color: rgb(255, 128, 0);" );
        }
    }
    else
    {
        setStyleSheet( "background-color: rgb(170, 0, 0);" );
    }

    ui->ledTimerMinute->setText( QString("%1").arg( ( timer % 3600 ) / 60, 2, 10, QChar( '0' ) ) );
    ui->ledTimerSecond->setText( QString("%1").arg( ( timer % 3600 ) % 60, 2, 10, QChar( '0' ) ) );

    ui->lblSeparator->setFocus();
}

void dlgTimer::on_ledTimerMinute_returnPressed()
{
    counterStart();
}

void dlgTimer::on_ledTimerSecond_returnPressed()
{
    counterStart();
}

void dlgTimer::on_ledTimerMinute_textEdited(const QString &/*arg1*/)
{
    m_nTimerLength = ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt();
    emit timerSet( m_nTimerLength );
}

void dlgTimer::on_ledTimerSecond_textEdited(const QString &/*arg1*/)
{
    m_nTimerLength = ui->ledTimerMinute->text().toInt()*60+ui->ledTimerSecond->text().toInt();
    emit timerSet( m_nTimerLength );
}

