#pragma once

#include <QWidget>
#include "ui_Segmenter.h"
#include "vtkImageSegmentCallback.h"

/*
 * ʵ�ַָ��
 * 
 * ��ʼ��:����һ��ͼƬ�ľ���·��
 * 
 */
class Segmenter : public QWidget
{
	Q_OBJECT

public:
	Segmenter(QWidget *parent = Q_NULLPTR);
	Segmenter(std::string Filename,QWidget *parent = Q_NULLPTR);
	~Segmenter();

private:
	Ui::Segmenter ui;
	std::string m_Filename;			//���ָ���ԭͼ�ľ���·��
	SegmentFunc m_segment_fun;		//�û�ѡ��ķָ��
signals:
	void SegmentFuncChange();		//�û�����ѡ��/�״�ѡ��ָ��㷨
public slots:
	void OnConnectedThreshold();	//��ͨ��ֵ����
	void OnWatershedThreshold();	//��ˮ��
	void OnNeighborhoodConnected(); //�������ӷ�
	void OnConfidenceConnected();	//�������ӷ�
	void OnFastMarching();			//����ƥ��
	void OnShapeDetection();		//��״���

	void OnSegmentFuncChange();		//��Ӧ�ָ���ı仯/�״�����
};
