#pragma once
#include "PacketQueue.h"
extern "C" {

#include <libavformat/avformat.h>

}
class Audio
{
public:
	Audio();
	~Audio();
	const uint32_t BUFFERSIZE = 192000;// �������Ĵ�С									   
	bool audioPlay();
	bool audioClose();
	double getCurrentAudioClock();
	int getStreamIndex();
	void setStreamIndex(const int streamIndex);
	int getAudioQueueSize();
	void enqueuePacket(const AVPacket pkt);
	AVPacket dequeuePacket();
	uint8_t* getAudioBuff();
	void setAudioBuff(uint8_t*& audioBuff);
	uint32_t getAudioBuffSize();
	void setAudioBuffSize(uint32_t audioBuffSize);
	uint32_t getAudioBuffIndex();
	void setAudioBuffIndex(uint32_t audioBuffIndex);
	double getAudioClock();
	void setAudioClock(const double &audioClock);
	AVStream *getStream();
	void setStream(AVStream *&stream);
	AVCodecContext *getAVCodecContext();
	void setAVCodecContext(AVCodecContext *audioContext);
	bool getIsPlaying();
	void setPlaying(bool isPlaying);
	void clearPacket();
private:
	AVCodecContext *audioContext;
	AVStream *stream;
	double audioClock; // audio clock
	PacketQueue audiaPackets;
	uint8_t *audioBuff;       // ��������ݵĻ���ռ�
	uint32_t audioBuffSize;  // buffer�е��ֽ���
	uint32_t audioBuffIndex; // buffer��δ�������ݵ�index
	int streamIndex;
	bool isPlay = false;

};
/**
* ���豸����audio���ݵĻص�����
*/
void audioCallback(void* userdata, Uint8 *stream, int len);

/**
* ����Avpacket�е�������䵽����ռ�
*/
int audioDecodeFrame(Audio*audio, uint8_t *audioBuffer, int bufferSize);
