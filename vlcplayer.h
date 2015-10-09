#ifndef VLCPLAYER_H
#define VLCPLAYER_H

#include <QString>
#include <QVector>
#include <QtWidgets/QMainWindow>

#include "ui_vlcplayer.h"


class RtspDialog;
class MonitorWindowManager;
class ConfigureDlg;
class MonitorWindow;

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
	void showRowColumnDlg();
	void Stop();

private slots:
	void recvFile(QString& file, QString &path,QString& format);
	void recvConfig(int row, int column);
	

private:
	Ui::VLCPlayerClass *ui;
	MonitorWindowManager *m_manager;
	RtspDialog *m_rtsp;
	ConfigureDlg *m_configDlg;
	QVector<MonitorWindow*> m_playerSet;
	
	int m_row, m_column;
	
};

#endif // VLCPLAYER_H
