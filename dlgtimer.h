#ifndef DLGTIMER_H
#define DLGTIMER_H

#include <QDialog>
#include <QMouseEvent>
#include <QSound>

namespace Ui {
class dlgTimer;
}

class dlgTimer : public QDialog
{
    Q_OBJECT

public:
    explicit dlgTimer(QWidget *parent = 0);
    ~dlgTimer();

public slots:
    void counterStop();
    void counterReset(int p_nTimerLength, int p_nExtendedLength);
    void counterStart(int p_nTimerLength = 0 , int p_nExtendedLength = 0);
    void counterContinue();
    void counterSetTimer(int p_nTimerLength);
    void counterSetExtTimer(int p_nExtendedLength);

protected:
    void keyPressEvent ( QKeyEvent *p_poEvent );
    void timerEvent( QTimerEvent *p_poEvent );
    void mousePressEvent ( QMouseEvent *p_poEvent );
    void mouseReleaseEvent ( QMouseEvent *p_poEvent );
    void mouseMoveEvent ( QMouseEvent *p_poEvent );
    void mouseDoubleClickEvent ( QMouseEvent *p_poEvent );

signals:
    void timerFinished();
    void timerExtFinished();
    void timerReseted();
    void timerSet( int p_nTimer );
    void showSettings();

private slots:
    void on_ledTimerMinute_returnPressed();
    void on_ledTimerSecond_returnPressed();
    void on_ledTimerMinute_textEdited(const QString &arg1);
    void on_ledTimerSecond_textEdited(const QString &arg1);

private:
    Ui::dlgTimer *ui;

    QSound  *m_pTimerMinute;

    bool     m_bMousePressed;
    int      m_nMouseX;
    int      m_nMouseY;
    int      m_nTimer;
    int      m_nTimerLength;
    int      m_nExtendedLength;

    void formatTimerString( int timer );
};

#endif // DLGTIMER_H
