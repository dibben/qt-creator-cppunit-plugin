
#include "QtcCppUnitPlugin.h"

#include "QtcCppUnitConstants.h"
#include "CustomClassWizard.h"
#include "TestNameDialog.h"


#include <cppeditor/cppeditorconstants.h>
#include <cpptools/cpptoolsconstants.h>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/command.h>

#include <cpptools/cppmodelmanager.h>
#include <cpptools/cpptoolsreuse.h>

#include <coreplugin/icore.h>
#include <coreplugin/editormanager/editormanager.h>

#include <cplusplus/Overview.h>
#include <cplusplus/Scope.h>
#include <cplusplus/Symbols.h>
#include <cplusplus/Names.h>

#include <utils/mimetypes/mimedatabase.h>

#include <QStringList>
#include <QMenu>
#include <QAction>
#include <QtPlugin>
#include <QKeySequence>
#include <QObject>
#include <QDir>
#include <QMessageBox>

using namespace QtcCppunit;
using namespace QtcCppunit::Internal;
using namespace CPlusPlus;


QtcCppunitPlugin::QtcCppunitPlugin()
{
}

QtcCppunitPlugin::~QtcCppunitPlugin()
{
}

bool QtcCppunitPlugin::initialize(const QStringList &arguments, QString *error_message)
{
    Q_UNUSED(arguments);
    Q_UNUSED(error_message);

	Core::ActionManager *am = Core::ActionManager::instance();

	Core::IWizardFactory::registerFactoryCreator(
				[this]() -> QList<Core::IWizardFactory *> {
					Core::IWizardFactory *wizard = new CppCustomClassWizard(false);
					wizard->setId("C.CppUnitClass");
					return QList<Core::IWizardFactory *>() << wizard;
				});


	Core::IWizardFactory::registerFactoryCreator(
				[this]() -> QList<Core::IWizardFactory *> {
					Core::IWizardFactory *wizard = new CppCustomClassWizard(true);
					wizard->setId("C.CppUnitTestClass");
					return QList<Core::IWizardFactory *>() << wizard;
				});
	
	Core::Context context(CppEditor::Constants::CPPEDITOR_ID);

    QAction* createTest = new QAction(tr("Create Test"),  this);
    Core::Command *testCmd = am->registerAction(createTest, Constants::CREATE_TEST, context);
	testCmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Shift+T")));
    connect(createTest, SIGNAL(triggered()), this, SLOT(createTest()));
    am->actionContainer(CppTools::Constants::M_TOOLS_CPP)->addAction(testCmd);

	QAction* switchTest = new QAction(tr("Switch Test/Source"),  this);
	Core::Command *switchCmd = am->registerAction(switchTest, Constants::SWITCH_TEST_SOURCE, context);
	switchCmd->setDefaultKeySequence(QKeySequence(tr("Shift+F4")));
	connect(switchTest, SIGNAL(triggered()), this, SLOT(switchTest()));
	am->actionContainer(CppTools::Constants::M_TOOLS_CPP)->addAction(switchCmd);

    return true;
}

void QtcCppunitPlugin::extensionsInitialized()
{

}


namespace {

/*!
   \brief Return a list of the scope names for a symbol
*/
QStringList scopesForSymbol(const Symbol* symbol)
{
	if (symbol == 0) return QStringList();

	QStringList scopes;

	const Scope *scope = symbol->enclosingScope();
	for (; scope; scope = scope->enclosingScope()) {
		const Symbol *owner = scope;

		if (owner && owner->name() && ! scope->isEnum()) {
			const Name *name = owner->name();
			Overview overview;
			overview.showArgumentNames = false;
			overview.showReturnTypes = false;
			scopes.prepend(overview.prettyName(name));
		}
	}
	return scopes;
}

/*!
   \brief Returns the current symbol in the open editor.
*/
Symbol* currentSymbol(Core::IEditor* editor)
{
	if (editor == 0) return 0;

	int line = editor->currentLine();
	int column = editor->currentColumn();

	CppTools::CppModelManager *modelManager = CppTools::CppModelManager::instance();

	if (!modelManager) {
		QMessageBox::information(0, QLatin1String("Error"), QLatin1String("Model manager is null"));
		return 0;
	}

	const Snapshot snapshot = modelManager->snapshot();
	Document::Ptr doc = snapshot.document(editor->document()->filePath());
	if (!doc) {
		QMessageBox::information(0, QLatin1String("Error"), QLatin1String("doc manager is null"));
		return 0;
	}

	return doc->lastVisibleSymbolAt(line, column);
}

/*!
   \brief If the current file is a CPP header then switch to the matching source file
*/
void switchToSource(Core::IEditor *editor)
{
	if (editor == 0) return;
	// Switch to source file
	QFileInfo fi(editor->document()->filePath().toFileInfo());

	Utils::MimeDatabase mdb;
	const QString typeName = mdb.mimeTypeForFile(fi).name();
	if (typeName == QLatin1String(CppTools::Constants::C_HEADER_MIMETYPE) ||
		typeName == QLatin1String(CppTools::Constants::CPP_HEADER_MIMETYPE)) {
		CppTools::switchHeaderSource();
	}
}


/*!
   \brief If the current file is a CPP source file then switch to the matching header file
*/

void switchToHeader(Core::IEditor *editor)
{
	if (editor == 0) return;

	QFileInfo fi(editor->document()->filePath().toFileInfo());
	Utils::MimeDatabase mdb;
	const QString typeName = mdb.mimeTypeForFile(fi).name();

	if (typeName == QLatin1String(CppTools::Constants::C_SOURCE_MIMETYPE) ||
		typeName == QLatin1String(CppTools::Constants::CPP_SOURCE_MIMETYPE)) {
		CppTools::switchHeaderSource();
	}
}

}

