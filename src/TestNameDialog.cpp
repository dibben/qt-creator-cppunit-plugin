
#include "TestNameDialog.h"
#include "ui_TestNameDialog.h"

using namespace QtcCppunit;
using namespace QtcCppunit::Internal;

namespace Ui {
    class TestNameDialog;
}

TestNameDialog::TestNameDialog(QWidget* parent) :
                    QDialog(parent)
{
    ui = new  Ui::TestNameDialog;
    ui->setupUi(this);
}

TestNameDialog::~TestNameDialog()
{
    delete ui;
}

QString TestNameDialog::testFunctions() const
{
	QStringList tests = ui->lineEdit->text().simplified().split(QLatin1String(" "));

	QStringList result;
	foreach (QString func, tests) {
		if (!func.startsWith(QLatin1String("test"))) {
			if (func.startsWith(QLatin1String("Test"))) {
				func.replace(QLatin1String("Test"), QLatin1String("test"));
			} else {
				func = QLatin1String("test") + func;
			}
		}
		result.append(func);
	}

	return result.join(QLatin1String(" "));
}
