TEMPLATE = lib

QT += xml

TARGET = QtcCppunit

PROVIDER = DavidDibben


include(paths.pri)

DEFINES += QTCCPPUNIT_LIBRARY



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

###### If the plugin can be depended upon by other plugins, this code needs to be outsourced to
###### <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
###### plugin's sources.

QTC_PLUGIN_NAME = QtcCppunit

QTC_LIB_DEPENDS += \
    utils
QTC_PLUGIN_DEPENDS += \
    cppeditor \
    projectexplorer \
    cpptools \
    texteditor \
    coreplugin


QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
