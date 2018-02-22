
#include "FrameQueue.h"

FrameQueue::FrameQueue()
{	 
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

int FrameQueue::getQueueSize()
{
	return queue.size();	
}

void FrameQueue::queueFlush() {
	while (!queue.empty())
	{		
		AVFrame *frame = deQueue();
		av_frame_unref(frame);
	}
}