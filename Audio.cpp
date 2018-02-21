#include "Audio.h"
#include "Media.h"
#include <fstream>
extern "C" {

#include <libswresample\swresample.h>

}

Audio::Audio()
{
	audioContext = nullptr;
	streamIndex = -1;
	stream = nullptr;
	audioClock = 0;

	audioBuff = new uint8_t[BUFFERSIZE];
	audioBuffSize = 0;
	audioBuffIndex = 0;
}


Audio::~Audio()
{
	if (audioBuff)
		delete[] audioBuff;
}

bool Audio::audioPlay()
{
	SDL_AudioSpec desired;
	desired.freq = audioContext->sample_rate;
	desired.channels = audioContext->channels;
	desired.format = AUDIO_S16SYS;
	desired.samples = 1024;
	desired.silence = 0;
	desired.userdata = this;
	desired.callback = audioCallback;

	if (SDL_OpenAudio(&desired, nullptr) < 0)
	{
		return false;
	}

	SDL_PauseAudio(0); // playing

	return true;
}

double Audio::get_audio_clock()
{
	int hw_buf_size = audioBuffSize - audioBuffIndex;
	int bytes_per_sec = stream->codec->sample_rate * audioContext->channels * 2;

	double pts = audioClock - static_cast<double>(hw_buf_size) / bytes_per_sec;


	return pts;
}

int Audio::getStreamIndex()
{
	return streamIndex;
}

void Audio::setStreamIndex(const int streamIndex)
{
	this->streamIndex = streamIndex;
}

int Audio::getAudioQueueSize()
{
	return audiaPackets.getPacketSize();
}

void Audio::enqueuePacket(const AVPacket pkt)
{
	audiaPackets.enQueue(pkt);
}

/**
* ���豸����audio���ݵĻص�����
*/
void audioCallback(void* userdata, Uint8 *stream, int len) {
	Audio  *audio = Media::getInstance()->audio;

	SDL_memset(stream, 0, len);

	int audio_size = 0;
	int len1 = 0;
	while (len > 0)// ���豸���ͳ���Ϊlen������
	{
		if (audio->audioBuffIndex >= audio->audioBuffSize) // ��������������
		{
			// ��packet�н�������
			audio_size = audioDecodeFrame(audio, audio->audioBuff, sizeof(audio->audioBuff));
			if (audio_size < 0) // û�н��뵽���ݻ�������0
			{
				audio->audioBuffSize = 0;
				memset(audio->audioBuff, 0, audio->audioBuffSize);
			}
			else
				audio->audioBuffSize = audio_size;

			audio->audioBuffIndex = 0;
		}
		len1 = audio->audioBuffSize - audio->audioBuffIndex; // ��������ʣ�µ����ݳ���
		if (len1 > len) // ���豸���͵����ݳ���Ϊlen
			len1 = len;

		SDL_MixAudio(stream, audio->audioBuff + audio->audioBuffIndex, len, SDL_MIX_MAXVOLUME);

		len -= len1;
		stream += len1;
		audio->audioBuffIndex += len1;
	}
}

/**
* ����Avpacket�е�������䵽����ռ�
*/
int audioDecodeFrame(Audio*audio, uint8_t *audioBuffer, int bufferSize) {
	AVFrame *frame = av_frame_alloc();
	int data_size = 0;
	AVPacket pkt = audio->audiaPackets.deQueue();
	SwrContext *swr_ctx = nullptr;
	static double clock = 0;

	/*if (quit)
		return -1;*/
	if (pkt.size <= 0)
	{
		return -1;
	}	

	if (pkt.pts != AV_NOPTS_VALUE)
	{
		audio->audioClock = av_q2d(audio->stream->time_base) * pkt.pts;
	}
	int ret = avcodec_send_packet(audio->audioContext, &pkt);
	if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
		return -1;

	ret = avcodec_receive_frame(audio->audioContext, frame);
	if (ret < 0 && ret != AVERROR_EOF)
		return -1;

	// ����ͨ������channel_layout
	if (frame->channels > 0 && frame->channel_layout == 0)
		frame->channel_layout = av_get_default_channel_layout(frame->channels);
	else if (frame->channels == 0 && frame->channel_layout > 0)
		frame->channels = av_get_channel_layout_nb_channels(frame->channel_layout);

	AVSampleFormat dst_format = AV_SAMPLE_FMT_S16;//av_get_packed_sample_fmt((AVSampleFormat)frame->format);
	Uint64 dst_layout = av_get_default_channel_layout(frame->channels);
	// ����ת������
	swr_ctx = swr_alloc_set_opts(nullptr, dst_layout, dst_format, frame->sample_rate,
		frame->channel_layout, (AVSampleFormat)frame->format, frame->sample_rate, 0, nullptr);
	if (!swr_ctx || swr_init(swr_ctx) < 0)
		return -1;

	// ����ת�����sample���� a * b / c
	uint64_t dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, frame->sample_rate) + frame->nb_samples, frame->sample_rate, frame->sample_rate, AVRounding(1));
	// ת��������ֵΪת�����sample����
	int nb = swr_convert(swr_ctx, &audioBuffer, static_cast<int>(dst_nb_samples), (const uint8_t**)frame->data, frame->nb_samples);
	data_size = frame->channels * nb * av_get_bytes_per_sample(dst_format);

	// ÿ������Ƶ���ŵ��ֽ��� sample_rate * channels * sample_format(һ��sampleռ�õ��ֽ���)
	audio->audioClock += static_cast<double>(data_size) / (2 * audio->stream->codec->channels * audio->stream->codec->sample_rate);


	av_frame_free(&frame);
	swr_free(&swr_ctx);

	return data_size;
}