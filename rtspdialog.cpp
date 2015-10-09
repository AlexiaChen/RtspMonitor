#include <QMessageBox>
#include <QFileDialog>
#include <QRegExp>
#include <QDebug>

#include "rtspdialog.h"

RtspDialog::RtspDialog(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::RtspDialog;
	ui->setupUi(this);

	connect(ui->ok, SIGNAL(clicked()), this, SLOT(OK()));
	connect(ui->cancel, SIGNAL(clicked()), this, SLOT(Cancel()));
	connect(ui->lookFile, SIGNAL(clicked()), this, SLOT(OpenFile()));
	connect(ui->lookPath, SIGNAL(clicked()), this, SLOT(OpenPath()));

	
}

RtspDialog::~RtspDialog()
{
	delete ui;
}

void RtspDialog::OK()
{

	QRegExp sdpFileExp(QString("[c-zC-Z]:(/\\w+)*([/]|[.][sdp]+)?"), Qt::CaseInsensitive);
	QRegExp recordDirExp(QString("[c-zC-Z]:(/\\w+)*/"),Qt::CaseInsensitive);
	
	if (sdpFileExp.exactMatch(ui->filePath->text()) && recordDirExp.exactMatch(ui->recordPath->text()))
	{
		
		emit fileComplete(ui->filePath->text(), ui->recordPath->text(),ui->format->currentText());
		this->close();
		
	}
	else
	{
		QMessageBox::warning(this, "Invalid Input", "<p>Example:</p>        \
        <p>SdpFile:</p> \
		<p><font color=RED>D:/UserName/test.sdp</font></p>  \
		<p>RecordPath:</p>                              \
		<p><font color=RED>D:/output/ </font></p>          \
		");
	}
}

void RtspDialog::Cancel()
{
	this->close();
}

void  RtspDialog::OpenFile()
{
	QString file = QFileDialog::getOpenFileName(this, tr("Open File"), "D:\\", tr("SDP File (*.sdp)"));

	ui->filePath->setText(file);
}

void RtspDialog::OpenPath()
{
	QString path = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "D:\\");
	path += QString("/");

	ui->recordPath->setText(path);
}





