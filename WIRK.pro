#-------------------------------------------------
#
# Project created by QtCreator 2013-04-28T16:07:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WIRK
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ext_lib/communi/irctextformat.cpp \
    ext_lib/communi/ircsessioninfo.cpp \
    ext_lib/communi/ircsession.cpp \
    ext_lib/communi/ircsender.cpp \
    ext_lib/communi/ircprotocol.cpp \
    ext_lib/communi/ircpalette.cpp \
    ext_lib/communi/ircmessagedecoder.cpp \
    ext_lib/communi/ircmessagedecoder_uchardet.cpp \
    ext_lib/communi/ircmessage.cpp \
    ext_lib/communi/ircmessage_p.cpp \
    ext_lib/communi/irccommand.cpp \
    ext_lib/communi/irc.cpp \
    ext_lib/mozilla/rdf_utils.c \
    ext_lib/uchardet/uchardet.cpp \
    ext_lib/uchardet/nsUTF8Prober.cpp \
    ext_lib/uchardet/nsUniversalDetector.cpp \
    ext_lib/uchardet/nsSJISProber.cpp \
    ext_lib/uchardet/nsSBCSGroupProber.cpp \
    ext_lib/uchardet/nsSBCharSetProber.cpp \
    ext_lib/uchardet/nsMBCSSM.cpp \
    ext_lib/uchardet/nsMBCSGroupProber.cpp \
    ext_lib/uchardet/nsLatin1Prober.cpp \
    ext_lib/uchardet/nsHebrewProber.cpp \
    ext_lib/uchardet/nsGB2312Prober.cpp \
    ext_lib/uchardet/nsEUCTWProber.cpp \
    ext_lib/uchardet/nsEUCKRProber.cpp \
    ext_lib/uchardet/nsEUCJPProber.cpp \
    ext_lib/uchardet/nsEscSM.cpp \
    ext_lib/uchardet/nsEscCharsetProber.cpp \
    ext_lib/uchardet/nsCharSetProber.cpp \
    ext_lib/uchardet/nsBig5Prober.cpp \
    ext_lib/uchardet/LangThaiModel.cpp \
    ext_lib/uchardet/LangHungarianModel.cpp \
    ext_lib/uchardet/LangHebrewModel.cpp \
    ext_lib/uchardet/LangGreekModel.cpp \
    ext_lib/uchardet/LangCyrillicModel.cpp \
    ext_lib/uchardet/LangBulgarianModel.cpp \
    ext_lib/uchardet/JpCntx.cpp \
    ext_lib/uchardet/CharDistribution.cpp \
    models/session.cpp \
    models/server.cpp \
    models/channel.cpp \
    models/user.cpp \
    utilities/messageparser.cpp \
    utilities/commandparser.cpp \
    viewmodels/animationviewmodel.cpp \
    preferences.cpp \
    models/messagehistory.cpp \
    textbox.cpp \
    newserver.cpp \
    utilities/preferenceshelper.cpp

HEADERS  += mainwindow.h \
    ext_lib/communi/irctextformat.h \
    ext_lib/communi/ircsessioninfo.h \
    ext_lib/communi/ircsession.h \
    ext_lib/communi/ircsession_p.h \
    ext_lib/communi/ircsender.h \
    ext_lib/communi/ircprotocol.h \
    ext_lib/communi/ircpalette.h \
    ext_lib/communi/ircmessagedecoder_p.h \
    ext_lib/communi/ircmessage.h \
    ext_lib/communi/ircmessage_p.h \
    ext_lib/communi/irccommand.h \
    ext_lib/communi/irc.h \
    ext_lib/uchardet/uchardet.h \
    ext_lib/uchardet/prmem.h \
    ext_lib/uchardet/nsUTF8Prober.h \
    ext_lib/uchardet/nsUniversalDetector.h \
    ext_lib/uchardet/nsSJISProber.h \
    ext_lib/uchardet/nsSBCSGroupProber.h \
    ext_lib/uchardet/nsSBCharSetProber.h \
    ext_lib/uchardet/nsPkgInt.h \
    ext_lib/uchardet/nsMBCSGroupProber.h \
    ext_lib/uchardet/nsLatin1Prober.h \
    ext_lib/uchardet/nsHebrewProber.h \
    ext_lib/uchardet/nsGB2312Prober.h \
    ext_lib/uchardet/nsEUCTWProber.h \
    ext_lib/uchardet/nsEUCKRProber.h \
    ext_lib/uchardet/nsEUCJPProber.h \
    ext_lib/uchardet/nsEscCharsetProber.h \
    ext_lib/uchardet/nscore.h \
    ext_lib/uchardet/nsCodingStateMachine.h \
    ext_lib/uchardet/nsCharSetProber.h \
    ext_lib/uchardet/nsBig5Prober.h \
    ext_lib/uchardet/JpCntx.h \
    ext_lib/uchardet/CharDistribution.h \
    models/session.h \
    models/server.h \
    models/channel.h \
    models/user.h \
    utilities/messageparser.h \
    utilities/commandparser.h \
    viewmodels/animationviewmodel.h \
    preferences.h \
    models/messagehistory.h \
    textbox.h \
    newserver.h \
    utilities/preferenceshelper.h

FORMS    += mainwindow.ui \
    preferences.ui \
    newserver.ui

OTHER_FILES += \
    ext_lib/uchardet/JISFreq.tab \
    ext_lib/uchardet/GB2312Freq.tab \
    ext_lib/uchardet/EUCTWFreq.tab \
    ext_lib/uchardet/EUCKRFreq.tab \
    ext_lib/uchardet/Big5Freq.tab \
    assets/icon.icns \
    assets/icon.ico \
    assets/icon.rc

INCLUDEPATH += ext_lib/communi \
                ext_lib/mozilla \
                ext_lib/uchardet \
                models \
                utilities \
                viewmodels

QT += network \
        webkit \
        webkitwidgets

ICON = assets/icon.icns

RESOURCES += \
    wirk.qrc

RC_FILE = assets/icon.rc

VERSION = 0.3.0

DEFINES += VER=\\\"$${VERSION}.$$system(git rev-parse --short HEAD)\\\"
