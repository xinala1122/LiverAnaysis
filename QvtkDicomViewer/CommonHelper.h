#pragma once
#include <QString>
#include <QFileDialog>
#include <qapplication.h>

/*
 * ���ڴ��һЩͨ�õĹ��߷���
 * ������Դ����,���õĸ�������ת����
 * ע�����Ķ��ⷽ��Ҫ������Ǿ�̬��
 * ԭ���ϱ�����Ӧ�þ���û�����Ժ��ֶ�
 */
class CommonHelper
{
public:
	CommonHelper();
	~CommonHelper();
	static void setStyle(const QString& style);//�������Ա�
	static bool isADicomOrDicomDir(std::string filename);//�����ж�һ���ļ��Ƿ�Ϊ��ѭDicom3.0��׼��ͼƬ�ļ���Dir�ļ�
};

