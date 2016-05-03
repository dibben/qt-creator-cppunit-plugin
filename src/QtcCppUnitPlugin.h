
#ifndef QTC_CPPUNIT_PLUGIN_H
#define QTC_CPPUNIT_PLUGIN_H

#include "extensionsystem/iplugin.h"

namespace QtcCppunit {
namespace Internal {

class QtcCppunitPlugin  : public ExtensionSystem::IPlugin
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtcCppunit.json")
public:
	QtcCppunitPlugin();
	~QtcCppunitPlugin();

    bool initialize(const QStringList &arguments, QString *error_message);
    void extensionsInitialized();

private slots:

    void createTest();
	void switchTest();


};

} // namespace Internal
} // namespace QtcCppunit

#endif
