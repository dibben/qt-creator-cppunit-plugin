
#include "CustomClassWizard.h"

#include "QtcCppUnitConstants.h"

#include <cppeditor/cppeditorconstants.h>
#include <cpptools/cpptoolsconstants.h>
#include <cpptools/cppmodelmanager.h>

#include <coreplugin/icore.h>

#include <utils/codegeneration.h>
#include <utils/newclasswidget.h>
#include <utils/qtcassert.h>

#include <utils/mimetypes/mimedatabase.h>

#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QSettings>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSpacerItem>
#include <QWizard>
#include <QCheckBox>
#include <QDate>

#include <QEvent>


using namespace QtcCppunit;
using namespace QtcCppunit::Internal;

// ========= ClassNamePage =========

ClassNamePage::ClassNamePage(bool isTestClass,QWidget *parent) :
    QWizardPage(parent),
	m_isValid(false),
	m_isTestClass(isTestClass)
{
    setTitle(tr("Enter class name"));
    setSubTitle(tr("The header and source file names will be derived from the class name"));

    m_newClassWidget = new Utils::NewClassWidget;
    // Order, set extensions first before suggested name is derived
	if (isTestClass) {
		m_newClassWidget->setBaseClassInputVisible(false);
	} else {
		m_newClassWidget->setBaseClassInputVisible(true);
		m_newClassWidget->setBaseClassChoices(QStringList() << QString()
				<< QLatin1String("QObject")
				<< QLatin1String("QWidget")
				<< QLatin1String("QMainWindow"));
		m_newClassWidget->setBaseClassEditable(true);
	}
    m_newClassWidget->setFormInputVisible(false);
    m_newClassWidget->setNamespacesEnabled(true);
    m_newClassWidget->setAllowDirectories(true);

    connect(m_newClassWidget, SIGNAL(validChanged()), this, SLOT(slotValidChanged()));

    QVBoxLayout *pageLayout = new QVBoxLayout(this);   
    pageLayout->addWidget(m_newClassWidget);
    QSpacerItem *vSpacer = new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::Expanding);
    pageLayout->addItem(vSpacer);
	QHBoxLayout *buttonLayout = new QHBoxLayout;
	pageLayout->addLayout(buttonLayout);
	m_warningLabel = new QLabel( tr("The class name should end in TestCase"), this);
	m_warningLabel->setHidden(true);
	buttonLayout->addWidget(m_warningLabel);
	QSpacerItem *hSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Ignored);
	buttonLayout->addItem(hSpacer);

	m_newClassWidget->installEventFilter(this);

    initParameters();
}

// Retrieve settings of CppTools plugin.
bool ClassNamePage::lowercaseHeaderFiles()
{
	QString lowerCaseSettingsKey = QLatin1String(CppTools::Constants::CPPTOOLS_SETTINGSGROUP);
	lowerCaseSettingsKey += QLatin1Char('/');
	lowerCaseSettingsKey += QLatin1String(CppTools::Constants::LOWERCASE_CPPFILES_KEY);
	const bool lowerCaseDefault = CppTools::Constants::lowerCaseFilesDefault;
	return Core::ICore::settings()->value(lowerCaseSettingsKey, QVariant(lowerCaseDefault)).toBool();
}

// Set up new class widget from settings
void ClassNamePage::initParameters()
{
	Utils::MimeDatabase mdb;
	m_newClassWidget->setHeaderExtension(mdb.mimeTypeForName(QLatin1String(CppTools::Constants::CPP_HEADER_MIMETYPE)).preferredSuffix());
	m_newClassWidget->setSourceExtension(mdb.mimeTypeForName(QLatin1String(CppTools::Constants::CPP_SOURCE_MIMETYPE)).preferredSuffix());
	m_newClassWidget->setLowerCaseFiles(lowercaseHeaderFiles());
}

/*!
	Pick up key presses so we can check for valid class names
*/
bool ClassNamePage::eventFilter(QObject* obj, QEvent* ev)
{
	if (ev->type() == QEvent::KeyRelease) {
		slotValidChanged();
	}
	return QWizardPage::eventFilter(obj, ev);
}

