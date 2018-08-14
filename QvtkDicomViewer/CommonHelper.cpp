#include "CommonHelper.h"
#include <sstream>
#include <iostream>
#include <ppltasks.h>
#include <fstream>

CommonHelper::CommonHelper()
{
}


CommonHelper::~CommonHelper()
{
}
/*
 * �������Ա�
 */
void CommonHelper::setStyle(const QString& style)
{
	QFile qss(style);
	qss.open(QFile::ReadOnly);
	qApp->setStyleSheet(qss.readAll());
	qss.close();
}
/*
 * �����ж�һ���ļ��Ƿ�Ϊ��ѭDicom3.0��׼��ͼƬ�ļ���Dir�ļ�
 * �ú���������,�����ʹ��.
 * ��ĳЩ�������������,����ɰѿյ��ı��ĵ������Ϸ����ļ�,
 * ʵ��������������ֻ��У����ǰ128�ֽڵ���Ϣ
 */
 bool CommonHelper::isADicomOrDicomDir(std::string filename)
 {
	
	 std::filebuf *pbuf;
	 std::ifstream filestr;
	 long size;
	 char * buffer;

	 filestr.open(filename, std::ios::binary);// �����ƴ� 
										 // ��ȡfilestr��Ӧbuffer�����ָ��  
	 pbuf = filestr.rdbuf();

	 //����buffer���󷽷���ȡ�ļ���С 
	 size = pbuf->pubseekoff(0, std::ios::end, std::ios::in);
	 //size = 132;
	 pbuf->pubseekpos(0, std::ios::in);

	 // �����ڴ�ռ� 
	 buffer = new char[size];

	 // ��ȡ�ļ����� 
	 pbuf->sgetn(buffer, size);

	 filestr.close();
	 //std::cout.write(buffer, size);
	 // ʹ��
	 int b0 = buffer[0] & 255, b1 = buffer[1] & 255, b2 = buffer[2] & 255, b3 = buffer[3] & 255;

	 if (buffer[128] == 68 && buffer[129] == 73 && buffer[130] == 67 && buffer[131] == 77)
	 {
		 return true;
	 }
	 else if ((b0 == 8 || b0 == 2) && b1 == 0 && b3 == 0)
	 {
		 return true;
	 }
	 return false;

	 delete[]buffer;
	 delete[]buffer;
 }
