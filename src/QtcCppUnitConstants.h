
#ifndef QTC_CPP_UNIT_CONSTANTS_H
#define QTC_CPP_UNIT_CONSTANTS_H

namespace QtcCppunit {
namespace Constants {

const char * const CREATE_DEFINITION = "CppEditor.CreateDefinition";
const char * const CREATE_TEST = "CppEditor.CreateTest";
const char * const SWITCH_TEST_SOURCE = "CppEditor.SwitchTestSource";

// QSettings keys for use by the "New Class" wizards.
const char * const CPPHELPER_SETTINGSGROUP = "QtcCppUnit";
const char * const CPPHELPER_AUTHOR = "Author";

const char * const CPPHELPER_SETTINGS_ID = QT_TRANSLATE_NOOP("QtcCppUnit", "New Class Options");
const char * const CPPHELPER_SETTINGS_CATEGORY = QT_TRANSLATE_NOOP("QtcCppUnit", "JSOL");

// build parsers
const char * const BUILD_PARSER_CPPUNIT    = "BuildParser.CppUnit";

} // namespace Constants
} // namespace CppHelper
#endif // CPPHELPERCONSTANTS_H
