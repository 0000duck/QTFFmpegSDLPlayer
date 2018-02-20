#include "QTPlayer.h"
#include "Media.h"
QTPlayer::QTPlayer(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QObject::connect(Media::getInstance()->video,//�źŷ����Ķ���
		SIGNAL(updateFrame()),//�ź�
		ui.openGLWidget,//�۽��յĶ���
		SLOT(setVideoImage())//��
	);
}
