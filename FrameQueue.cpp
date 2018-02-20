
#include "FrameQueue.h"

FrameQueue::FrameQueue()
{
	nb_frames = 0;
}

bool FrameQueue::enQueue(const AVFrame* frame)
{
	AVFrame* p = av_frame_alloc();

	int ret = av_frame_ref(p, frame);
	if (ret < 0)
		return false;

	p->opaque = (void *)new double(*(double*)p->opaque); //��һ��ָ�����һ���ֲ��ı������������·���pts�ռ�

	mutex.lock();
	queue.push(p);

	nb_frames++;
	
	cond.notify_one();
	mutex.unlock();
	
	return true;
}

bool FrameQueue::deQueue(AVFrame **frame)
{
	bool ret = true;

	mutex.lock();
	while (true)
	{
		if (!queue.empty())
		{
			if (av_frame_ref(*frame, queue.front()) < 0)
			{
				ret = false;
				break;
			}

			auto tmp = queue.front();
			queue.pop();

			av_frame_free(&tmp);

			nb_frames--;

			ret = true;
			break;
		}
		else
		{
			cond.wait(&mutex);
		}
	}

	mutex.unlock();
	return ret;
}