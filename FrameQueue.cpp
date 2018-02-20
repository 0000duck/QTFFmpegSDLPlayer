
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
	
	cond.wakeOne();
	mutex.unlock();
	
	return true;
}

AVFrame * FrameQueue::deQueue()
{
	bool ret = true;
	AVFrame *tmp;
	mutex.lock();
	while (true)
	{
		if (!queue.empty())
		{
			 

			tmp = queue.front();
			queue.pop();

			 

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
	return tmp;
}