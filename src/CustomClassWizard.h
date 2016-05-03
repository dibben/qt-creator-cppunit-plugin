

#ifndef CUSTOM_CLASS_WIZARD_H
#define CUSTOM_CLASS_WIZARD_H

#include <coreplugin/basefilewizard.h>
#include <coreplugin/basefilewizardfactory.h>

#include <utils/newclasswidget.h>

#include <utils/wizard.h>
#include <QStringList>
#include <QWizardPage>
#include <QWizard>
#include <QLabel>


namespace QtcCppunit {
namespace Internal {

class ClassNamePage : public QWizardPage
{
    Q_OBJECT

public:
	explicit ClassNamePage(bool isTestClass, QWidget *parent = 0);

    bool isComplete() const { return m_isValid; }
	Utils::NewClassWidget* newClassWidget() const { return m_newClassWidget; }

	bool	eventFilter(QObject* obj, QEvent* ev);

private slots:

	void	slotValidChanged();

private:

	void	initParameters();
	bool	lowercaseHeaderFiles();

	Utils::NewClassWidget*	m_newClassWidget;
	QLabel*					m_warningLabel;
	bool					m_isValid;
	bool					m_isTestClass;

};


struct CppCustomClassWizardParameters
{
	QString m_className;
	QString m_headerFile;
	QString m_sourceFile;
	QString m_baseClass;
	QString m_path;
	Utils::NewClassWidget::ClassType m_classType;
};

class CppCustomClassWizardDialog : public Core::BaseFileWizard
{
    Q_OBJECT
	Q_DISABLE_COPY(CppCustomClassWizardDialog)
public:
	explicit CppCustomClassWizardDialog(bool isTestClass, const Core::BaseFileWizardFactory *factory, QWidget *parent = 0);

    void setPath(const QString &path);
	CppCustomClassWizardParameters parameters() const;

private:
	ClassNamePage*	m_classNamePage;
};


class CppCustomClassWizard : public Core::BaseFileWizardFactory
{
    Q_OBJECT
public:

	explicit CppCustomClassWizard(bool testClass);

protected:

	Core::BaseFileWizard *create(QWidget *parent, const Core::WizardDialogParameters &parameters) const;

	Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;

    QString sourceSuffix() const;
    QString headerSuffix() const;

private:

	bool generateHeaderAndSource(const CppCustomClassWizardParameters &params,
										QString *header, QString *source) const;

	static QString	readFile(const QString& fileName, const QMap<QString, QString>& replacementMap);

	bool	m_isTestClass;
};

} // namespace Internal
} // namespace QtcCppunit

#endif