void ClassNamePage::slotValidChanged()
{
	bool validNow = m_newClassWidget->isValid();

    if (m_isTestClass && !m_newClassWidget->className().endsWith(QString::fromLatin1("TestCase"))) {
		m_warningLabel->setHidden(false);
		validNow = false;
	} else {
		m_warningLabel->setHidden(true);
	}

    if (m_isValid != validNow) {
        m_isValid = validNow;
        emit completeChanged();
    }
}

CppCustomClassWizardDialog::CppCustomClassWizardDialog(bool isTestClass, const Core::BaseFileWizardFactory* factory, QWidget *parent) :
		Core::BaseFileWizard(factory, QVariantMap(), parent),
		m_classNamePage(new ClassNamePage(isTestClass, this))
{

	if (isTestClass) {
		setWindowTitle(tr("Test Case Class Wizard"));
	} else {
		setWindowTitle(tr("C++ Class Wizard"));
	}
    addPage(m_classNamePage);
}

void CppCustomClassWizardDialog::setPath(const QString &path)
{
    m_classNamePage->newClassWidget()->setPath(path);
}

CppCustomClassWizardParameters  CppCustomClassWizardDialog::parameters() const
{
	CppCustomClassWizardParameters rc;
    const Utils::NewClassWidget *ncw = m_classNamePage->newClassWidget();
	rc.m_className = ncw->className();
	rc.m_headerFile = ncw->headerFileName();
	rc.m_sourceFile = ncw->sourceFileName();
	rc.m_baseClass = ncw->baseClassName();
	rc.m_path = ncw->path();
	rc.m_classType = ncw->classType();
    return rc;
}

// ========= CppCustomClassWizard =========

CppCustomClassWizard::CppCustomClassWizard(bool testClass)
{
	m_isTestClass = testClass;

	setCategory(QLatin1String("O.C++"));
	setDisplayCategory(tr("C++"));
	setId("A.Class");


	if (testClass) {
		setDisplayName(tr("Test Case Class"));
		setDescription(tr("Creates a CppUnit test case header and a source file for a new class."));
	} else {
		setDisplayName(tr("C++ Class"));
		setDescription(tr("Creates a custom header and a source file for a new class."));
	}

}

QString CppCustomClassWizard::sourceSuffix() const
{
	return preferredSuffix(QLatin1String(CppTools::Constants::CPP_SOURCE_MIMETYPE));
}

QString CppCustomClassWizard::headerSuffix() const
{
	return preferredSuffix(QLatin1String(CppTools::Constants::CPP_HEADER_MIMETYPE));
}

Core::BaseFileWizard *CppCustomClassWizard::create(QWidget *parent,
											const Core::WizardDialogParameters &wizardDialogParameters) const
{

	CppCustomClassWizardDialog *wizard = new CppCustomClassWizardDialog(m_isTestClass, this, parent);
	foreach (QWizardPage *p, wizard->extensionPages())
        wizard->addPage(p);
    wizard->setPath(wizardDialogParameters.defaultPath());
    return wizard;
}

Core::GeneratedFiles CppCustomClassWizard::generateFiles(const QWizard *w, QString *errorMessage) const
{
	const CppCustomClassWizardDialog *wizard = qobject_cast<const CppCustomClassWizardDialog *>(w);
	CppCustomClassWizardParameters params = wizard->parameters();

	const QString sourceFileName = Core::BaseFileWizardFactory::buildFileName(params.m_path, params.m_sourceFile, sourceSuffix());
	const QString headerFileName = Core::BaseFileWizardFactory::buildFileName(params.m_path, params.m_headerFile, headerSuffix());


	Core::GeneratedFile sourceFile(sourceFileName);
	sourceFile.setEditorId( (CppEditor::Constants::CPPEDITOR_ID));

	Core::GeneratedFile headerFile(headerFileName);
	headerFile.setEditorId((CppEditor::Constants::CPPEDITOR_ID));

	if (m_isTestClass){
		params.m_baseClass = QString::fromLatin1("CppUnit::TestFixture");
	}

    QString header, source;
    if (!generateHeaderAndSource(params, &header, &source)) {
        *errorMessage = tr("Error while generating file contents.");
        return Core::GeneratedFiles();
    }
    headerFile.setContents(header);
    sourceFile.setContents(source);
    return Core::GeneratedFiles() << headerFile << sourceFile;
}

