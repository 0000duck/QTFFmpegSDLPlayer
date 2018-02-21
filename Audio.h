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
	double audioClock; // audio clock
	uint8_t *audioBuff;       // ��������ݵĻ���ռ�
	uint32_t audioBuffSize;  // buffer�е��ֽ���
	uint32_t audioBuffIndex; // buffer��δ�������ݵ�index
							   /**
							   * audio play
							   */
	bool audioPlay();

	// get audio clock
	double get_audio_clock();

	int getStreamIndex();
	void setStreamIndex(const int streamIndex);
	AVCodecContext *audioContext;
	AVStream *stream;
	int getAudioQueueSize();
	void enqueuePacket(const AVPacket pkt);
	PacketQueue audiaPackets;
private:
	int streamIndex;

};
/**
* ���豸����audio���ݵĻص�����
*/
void audioCallback(void* userdata, Uint8 *stream, int len);

/**
* ����Avpacket�е�������䵽����ռ�
*/
int audioDecodeFrame(Audio*audio, uint8_t *audioBuffer, int bufferSize);
