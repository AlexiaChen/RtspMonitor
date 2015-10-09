#ifndef QFFMPEG_H
#define QFFMPEG_H

// 必须加以下内容, 否则编译不能通过, 为了兼容C和C99标准
//#ifndef INT64_C
//#define INT64_C
//#define UINT64_C
//#endif

//引入ffmpeg头文件
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libswscale/swscale.h>
#include <libavutil/frame.h>
#include <libavdevice/avdevice.h>

}

#include <QObject>
#include <QMutex>
#include <QImage>

//解码器
class QFFmpeg : public QObject
{
	Q_OBJECT

public:
	//初始化解码器
	QFFmpeg(QObject *parent);
	~QFFmpeg();
	
	//解码器初始化
	bool Init();
	//解码一帧，一帧包含buffer和buffer长，还有帧对应的timestamp
	void decodeFrame(uint8_t* frameBuffer, int frameLength, long second, long microSecond);
	//录制
	void Record();
	//设置播放状态，播放或暂停
	void setPlayState(bool pause);
	//设置录制状态，开始录制或停止录制
	void setRecordState(bool record);
	//设置URL
	void SetUrl(QString url){ this->m_url = url; }
	//设置录制文件的输出路径
	void SetOutFile(QString &path) { this->m_recordFile = path; }
	//返回url
	QString Url()const{ return m_url; }

signals:
	//一旦一帧解码成功，立刻发送该信号
	void GetImage(const QImage &image);

private:

	QMutex m_playMutex;
	QMutex m_recordMutex;
	AVPicture  m_AVPicture;
	AVCodec *m_pAVCodec;
	AVCodecContext *m_pAVCodecContext;
	AVFrame *m_pAVFrame;
	SwsContext * m_pSwsContext;
	AVStream *m_pStream;
	AVFormatContext *m_outFmtCtx;
	AVStream*  m_outVideoStream;

	QString m_url;
	QString m_recordFile;
	
	int m_videoWidth;
	int m_videoHeight;
	int m_videoStreamIndex;
	int m_isRecord;

};

#endif // QFFMPEG_H
