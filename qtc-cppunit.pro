

DEFINES += QTCCPPUNIT_LIBRARY

# Qt Creator linking

## uncomment to build plugin into user config directory
## <localappdata>/plugins/<ideversion>
##    where <localappdata> is e.g.
##    "%LOCALAPPDATA%\Nokia\qtcreator" on Windows Vista and later
##    "$XDG_DATA_HOME/Nokia/qtcreator" or "~/.local/share/Nokia/qtcreator" on Linux
##    "~/Library/Application Support/Nokia/Qt Creator" on Mac
#USE_USER_DESTDIR = yes

## set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
isEmpty(QTCREATOR_SOURCES):QTCREATOR_SOURCES=/home/dibben/Develop/QtCreator/qt-creator

## set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)
isEmpty(IDE_BUILD_TREE):IDE_BUILD_TREE=/home/dibben/Develop/QtCreator/qt-creator

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)


TEMPLATE = lib

QT += xml

TARGET = QtcCppunit

PROVIDER = DavidDibben

include(paths.pri)



HEADERS += \
    src/TestNameDialog.h \
    src/QtcCppUnitPlugin.h \
    src/PluginGlobal.h \
    src/QtcCppUnitConstants.h \
    src/CustomClassWizard.h

SOURCES += \
    src/TestNameDialog.cpp \
    src/QtcCppUnitPlugin.cpp \
    src/CustomClassWizard.cpp

OTHER_FILES += LICENSE \
    README.md \
    QtcCppunit.json.in

RESOURCES += qtc-cppunit.qrc

FORMS += \
    src/TestNameDialog.ui



LIBS += -L$$IDE_PLUGIN_PATH/QtProject


