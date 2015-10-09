#ifndef QFFMPEG_H
#define QFFMPEG_H

// �������������, ������벻��ͨ��, Ϊ�˼���C��C99��׼
//#ifndef INT64_C
//#define INT64_C
//#define UINT64_C
//#endif

//����ffmpegͷ�ļ�
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

//������
class QFFmpeg : public QObject
{
	Q_OBJECT

public:
	//��ʼ��������
	QFFmpeg(QObject *parent);
	~QFFmpeg();
	
	//��������ʼ��
	bool Init();
	//����һ֡��һ֡����buffer��buffer��������֡��Ӧ��timestamp
	void decodeFrame(uint8_t* frameBuffer, int frameLength, long second, long microSecond);
	//¼��
	void Record();
	//���ò���״̬�����Ż���ͣ
	void setPlayState(bool pause);
	//����¼��״̬����ʼ¼�ƻ�ֹͣ¼��
	void setRecordState(bool record);
	//����URL
	void SetUrl(QString url){ this->m_url = url; }
	//����¼���ļ������·��
	void SetOutFile(QString &path) { this->m_recordFile = path; }
	//����url
	QString Url()const{ return m_url; }

signals:
	//һ��һ֡����ɹ������̷��͸��ź�
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
