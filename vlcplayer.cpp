#include <iostream>
#include <cstdlib>

#include <QFileDialog>
#include <QMenu>
#include <QString>
#include <QStringList>
#include <QIcon>
#include <QDebug>
#include <QSize>

#include "vlcplayer.h"


VLCPlayer::VLCPlayer(QWidget *parent)
    : QMainWindow(parent), m_row(1), m_column(1)
{
	
	ui = new Ui::VLCPlayerClass;
	m_rtsp = new RtspDialog(this);
    m_rtsp_url = new add_Rtsp_Url(this);
	m_configDlg = new ConfigureDlg(this);
	
	ui->setupUi(this);

	QMenu *file = menuBar()->addMenu("File");
	QAction *openUrl = file->addAction("Open SDP File");
    QAction *openRtsp = file->addAction("Open Rtsp Url");
    QAction *exit = file->addAction("Exit");


	QMenu *config = menuBar()->addMenu("Configure");
	QAction *layout = config->addAction("Row X Column");
	
	//����һ��Manager�࣬�Ա������Ӵ��ڵ���������
	m_manager = new MonitorWindowManager(m_row, m_column, ui->container);

	ui->container->setLayout(m_manager);
	
	for (int i = 0; i < m_row; ++i)
	{
		for (int j = 0; j < m_column;++j)
		{
			m_playerSet.append(m_manager->getItemAtPosition(i, j));
		}
	}

	for (auto& player : m_playerSet)
	{
		connect(player->ffmpeg(), SIGNAL(GetImage(const QImage &)), player, SLOT(setImage(const QImage&)));
	}
	
	connect(ui->play, SIGNAL(clicked()), this, SLOT(Play()));
	connect(ui->replay, SIGNAL(clicked()), this, SLOT(Replay()));
	connect(ui->record, SIGNAL(clicked()), this, SLOT(Record()));
	connect(ui->stop, SIGNAL(clicked()), this, SLOT(Stop()));

	connect(exit, SIGNAL(triggered()), this, SLOT(Exit()));
	connect(openUrl, SIGNAL(triggered()), this, SLOT(OpenURL()));
    connect(openRtsp, SIGNAL(triggered()), this, SLOT(OpenRTSP()));
	connect(layout, SIGNAL(triggered()), this, SLOT(showRowColumnDlg()));
	connect(m_rtsp, SIGNAL(fileComplete(QString&, QString&,QString&)), this,SLOT(recvFile(QString&, QString&,QString&)));
	connect(m_configDlg, SIGNAL(configComplete(int, int)), this, SLOT(recvConfig(int, int)));
    connect(m_rtsp_url, SIGNAL(add_a_device(QString&,QString&)),this,SLOT(recvDevice(QString&, QString&)));

}

VLCPlayer::~VLCPlayer()
{
	delete ui;
	delete m_rtsp;
	delete m_configDlg;
	delete m_manager;
	
}

//���ź���ͣ
void VLCPlayer::Play()
{
	
	for (auto &player : m_playerSet)
	{
		if (player->isActive())
		{
			if (player->isPlaying())
			{
				player->pausePlaying();
				ui->play->setIcon(QIcon(":/Resources/play.png"));

			}
			else
			{
				player->resumePlaying();
				ui->play->setIcon(QIcon(":/Resources/pause.png"));

			}
		}
	}

}

void VLCPlayer::Replay()
{
	
}

void VLCPlayer::Record()
{
	for (auto& player : m_playerSet)
	{
		if (player->isActive())
		{
			if (player->isRecording())
			{
				player->stopRecording();
				ui->record->setIcon(QIcon(":/Resources/record.png"));
			}
			else
			{
				player->startRecording();
				ui->record->setIcon(QIcon(":/Resources/offrecord.png"));
			}
		}
	}
	
}

void VLCPlayer::Exit()
{
	this->close();
}

void VLCPlayer::OpenURL()
{
	m_rtsp->show();
}

void VLCPlayer::OpenRTSP()
{
    m_rtsp_url->show();
}

void VLCPlayer::recvFile(QString& file, QString &path,QString& format)
{
	
	for (auto& player : m_playerSet)
	{
		if (!player->isActive())
		{
			ui->play->setIcon(QIcon(":/Resources/pause.png"));
			player->setUrl(file);
//			player->setOutFile(path);
            player->setOutPath(path, format);
            player->startPlaying();
		}
	}

}

void::VLCPlayer::recvDevice(QString& rtsp, QString& deviceName)
{
    for(auto& player : m_playerSet)
    {
        if(!player->isActive())
        {
            ui->play->setIcon(QIcon(":/Resources/pause.png"));
            player->setRtspUrl(rtsp);
            player->startPlaying();
        }
    }
}

void VLCPlayer::recvConfig(int row, int column)
{
	m_row = row;
	m_column = column;

	m_manager->setMonitorLayout(m_row, m_column);

	m_playerSet.clear();

	for (int i = 0; i < m_row; ++i)
	{
		for (int j = 0; j < m_column; ++j)
		{
			m_playerSet.append(m_manager->getItemAtPosition(i, j));
		}
	}

	for (auto &player : m_playerSet)
	{
		connect(player->ffmpeg(), SIGNAL(GetImage(const QImage &)), player, SLOT(setImage(const QImage&)));
	}

}

//ֹͣ���رռ���
void VLCPlayer::Stop()
{
	
	for (auto &player : m_playerSet)
	{
		if (player->isActive())
		{
			player->stop();
		}
	}

	ui->play->setIcon(QIcon(":/Resources/play.png"));
	ui->record->setIcon(QIcon(":/Resources/record.png"));
	
	for(auto &player : m_playerSet)
	{
		player->setPixmap(QPixmap());
	}

}

void VLCPlayer::showRowColumnDlg()
{
	m_configDlg->show();
}



