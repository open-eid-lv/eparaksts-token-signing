TEMPLATE = app
CONFIG += console c++11 link_pkgconfig
CONFIG -= app_bundle
QT += widgets network
isEmpty(VERSION):VERSION=1.0.0.0
PKGCONFIG += openssl
INCLUDEPATH += ../host-shared
LIBS += -ldl
DEFINES += VERSION=\\\"$$VERSION\\\"
SOURCES += \
    ../host-shared/Labels.cpp \
    ../host-shared/Logger.cpp \
    chrome-host.cpp
HEADERS += *.h ../host-shared/*.h
RESOURCES += eparaksts-chrome-extension.qrc
target.path = /usr/bin
hostconf.path = /etc/opt/chrome/native-messaging-hosts
hostconf.files += lv.eparaksts.eparaksts_chrome_extension.json
extension.path = /opt/google/chrome/extensions
extension.files += ../dfablgdffinpaeiilgjpebchbacimpoa.json
INSTALLS += target hostconf extension