QString headerGuard(const QString &file)
{
	const QFileInfo fi(file);
	QString rc = Utils::fileNameToCppIdentifier(fi.completeBaseName());

	for (int i = 2; i < rc.length() - 1; i++) {
		if (rc[i].isLower() && rc[i + 1].isUpper()) {
			rc.insert(i + 1, QLatin1Char('_'));
		}
	}
	rc = rc.toUpper();
	rc += QLatin1Char('_');
	rc += Utils::fileNameToCppIdentifier(fi.suffix()).toUpper();
	return rc;
}


QString CppCustomClassWizard::readFile(const QString& fileName, const QMap<QString, QString>& replacementMap)
{
	QFile file(fileName);
	file.open(QFile::ReadOnly);

    QString retStr = QString::fromLatin1(file.readAll());
    retStr.replace(QString::fromLatin1("\r"), QString::fromLatin1(""));

	QMap<QString, QString>::const_iterator it = replacementMap.begin();
	QMap<QString, QString>::const_iterator end = replacementMap.end();
	for (;it !=end; ++it) {
		retStr.replace(it.key(), it.value());
	}
	return retStr;
}


bool CppCustomClassWizard::generateHeaderAndSource(const CppCustomClassWizardParameters &params,
											 QString *header, QString *source) const
{
    // TODO:
    //  Quite a bit of this code has been copied from FormClassWizardParameters::generateCpp
    //  and is duplicated in the library wizard.
    //  Maybe more of it could be merged into Utils.

    const QString indent = QString(4, QLatin1Char(' '));


    // Do we have namespaces?
	QStringList namespaceList = params.m_className.split(QLatin1String("::"));
    if (namespaceList.empty()) // Paranoia!
        return false;

	const QString unqualifiedClassName = namespaceList.takeLast();
	const QString guard = headerGuard(params.m_headerFile);

	QMap<QString, QString> replacementMap;
    replacementMap[QString::fromLatin1("{{CLASS_NAME}}")] = unqualifiedClassName;
	replacementMap[QString::fromLatin1("{{DATE}}")] = QDate::currentDate().toString();
	replacementMap[QString::fromLatin1("{{FILE_NAME}}")] = params.m_headerFile;

    QString headerPreamble = readFile(QString::fromLatin1(":/resources/HeaderTemplate.txt"), replacementMap);
    QString classPreamble = readFile(QString::fromLatin1(":/resources/ClassHeaderTemplate.txt"), replacementMap);

	replacementMap[QString::fromLatin1("{{FILE_NAME}}")] = params.m_sourceFile;
    QString sourcePreamble = readFile(QString::fromLatin1(":/resources/SourceTemplate.txt"), replacementMap);
    QString sourcebottom = readFile(QString::fromLatin1(":/resources/SourceBriefTemplate.txt"), replacementMap);



    // == Header file ==
    QTextStream headerStr(header);
	headerStr << headerPreamble << "#ifndef " << guard
              << "\n#define " <<  guard << '\n';

    const QRegExp qtClassExpr(QLatin1String("^Q[A-Z3].+"));
    QTC_ASSERT(qtClassExpr.isValid(), /**/);
    // Determine parent QObject type for Qt types. Provide base
    // class in case the user did not specify one.
    QString parentQObjectClass;
    bool defineQObjectMacro = false;
	switch(params.m_classType) {
		case Utils::NewClassWidget::ClassInheritsQObject:
			parentQObjectClass = QLatin1String("QObject");
			defineQObjectMacro = true;
			break;
		case Utils::NewClassWidget::ClassInheritsQWidget:
			parentQObjectClass = QLatin1String("QWidget");
			defineQObjectMacro = true;
			break;
		default:
			break;
    }
	const QString baseClass = params.m_baseClass.isEmpty()
							  && params.m_classType != Utils::NewClassWidget::NoClassType ?
							  parentQObjectClass : params.m_baseClass;
    const bool superIsQtClass = qtClassExpr.exactMatch(baseClass);
    if (superIsQtClass) {
        headerStr << '\n';
        Utils::writeIncludeFileDirective(baseClass, true, headerStr);
    }
	if (m_isTestClass) {
		headerStr << '\n';
        Utils::writeIncludeFileDirective(QString::fromLatin1("cppunit/TestFixture.h"), true, headerStr);
        Utils::writeIncludeFileDirective(QString::fromLatin1("cppunit/extensions/HelperMacros.h"), true, headerStr);
	}

    const QString namespaceIndent = Utils::writeOpeningNameSpaces(namespaceList, QString(), headerStr);

    // Class declaration
	headerStr << '\n';
	headerStr << classPreamble;
    headerStr << '\n';
    headerStr << namespaceIndent << "class " << unqualifiedClassName;
    if (!baseClass.isEmpty())
        headerStr << " : public " << baseClass << "\n";
    else
        headerStr << "\n";
    headerStr << namespaceIndent << "{\n";
	if (defineQObjectMacro) {
        headerStr << namespaceIndent << "Q_OBJECT\n";
	}
	if (m_isTestClass) {
		headerStr << namespaceIndent << "CPPUNIT_TEST_SUITE( " << unqualifiedClassName << " );\n";
		headerStr << namespaceIndent << indent << "CPPUNIT_TEST( test );\n";
		headerStr << namespaceIndent << "CPPUNIT_TEST_SUITE_END();\n";
	}

	headerStr << namespaceIndent << "public:\n";
    // Constructor

	if (m_isTestClass) {
		headerStr << namespaceIndent << indent << "void    setUp();\n";
		headerStr << namespaceIndent << indent << "void    tearDown();\n";
		headerStr << namespaceIndent << indent << "\n";
		headerStr << namespaceIndent << indent << "void    test();\n\n";
	} else {
		headerStr << namespaceIndent << indent;
		if (parentQObjectClass.isEmpty()) {
			headerStr << unqualifiedClassName << "();\n";
		} else {
			headerStr << "explicit " << unqualifiedClassName << '(' << parentQObjectClass
					<< " *parent = 0);\n";
		}
	}

	if (defineQObjectMacro) {
        headerStr << '\n' << namespaceIndent << "signals:\n\n" << namespaceIndent << "public slots:\n\n";
	}

    headerStr << namespaceIndent << "};\n";

    Utils::writeClosingNameSpaces(namespaceList, QString(), headerStr);

    headerStr << '\n';
	headerStr << "#endif\n";


    // == Source file ==
    QTextStream sourceStr(source);
	sourceStr << sourcePreamble;
	Utils::writeIncludeFileDirective(params.m_headerFile, false, sourceStr);

	Utils::writeOpeningNameSpaces(namespaceList, QString(), sourceStr);

	if (m_isTestClass) {
		sourceStr << '\n';

		sourceStr << namespaceIndent << "// Registers the fixture into the 'registry'\n";
		sourceStr << namespaceIndent << "CPPUNIT_TEST_SUITE_REGISTRATION( " << unqualifiedClassName << " );\n";
		sourceStr << "\n\n";

		sourceStr << namespaceIndent << "void " << unqualifiedClassName << "::setUp()\n";
		sourceStr << namespaceIndent << "{\n" << namespaceIndent << "}\n\n";
		sourceStr << namespaceIndent << "void " << unqualifiedClassName << "::tearDown()\n";
		sourceStr << namespaceIndent << "{\n" << namespaceIndent << "}\n\n";
		sourceStr << namespaceIndent << "void " << unqualifiedClassName << "::test()\n";
		sourceStr << namespaceIndent << "{\n" << namespaceIndent << "}\n";

	} else {
		// Constructor
		sourceStr << sourcebottom;

		sourceStr << '\n' << namespaceIndent ;
		if (parentQObjectClass.isEmpty()) {
			sourceStr << unqualifiedClassName << "::" << unqualifiedClassName << "()\n";
		} else {
			sourceStr << unqualifiedClassName << "::" << unqualifiedClassName
					  << '(' << parentQObjectClass << " *parent) :\n"
					  << namespaceIndent << indent << baseClass << "(parent)\n";
		}

		sourceStr << namespaceIndent << "{\n" << namespaceIndent << "}\n";
	}

    Utils::writeClosingNameSpaces(namespaceList, QString(), sourceStr);
    return true;
}

