#include "qffmpeg.h"
#include <stdint.h>
#include <time.h>
#include <QDebug>
#include <QDateTime>
#include <QPainter>
#include <QFont>


QFFmpeg::QFFmpeg(QObject *parent)
	: QObject(parent)
{
	m_videoStreamIndex = -1;
	m_isRecord = -1;
	m_pSwsContext = nullptr;
	
	av_register_all();//注册库中所有可用的文件格式和解码器
	
	m_pAVFrame = av_frame_alloc();
	
}

QFFmpeg::~QFFmpeg()
{

	
	av_frame_free(&m_pAVFrame);

}


bool QFFmpeg::Init()
{
	
	m_pAVCodec = avcodec_find_decoder(AV_CODEC_ID_H264);

	if (!m_pAVCodec)  return false;

	m_pAVCodecContext = avcodec_alloc_context3(m_pAVCodec);
	
	if (avcodec_open2(m_pAVCodecContext, m_pAVCodec, NULL) < 0)  return false;
	
	return true;
}

void QFFmpeg::Record()
{
	

	/*AVFormatContext *recordFmtCtx;
	AVStream *record_video_stream;

	if (avformat_open_input(&recordFmtCtx, m_url.toStdString().c_str(), NULL, NULL) != 0)
	{
		return;
	}


	if (avformat_find_stream_info(recordFmtCtx, NULL) < 0)
	{
		
			return ;
	}

	for (unsigned i = 0; i < recordFmtCtx->nb_streams; i++)
	{
		
		if (recordFmtCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
					record_video_stream = recordFmtCtx->streams[i];
					break;
		}
	}
		
	if (record_video_stream == NULL)
	{
			
		return ;
	}

	avformat_alloc_output_context2(&m_outFmtCtx, NULL, NULL,file.toStdString().c_str());

	m_outVideoStream = avformat_new_stream(m_outFmtCtx, NULL);
	
	m_outFmtCtx->streams[0]->codec = record_video_stream->codec;
	m_outFmtCtx->streams[0]->codec->bit_rate = 400000;
	m_outFmtCtx->streams[0]->codec->codec_id = record_video_stream->codec->codec_id;
	m_outFmtCtx->streams[0]->codec->codec_type = record_video_stream->codec->codec_type;
	m_outFmtCtx->streams[0]->codec->time_base.num = record_video_stream->time_base.num;
	m_outFmtCtx->streams[0]->codec->time_base.den = record_video_stream->time_base.den;
	m_outFmtCtx->streams[0]->codec->width = record_video_stream->codec->width;
	m_outFmtCtx->streams[0]->codec->height = record_video_stream->codec->height;
	m_outFmtCtx->streams[0]->codec->pix_fmt = record_video_stream->codec->pix_fmt;
	m_outFmtCtx->streams[0]->codec->flags = record_video_stream->codec->flags;
	m_outFmtCtx->streams[0]->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	m_outFmtCtx->streams[0]->codec->me_range = record_video_stream->codec->me_range;
	m_outFmtCtx->streams[0]->codec->max_qdiff = record_video_stream->codec->max_qdiff;
	m_outFmtCtx->streams[0]->codec->qmin = record_video_stream->codec->qmin;
	m_outFmtCtx->streams[0]->codec->qmax = record_video_stream->codec->qmax;
	

	if (avio_open(&m_outFmtCtx->pb, file.toStdString().c_str(), AVIO_FLAG_WRITE) < 0)
	{
		return;
	}

	int ret = avformat_write_header(m_outFmtCtx, NULL);
	if (ret != 0)
	{
		return;
	}

	int last_pts = 0;
	int last_dts = 0;

	int64_t pts, dts;
	
	while(true)
	{
		
		m_recordMutex.lock();
		
		AVPacket tmpPacket;
		av_init_packet(&tmpPacket);
		tmpPacket.size = 0;
		tmpPacket.data = nullptr;

		if (av_read_frame(recordFmtCtx, &tmpPacket) < 0)
		{
			break;
		}

		tmpPacket.flags |= AV_PKT_FLAG_KEY;
		
		pts = tmpPacket.pts;
		tmpPacket.pts += last_pts;
		
		dts = tmpPacket.dts;
		tmpPacket.dts += last_dts;

		tmpPacket.stream_index = 0;


		av_interleaved_write_frame(m_outFmtCtx, &tmpPacket);

		m_recordMutex.unlock();

		if (m_isRecord == 0)
		{
			break;
		}
	}

	last_dts += dts;
	last_pts += pts;

	
	av_write_trailer(m_outFmtCtx);
	avio_close(m_outFmtCtx->pb);
	av_free(m_outFmtCtx);
	avformat_close_input(&recordFmtCtx);*/

	//avformat_alloc_output_context2(&m_outFmtCtx, NULL, NULL, file.toStdString().c_str());

}

void QFFmpeg::setPlayState(bool pause)
{
	if (pause)
	{
		
		m_playMutex.lock();
		
	}
	else
	{
		m_playMutex.unlock();
	}
}

void QFFmpeg::setRecordState(bool record)
{
	if (record)
	{
		m_isRecord = 1;
	}
	else
	{
		m_isRecord = 0;
	}
}

void  QFFmpeg::decodeFrame(uint8_t* frameBuffer, int frameLength, long second, long microSecond)
{
	if (frameLength <= 0) return;

	int frameFinished = 0;

	AVPacket framePacket;
	av_init_packet(&framePacket);

	framePacket.size = frameLength;
	framePacket.data = frameBuffer;

	int ret = avcodec_decode_video2(m_pAVCodecContext, m_pAVFrame, &frameFinished, &framePacket);

	if (ret < 0)
	{
		qDebug() << "Decode error";
		return;
	}
	
	//解码成功
	if (frameFinished)
	{
		m_playMutex.lock();

		m_videoWidth = m_pAVFrame->width;
		m_videoHeight = m_pAVFrame->height;

		//创建一个Image，以存储放缩后的帧图像
		QImage frame = QImage(m_videoWidth, m_videoHeight, QImage::Format_ARGB32);
		
		m_pSwsContext = sws_getCachedContext(m_pSwsContext, m_videoWidth, m_videoHeight, AVPixelFormat::AV_PIX_FMT_YUV420P, m_videoWidth, m_videoHeight,
			AVPixelFormat::AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

		uint8_t *dstSlice[] = { frame.bits() };
		int dstStride = frame.width() * 4;
		
		sws_scale(m_pSwsContext, m_pAVFrame->data, m_pAVFrame->linesize, 0, m_videoHeight, dstSlice, &dstStride);
		
		char timestamp[100];
		char  millisecond[50];

		time_t time = second;
		struct tm *now_time;

		now_time = gmtime(&time);

		strftime(timestamp, 100, "%Y-%m-%d %H:%M:%S", now_time);
		sprintf_s(millisecond, 50, "  %ld.%d%d%d",microSecond/1000,0,0,0);

		//发送获取一帧图像信号
		QImage image((uchar*)dstSlice[0], m_videoWidth, m_videoHeight, QImage::Format_RGB32);

		//在帧图像上打印时间戳信息并发送
		QPainter pen(&image);
		pen.setPen(Qt::white);
		pen.setFont(QFont("Times", 30, QFont::Bold));
		pen.drawText(image.rect(), Qt::AlignBottom, QString(timestamp) + QString(millisecond));
		emit GetImage(image);
		m_playMutex.unlock();
	}

	
}

