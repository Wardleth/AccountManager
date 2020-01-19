include(gtest_dependency.pri)

TARGET = AccountManagerTests
TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

HEADERS += \    
    tst_caisseepargnestatementreader.h

INCLUDEPATH += \
    ../../src/app

SOURCES += \
    main.cpp \
    ../../src/app/caisseepargnestatementreader.cpp
