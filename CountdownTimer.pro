
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT          += core gui multimedia
CONFIG      += qt
RESOURCES   += countdowntimer.qrc
RC_FILE      = countdowntimer.rc
TEMPLATE     = app
DESTDIR      = ..
TRANSLATIONS = countdowntimer_hu.ts \
               countdowntimer_fr.ts
TARGET       = CountdownTimer

HEADERS     += dlgmain.h \
               dlgtimer.h

SOURCES     += main.cpp\
               dlgmain.cpp \
               dlgtimer.cpp

FORMS       += dlgmain.ui \
               dlgtimer.ui
