#ifndef TEST_NAME_DIALOG_H
#define TEST_NAME_DIALOG_H

#include <QDialog>

namespace QtcCppunit {
namespace Internal {


namespace Ui {
    class TestNameDialog;
}

class TestNameDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TestNameDialog(QWidget *parent = 0);
    ~TestNameDialog();

	QString	testFunctions() const;

private:

	Ui::TestNameDialog *ui;
};


}
}

#endif
