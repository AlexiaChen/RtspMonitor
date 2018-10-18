#ifndef RTSPTHREAD_H
#define RTSPTHREAD_H

#include <QThread>
#include "qffmpeg.h"
#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"

#define SDP 0
#define RTSP 1

class TaskScheduler;
class UsageEnvironment;
class MediaSession;
class MediaSubsession;

#define DUMMY_SINK_RECEIVE_BUFFER_SIZE 100000

static const char* sprop_parameter_sets = "Z2QAKK2EBUViuKxUdCAqKxXFYqOhAVFYrisVHQgKisVxWKjoQFRWK4rFR0ICorFcVio6ECSFITk8nyfk/k/J8nm5s00IEkKQnJ5Pk/J/J+T5PNzZprQDwBE/LgKpAAADAAEAAAMAPGBAAB6EgAACJVFve+F4RCNQAAAAAQ==,aO48sA==";


class imshowSink : public MediaSink
{
public:
    static imshowSink* createNew(UsageEnvironment& env,
        MediaSubsession& subsession, // identifies the kind of data that's being received
        char const* streamId = NULL) // identifies the stream itself (optional)
    {
        return new imshowSink(env, subsession, streamId);
    }

    void setFFmpeg(QFFmpeg* ffmpeg)
    {
        m_ffmpeg = ffmpeg;
    }

private:
    imshowSink(UsageEnvironment& env, MediaSubsession& subsession, char const* streamId)
        : MediaSink(env), fSubsession(subsession)
    {
        fStreamId = strDup(streamId);
        fReceiveBuffer = new u_int8_t[DUMMY_SINK_RECEIVE_BUFFER_SIZE];
    }

    virtual ~imshowSink()
    {
        delete[] fReceiveBuffer;
        delete[] fStreamId;
    }

    static void afterGettingFrame(void* clientData, unsigned frameSize,
        unsigned numTruncatedBytes,
        struct timeval presentationTime,
        unsigned durationInMicroseconds)
    {
        imshowSink* sink = (imshowSink*)clientData;
        sink->afterGettingFrame(frameSize, numTruncatedBytes, presentationTime, durationInMicroseconds);
    }
    void afterGettingFrame(unsigned frameSize, unsigned numTruncatedBytes,
        struct timeval presentationTime, unsigned durationInMicroseconds)
    {
        if (fStreamId != NULL) envir() << "Stream \"" << fStreamId << "\"; ";
        envir() << fSubsession.mediumName() << "/" << fSubsession.codecName() << ":\tReceived " << frameSize << " bytes";

        QByteArray frameBuffer((char*)fReceiveBuffer, frameSize);

        QByteArray sps = fSubsession.fmtp_spropparametersets();
//        printf("????????????%s\n",sps.data() );
        QByteArray text = QByteArray::fromBase64(sps);
//        printf("!!!!!!!!!!!!%s\n", sps.data() );
        //插入SPS PPS才能让H264解码器正确解码
        if(sps.isEmpty())
        {
            sps = sprop_parameter_sets;
        }
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
    //
    void set_url_form(qint8 url_form){m_url_form = url_form;}
	//设置录制输出文件的路径
	void setOutFile(QString file) { m_recordFile = file; }
	//为线程设置解码器
	void setffmpeg(QFFmpeg *f){ m_ffmpeg = f; }
    void sdpRun();



private:
	QFFmpeg * m_ffmpeg;
    qint8 m_url_form;
	QString m_url;
	QString m_recordFile;
	TaskScheduler* m_scheduler;
	UsageEnvironment* m_env;
	MediaSession* m_session;
	MediaSubsession* m_subsession;

};

// Define a class to hold per-stream state that we maintain throughout each stream's lifetime:

class StreamClientState {
public:
  StreamClientState();
  virtual ~StreamClientState();

public:
  MediaSubsessionIterator* iter;
  MediaSession* session;
  MediaSubsession* subsession;
  TaskToken streamTimerTask;
  double duration;

  QFFmpeg * m_ffmpeg;
};

// If you're streaming just a single stream (i.e., just from a single URL, once), then you can define and use just a single
// "StreamClientState" structure, as a global variable in your application.  However, because - in this demo application - we're
// showing how to play multiple streams, concurrently, we can't do that.  Instead, we have to have a separate "StreamClientState"
// structure for each "RTSPClient".  To do this, we subclass "RTSPClient", and add a "StreamClientState" field to the subclass:

class ourRTSPClient: public RTSPClient {
public:
  static ourRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
                  int verbosityLevel = 0,
                  char const* applicationName = NULL,
                  portNumBits tunnelOverHTTPPortNum = 0);

protected:
  ourRTSPClient(UsageEnvironment& env, char const* rtspURL,
        int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum);
    // called only by createNew();
  virtual ~ourRTSPClient();

public:
  StreamClientState scs;
};

// RTSP 'response handlers':
void continueAfterDESCRIBE(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterSETUP(RTSPClient* rtspClient, int resultCode, char* resultString);
void continueAfterPLAY(RTSPClient* rtspClient, int resultCode, char* resultString);

// Other event handler functions:
void subsessionAfterPlaying(void* clientData); // called when a stream's subsession (e.g., audio or video substream) ends
void subsessionByeHandler(void* clientData); // called when a RTCP "BYE" is received for a subsession
void streamTimerHandler(void* clientData);
  // called at the end of a stream's expected duration (if the stream has not already signaled its end using a RTCP "BYE")

// The main streaming routine (for each "rtsp://" URL):
void openURL(UsageEnvironment& env, char const* progName, char const* rtspURL, QFFmpeg * ffmepg);

// Used to iterate through each stream's 'subsessions', setting up each one:
void setupNextSubsession(RTSPClient* rtspClient);

// Used to shut down and close a stream (including its "RTSPClient" object):
void shutdownStream(RTSPClient* rtspClient, int exitCode = 1);



#endif // RTSPTHREAD_H
