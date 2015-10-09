#ifndef CONFIGUREDLG_H
#define CONFIGUREDLG_H

#include <QDialog>
namespace Ui {class ConfigureDlg;};

class ConfigureDlg : public QDialog
{
	Q_OBJECT

public:
	ConfigureDlg(QWidget *parent = 0);
	~ConfigureDlg();

public slots:
	void ok();


signals:
	void configComplete(int row, int column);

private:
	Ui::ConfigureDlg *ui;
};

#endif // CONFIGUREDLG_H
