#include "rtspthread.h"

#include <time.h>
#include <QDebug>
#include <QList>
#include <QByteArray>



#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "global.h"



static char* readSDPFile(const char* path)
{
	char* text;
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, path, "r");
	if (err != 0)
	{
		printf("the .sdp file open failed\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);
	text = (char*)malloc(fileSize + 1);

	char c;
	int len = 0;
	while ((c = fgetc(fp)) != EOF)
	{
		text[len++] = c;
	}

	text[len] = '\0';
	return text;
}


static UsageEnvironment& operator<<(UsageEnvironment& env, const MediaSubsession& subsession)
{
	return env << subsession.mediumName() << "/" << subsession.codecName();
}

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 100000



class DummySink : public MediaSink
{
public:
	static DummySink* createNew(UsageEnvironment& env,
		MediaSubsession& subsession, // identifies the kind of data that's being received
		char const* streamId = NULL) // identifies the stream itself (optional)
	{
		return new DummySink(env, subsession, streamId);
	}

	void setFFmpeg(QFFmpeg* ffmpeg)
	{
		m_ffmpeg = ffmpeg;
	}

private:
	DummySink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
		: MediaSink(env), fSubsession(subsession)
	{
		fStreamId = strDup(streamId);
		fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
	}

	virtual ~DummySink()
	{
		delete[] fReceiveBuffer;
		delete[] fStreamId;
	}

	static void afterGettingFrame(void* clientData, unsigned frameSize,
		unsigned numTruncatedBytes,
	struct timeval presentationTime,
		unsigned durationInMicroseconds)
	{
		DummySink* sink = (DummySink*)clientData;
		sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
	}
	void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
	struct timeval presentationTime, unsigned durationInMicroseconds)
	{
		if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
		envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";

		QByteArray frameBuffer((char*)fReceiveBuffer, frameSize);
		
		
		//插入SPS PPS才能让H264解码器正确解码
		QByteArray sps = sprop_parameter_sets;
		QByteArray extraData;
		QList<QByteArray> recodList = sps.split(',');

		for (int i = 0; i < recodList.size(); ++i)
		{
			extraData.append(char(0x00));
			extraData.append(char(0x00));
			extraData.append(char(0x00));
			extraData.append(char(0x01));

			extraData += QByteArray::fromBase64(recodList.at(i));
		}
		QByteArray endMark = QByteArray(4, 0);
		endMark[3] = 0x01;


		frameBuffer.insert(0, extraData);
		frameBuffer.insert(extraData.size(), endMark);

		m_ffmpeg->decodeFrame((uint8_t*)frameBuffer.data(), frameBuffer.size(), presentationTime.tv_sec, presentationTime.tv_usec);

		// Then continue, to request the next frame of data:
		continuePlaying();
	}

private:
	virtual Boolean continuePlaying()
	{
		if (fSource == NULL) return False; // sanity check (should not happen)          
		fSource->getNextFrame(fReceiveBuffer, DUMMY_SINK_RECEIVE_BUFFER_SIZE, afterGettingFrame, this, onSourceClosure, this);
		return True;
	}

private:
	u_int8_t* fReceiveBuffer;
	MediaSubsession& fSubsession;
	char* fStreamId;
	QFFmpeg* m_ffmpeg;
};



RtspThread::RtspThread(QObject *parent)
	: QThread(parent)
{

}

RtspThread::~RtspThread()
{
	
}

void  RtspThread::run()
{
	
	if (!m_ffmpeg->Init())  return;

	//初始化Live555的环境
	m_scheduler = BasicTaskScheduler::createNew();
	m_env = BasicUsageEnvironment::createNew(*m_scheduler);

	//通过dp文件的内容创建媒体会话
	m_session = MediaSession::createNew(*m_env, readSDPFile(m_url.toStdString().c_str()));

	if (!m_session)
	{
		qDebug() << "Created session failed from SDP file";
		return ;
	}

	MediaSubsessionIterator iter(*m_session);

	//迭代子会话，并创建子会话数据接收器。
	while ((m_subsession = iter.next()) != NULL)
	{
		if (!m_subsession->initiate(0))
		{
			*m_env << "Failed to initiate the \"" << *m_subsession << "\" subsession: " << m_env->getResultMsg() << "\n";
		}
		else
		{
			m_subsession->sink = DummySink::createNew(*m_env, *m_subsession, m_url.toStdString().c_str());
			((DummySink*)(m_subsession->sink))->setFFmpeg(m_ffmpeg);
			if (m_subsession->sink == NULL)
			{
				*m_env << "Failed to create a data sink for the \"" << *m_subsession << "\" subsession: " << m_env->getResultMsg() << "\n";
			}
			else
			{
				m_subsession->sink->startPlaying(*m_subsession->rtpSource(), NULL, NULL);
			}
		}
	}

	char eventLoopWatchVariable = 0;
	m_env->taskScheduler().doEventLoop(&eventLoopWatchVariable);
}


void RtspThread::pause()
{
	m_ffmpeg->setPlayState(true);
}

void RtspThread::resume()
{
	m_ffmpeg->setPlayState(false);
}

