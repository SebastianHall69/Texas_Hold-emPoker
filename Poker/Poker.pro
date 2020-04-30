TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        card.cpp \
        deck.cpp \
        main.cpp \
        hand.cpp \
        player.cpp

HEADERS += \
    card.h \
    deck.h \
    hand.h \
    player.h
