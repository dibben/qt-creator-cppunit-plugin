#Qt Creator CppUnit Helper Plugin

##Introduction

This Qt Creator plugin adds some helper functions for using CppUnit with Qt Creator.

CppUnit is a unit test framework based on the original JUnit. The original cppunit library
can be found [here](https://sourceforge.net/projects/cppunit/ "Sourceforge").
A newer freedesktop.org  fork can be found [here](https://www.freedesktop.org/wiki/Software/cppunit/ "freedesktop.org")


##Features

* Custom class wizard for creating test case class with necessary boilerplate code.
* Function to add a new test to a test class
* Shortcut to switch between testcase and main class

##Background

CppUnit requires the test case to be added to a test suite so that it can be run.
Each new test needs, at minimum, the test case function itself and a separate declaration to
add it to the test suite. If the test itself is declared in a header and defined in a separate source (.cpp)
file this reuqires 3 additions (declaration, addition to test suite and definition) for each test case.

The main purpose of this plugin is to remove the tedium of adding a test case. When editing a test case class,
a new test case dialog can be opened with a shortcut (default Ctrl+Shift+T). A series of space separated test
functions can be entred and then the plugin in will

* Add a declaration to the test fixture header file
* Add the test to the test suite for the fixture
* Add an empty definition in the test source file

The plugin also provides a shortcut to switch to the test case class. This assumes that

* tests are in a subfolder called tests
* the test case file has the same name as the current file with TestCase appended

So if edition a file MyClass.cpp the shortcut will look for a test file MyClassTestCase.cpp.
Although using a test-fixture-per-class rather than testing scenarios is something of a
TDD anti-pattern this can still be useful.

Using this plugin the TDD workflow is

1. Shift+F4 to switch to test case
2. Ctrl+Shift+T to add new test case
3. Code the test body
4. Build - and automatically run the tests (see tips)
5. Shift+F4 to switch back to the source
6. Implement the code.
7. Build and run tests again

##Tips

* Adding the test executable as a custom build step means the test are run automatically after each build
* Adding custom text snippets in QtCreator makes it easier to add the insert macros
  - eg ```CPPUNIT_ASSERT_EQUAL_MESSAGE("$message$", $expected$, $actual$);```
* Custom output printers for CppUnit allow MSVC/G++ style messages that can be automatially picked up by QtCreator as issues.
  - See [cppunit-utilities](https://github.com/dibben/cppunit-utilities "Github")


##Installation

###From source
1. Set Environment variables (or modify paths.pri directly):
 - set `QTC_SOURCE` variable to path to Qt Creator source dir
 - set `QTC_BUILD` variable to  path to compiled Qt Creator dir

2. Compile plugin.

## More Information

More information on using the CppUnit with Qt creator is available [here](http://www.codegardening.com/2016/05/using-cppunit-with-qtcreator.html "Using CppUnit with QtCreator")


