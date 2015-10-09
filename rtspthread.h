#ifndef RTSPTHREAD_H
#define RTSPTHREAD_H

#include <QThread>
#include "qffmpeg.h"


class TaskScheduler;
class UsageEnvironment;
class MediaSession;
class MediaSubsession;

//����RTSP���߳�
class RtspThread : public QThread
{
	Q_OBJECT

public:
	RtspThread(QObject *parent = 0);
	~RtspThread();
	//�̺߳���
	void run();
	//��ͣ�߳�
	void pause();
	//�����߳�
	void resume();
	//����RTSP�ĵ�ַ��Ҳ����sdp�ļ���·��
	void setUrl(QString url){ m_url = url; }
	//����¼������ļ���·��
	void setOutFile(QString file) { m_recordFile = file; }
	//Ϊ�߳����ý�����
	void setffmpeg(QFFmpeg *f){ m_ffmpeg = f; }

private:
	QFFmpeg * m_ffmpeg;
	
	QString m_url;
	QString m_recordFile;
	TaskScheduler* m_scheduler;
	UsageEnvironment* m_env;
	MediaSession* m_session;

	MediaSubsession* m_subsession;

};

#endif // RTSPTHREAD_H
