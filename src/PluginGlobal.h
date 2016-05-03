
#ifndef QTC_CPPUNIT_GLOBAL_H
#define QTC_CPPUNIT_GLOBAL_H

#include <qglobal.h>

#if defined(QTCCPPUNIT_LIBRARY)
#  define CPPHELPER_EXPORT Q_DECL_EXPORT
#else
#  define CPPHELPER_EXPORT Q_DECL_IMPORT
#endif

#endif
