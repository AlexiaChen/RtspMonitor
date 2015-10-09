#ifndef RTSPTHREAD_H
#define RTSPTHREAD_H

#include <QThread>
#include "qffmpeg.h"


class TaskScheduler;
class UsageEnvironment;
class MediaSession;
class MediaSubsession;

//解析RTSP的线程
class RtspThread : public QThread
{
	Q_OBJECT

public:
	RtspThread(QObject *parent = 0);
	~RtspThread();
	//线程函数
	void run();
	//暂停线程
	void pause();
	//继续线程
	void resume();
	//解析RTSP的地址，也就是sdp文件的路径
	void setUrl(QString url){ m_url = url; }
	//设置录制输出文件的路径
	void setOutFile(QString file) { m_recordFile = file; }
	//为线程设置解码器
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
