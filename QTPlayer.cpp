#include "QTPlayer.h"
#include "DisplayMediaThread.h"
QTPlayer::QTPlayer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QObject::connect(DisplayMediaThread::getInstance(),//�źŷ����Ķ���
		SIGNAL(updateFrame(QImage)),//�ź�
		ui.openGLWidget,//�۽��յĶ���
		SLOT(setVideoImage(QImage))//��
	);
}
