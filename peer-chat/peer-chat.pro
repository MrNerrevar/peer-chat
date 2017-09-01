#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T13:50:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT       += network


TARGET = peer-chat
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    identity.cpp \
    message.cpp \
    chatroom.cpp \
    signin.cpp \
    chatdialog.cpp \
    addfriend.cpp \
    signindetails.cpp \
    addfrienddetails.cpp \
    chathouse.cpp \
    peer.cpp \
    addchatroom.cpp \
    addchatroomdetails.cpp

HEADERS  += \
    identity.h \
    message.h \
    chatroom.h \
    signin.h \
    chatdialog.h \
    addfriend.h \
    signindetails.h \
    addfrienddetails.h \
    chathouse.h \
    peer.h \
    addchatroom.h \
    addchatroomdetails.h

FORMS    += \
    signin.ui \
    chatdialog.ui \
    addfriend.ui \
    addchatroom.ui
