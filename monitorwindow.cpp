#include "monitorwindow.h"

#include "rtspthread.h"
#include "qffmpeg.h"

#include <QPixmap>

MonitorWindow::MonitorWindow(QWidget *parent)
	: QLabel(parent)
{
	m_thread = new RtspThread(this);
	m_ffmpeg = new QFFmpeg(this);
	m_url = QString();
	m_recordPath =  QString();

	m_thread->setffmpeg(m_ffmpeg);

	m_isRecording = false;
	m_isPlaying = false;
	m_isActive = false;

	this->setStyleSheet(QString("border: 3px solid gray; \
								border-radius: 10px; \
								background: white; \
								background-image: url(:/Resources/background.jpg) 0 0 0 0 stretch stretch; "));


}

MonitorWindow::~MonitorWindow()
{
	if (m_thread)
	{
		m_thread->terminate();
		delete m_thread;
	}

	if (m_ffmpeg)
	{
		delete m_ffmpeg;
	}
}



bool MonitorWindow::isPlaying() const
{
	return m_isPlaying;
}

bool MonitorWindow::isRecording() const
{
	return m_isRecording;
}

bool  MonitorWindow::isActive() const
{
	return m_isActive;
}

QFFmpeg* MonitorWindow::ffmpeg() const
{
	return m_ffmpeg;
}

void MonitorWindow::setUrl(QString& url)
{
	m_url = url;
	m_thread->setUrl(m_url);
}

void MonitorWindow::setOutPath(QString& path, QString& format)
{
	m_recordPath = path;
	m_format = format;
}

void MonitorWindow::startPlaying()
{
	
	m_thread->start();
	m_isActive = true;
	m_isPlaying = true;
}

void MonitorWindow::pausePlaying()
{
	m_thread->pause();
	m_isPlaying = false;
}

void MonitorWindow::resumePlaying()
{
	m_thread->resume();
	m_isPlaying = true;
}


void MonitorWindow::startRecording()
{
	
	m_isRecording = true;
}

void MonitorWindow::stopRecording()
{
	m_isRecording = false;
}

void MonitorWindow::stop()
{
	if (m_thread)
	{
		delete m_thread;
	}

	m_thread = nullptr;
	m_isPlaying = false;
	m_isRecording = false;
	m_isActive = false;
}

void MonitorWindow::setImage(const QImage& img)
{
	if (img.height() > 0)
	{

		QPixmap pix = QPixmap::fromImage(img.scaled(this->width() - 15, this->height() - 15));
		this->setPixmap(pix);

	}
}

