#include "Startup.h"

Startup::Startup(QWidget *parent)
	: QWidget(parent)
{
	//�ޱ߿�,�����ڶ���
	//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
	ui.setupUi(this);
}

Startup::~Startup()
{
}
