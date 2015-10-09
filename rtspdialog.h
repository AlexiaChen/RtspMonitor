#ifndef RTSPDIALOG_H
#define RTSPDIALOG_H

#include <QWidget>
#include <QDialog>
#include "ui_rtspdialog.h"

class RtspDialog : public QDialog
{
	Q_OBJECT

public:
	RtspDialog(QWidget *parent = 0);
	~RtspDialog();

signals:
	void fileComplete(QString& file, QString &path,QString &format);

public slots:
	void OK();
	void Cancel();
	void OpenFile();
	void OpenPath();

private:
	Ui::RtspDialog *ui;
};

#endif // RTSPDIALOG_H
