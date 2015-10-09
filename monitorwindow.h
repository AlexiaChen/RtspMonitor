#ifndef MONITORWINDOW_H
#define MONITORWINDOW_H

#include <QObject>
#include <QLabel>
#include <QString>

class QFFmpeg;
class RtspThread;


//监视窗口，视频图像就显示在监视窗口上
class MonitorWindow : public QLabel
{
	Q_OBJECT

public:
	MonitorWindow(QWidget *parent);
	~MonitorWindow();

public slots:
    //判断窗口是否在播放中
	bool isPlaying() const;
	//判断窗口是否在录制中
	bool isRecording() const;
	//判断窗口是否出于活动状态
	bool isActive() const;
	//获得窗口的解码器
	QFFmpeg* ffmpeg() const;
	//设置窗口的url路径
	void setUrl(QString& url);
	//设置窗口视频的录制文件的输出路径
	void setOutPath(QString& path,QString& format);
	//开始播放
	void startPlaying();
	//暂停播放
	void pausePlaying();
	//继续播放
	void resumePlaying();
	//开始录制
	void startRecording();
	//停止录制
	void stopRecording();
	//关闭播放和录制，此时isActive会返回false
	void stop();
	//把一帧图像设置到窗口上显示
	void setImage(const QImage& img);
	

private:

	RtspThread *m_thread;
	QFFmpeg *m_ffmpeg;
	QString m_url;
	QString m_recordPath;
	QString m_format;

	bool m_isRecording;
	bool m_isPlaying;
	bool m_isActive;
	
};

#endif // MONITORWINDOW_H
