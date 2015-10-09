#include "configuredlg.h"
#include "ui_configuredlg.h"


#include <QMessageBox>
ConfigureDlg::ConfigureDlg(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::ConfigureDlg();
	ui->setupUi(this);

	connect(ui->ok, SIGNAL(clicked()), this, SLOT(ok()));
}

ConfigureDlg::~ConfigureDlg()
{
	delete ui;
}

void ConfigureDlg::ok()
{
	if (!ui->row->text().isEmpty() && !ui->column->text().isEmpty())
	{
		bool r_ok ,c_ok;
		int row = ui->row->text().toInt(&r_ok);
		int column = ui->column->text().toInt(&c_ok);

		if (!r_ok || !c_ok)
		{
			QMessageBox::warning(this, "Invalid Input", "        \
			<p>Row And Column must be numerical value.</p>  \
			");

			return;
		}

		if (row > 3 || column > 3)
		{
			QMessageBox::warning(this, "Invalid Input", "        \
			<p>Max Row X Column is 3 X 3.</p>  \
			");

			return;
		}

		emit configComplete(row, column);
		this->close();
	}
	else
	{
		QMessageBox::warning(this, "Invalid Input", "        \
		<p>Row And Column cannot be empty.</p>  \
		");
	}
}

