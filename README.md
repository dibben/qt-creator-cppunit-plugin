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

CppUnit requires the test to be added to a test suite so that it can be run.
Each new test needs, at minimum, the test case function itself and a separate declaration to
add it to the test suite. If the test itself is declared in a header and defined in a separate source (.cpp)
file this reuqires 3 additions (declaration, addition to test suite and definition) for each test case.

The main purpose of this plugin is to remove the tedium of adding a test case. When editing a test case class,
a new test case dialog can be opened with a shortcut (default Ctrl+Shift+T). A series of space separated test
functions can be entred and then the plugin in will
* Add a declaration to the test fixture header file
* Add the test to the test suite for the fixture
* Add an empty definition in the test source file

The original C++



##Installation

###From source
1. Set Environment variables (or modify paths.pri directly):
 - set `QTC_SOURCE` variable to path to Qt Creator source dir
 - set `QTC_BUILD` variable to  path to compiled Qt Creator dir

2. Compile plugin.


##More info
Additional information can be obtained [here](http://gres.biz/qtc-cppcheck/ "Homepage") (in russian)
