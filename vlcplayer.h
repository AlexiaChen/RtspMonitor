#ifndef VLCPLAYER_H
#define VLCPLAYER_H

#include <QString>
#include <QVector>
#include <QtWidgets/QMainWindow>

#include "ui_vlcplayer.h"
#include "rtspdialog.h"
#include "rtspthread.h"
#include "add_rtsp_url.h"
#include "configuredlg.h"
#include "monitorwindowmanager.h"
#include "monitorwindow.h"

//class RtspDialog;
//class add_Rtsp_Url;
//class MonitorWindowManager;
//class ConfigureDlg;
//class MonitorWindow;

class VLCPlayer : public QMainWindow
{
	Q_OBJECT

public:
	VLCPlayer(QWidget *parent = 0);
	~VLCPlayer();

public slots:
	void Play();
	void Replay();
	void Record();
	void Exit();
	void OpenURL();
    void OpenRTSP();
	void showRowColumnDlg();
	void Stop();

private slots:
	void recvFile(QString& file, QString &path,QString& format);
	void recvConfig(int row, int column);
    void recvDevice(QString& rtsp, QString& deviceName);
	

private:
	Ui::VLCPlayerClass *ui;
	MonitorWindowManager *m_manager;
	RtspDialog *m_rtsp;
    add_Rtsp_Url *m_rtsp_url;
	ConfigureDlg *m_configDlg;
	QVector<MonitorWindow*> m_playerSet;
	
	int m_row, m_column;
	
};

#endif // VLCPLAYER_H