/*!
   \brief Create a new test function

   Creates
	* a function declaration
	* a function body
	* adds function to the test suite
*/
void QtcCppunitPlugin::createTest()
{
	const Core::ICore *core = Core::ICore::instance();

	Core::IEditor *editor = Core::EditorManager::currentEditor();
	QWidget *mainWindow = core->mainWindow();

	QString testHeader = editor->document()->filePath().toString();
	QFileInfo fi(testHeader);
	QString baseName = fi.baseName();
	if (!baseName.contains(QLatin1String("TestCase"))) {
		testHeader = fi.dir().filePath(baseName + QLatin1String("TestCase.h"));
		if (!QFileInfo(testHeader).exists()) {
			testHeader = fi.dir().filePath(QLatin1String("tests/") + baseName + QLatin1String("TestCase.h"));
		}
		if (!QFileInfo(testHeader).exists()) {
			QMessageBox::warning(mainWindow, QLatin1String("Create Test"),
											 QString::fromLatin1("Cannot find test class for %1").arg(baseName));
			return;
		}
	}

	TestNameDialog nameDialog(mainWindow);
	if (nameDialog.exec() != QDialog::Accepted) return;

	QStringList functions = nameDialog.testFunctions().split(QLatin1String(" "), QString::SkipEmptyParts);
	if (functions.size() == 0) return;

	if (testHeader != editor->document()->filePath().toString()) {
		Core::EditorManager::openEditorAt(testHeader, 0, 0, CppEditor::Constants::CPPEDITOR_ID);
	}

	QString functionDeclaration;
	QString testDeclaration;
	QString functionDefinition;
	foreach (QString func, functions) {
		if (!func.startsWith(QLatin1String("test"))) func = QLatin1String("test") + func;
		functionDeclaration += QString::fromLatin1("\n\tvoid\t%1();").arg(func);
		testDeclaration +=  QString::fromLatin1("\n\tCPPUNIT_TEST( %1 );").arg(func);
		QString functionName = QString::fromLatin1("void {{CLASSNAME}}::%1()").arg(func);
		functionDefinition +=  QString::fromLatin1("\n\n%1\n{\n\n}").arg(functionName);
	}


	editor = Core::EditorManager::currentEditor();
	switchToHeader(editor);

	editor = Core::EditorManager::currentEditor();

	QString className;

	// Write the function in the editor
	TextEditor::TextEditorWidget *editorWidget = qobject_cast<TextEditor::TextEditorWidget*>(
			Core::EditorManager::currentEditor()->widget());
	if (editorWidget) {
		editorWidget->moveCursor(QTextCursor::End);
		editorWidget->find(QLatin1String("test"), QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
		editorWidget->moveCursor(QTextCursor::EndOfLine);

		Symbol* lastSymbol = currentSymbol(editor);
		QStringList scopes = scopesForSymbol(lastSymbol);
		className = scopes.join(QLatin1String("::"));

		editorWidget->insertPlainText(functionDeclaration);
		editorWidget->moveCursor(QTextCursor::Up);

		editorWidget->find(QLatin1String("CPPUNIT"), QTextDocument::FindBackward | QTextDocument::FindCaseSensitively);
		editorWidget->moveCursor(QTextCursor::Up);
		editorWidget->moveCursor(QTextCursor::EndOfLine);
		editorWidget->insertPlainText(testDeclaration);
	}

	functionDefinition.replace(QLatin1String("{{CLASSNAME}}"), className);

	editor = Core::EditorManager::currentEditor();
	switchToSource(editor);

	editorWidget = qobject_cast<TextEditor::TextEditorWidget*>(
				Core::EditorManager::currentEditor()->widget());
	if (editorWidget) {
		editorWidget->moveCursor(QTextCursor::End);
		editorWidget->find(QLatin1String("}"), QTextDocument::FindBackward);
		editorWidget->moveCursor(QTextCursor::EndOfLine);
		editorWidget->insertPlainText(functionDefinition);
		editorWidget->moveCursor(QTextCursor::Up);
	}
}



void QtcCppunitPlugin::switchTest()
{
	const Core::ICore *core = Core::ICore::instance();

	Core::IEditor *editor = Core::EditorManager::currentEditor();
	QWidget *mainWindow = core->mainWindow();

	QString testHeader = editor->document()->filePath().toString();
	QFileInfo fi(testHeader);
	QString baseName = fi.baseName();
	if (baseName.contains(QLatin1String("TestCase"))) {
		QString source = baseName;
		source.replace(QLatin1String("TestCase"), QLatin1String(""));
		testHeader = fi.dir().filePath(source + QLatin1String(".cpp"));
		if (!QFileInfo(testHeader).exists()) {
			testHeader = fi.dir().filePath(QLatin1String("../") + source+ QLatin1String(".cpp"));
		}
		if (!QFileInfo(testHeader).exists()) {
			QMessageBox::warning(mainWindow, QLatin1String("Switch Test"),
								 QString::fromLatin1("Cannot find source class for %1").arg(baseName));
			return;
		}
	} else {
		testHeader = fi.dir().filePath(baseName+QLatin1String("TestCase.cpp"));
		if (!QFileInfo(testHeader).exists()) {
			testHeader = fi.dir().filePath(QLatin1String("tests/") + baseName+ QLatin1String("TestCase.cpp"));
		}
		if (!QFileInfo(testHeader).exists()) {
			QMessageBox::warning(mainWindow, QLatin1String("Switch Test"), QString::fromLatin1("Cannot find test class for %1").arg(baseName));
			return;
		}
	}

	if (testHeader != editor->document()->filePath().toString()) {
		Core::EditorManager::openEditorAt(testHeader, 0,	0, CppEditor::Constants::CPPEDITOR_ID);
	}
}
