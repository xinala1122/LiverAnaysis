#pragma once
#include <string>
#include <vector>
#include "DicomStudy.h"

/*
 * ����洢�Ͳ�����ص�һЩ��Ϣ
 * ��Dicom��׼��,һ�����ݿ��д���Ŷ�����˵���Ϣ
 * ÿ�����˿�����1�����Study,ÿ��Study������1�����Series,ÿ��Series�����ж���Image,
 * ͬһ�����˵������¼���Ϣ�Ĳ��������Ͳ���ID����ͬ��
 * ע��,��Ҳ����������ӡ��,���ǹ��ڲ��˵ĸ�����Ϣ����������ڲ�������㼶��
 * ���ǿ��ǵ�,һ���˵�һ�������ܶ�μ��,ÿ�μ�鶼�п���ʹ��1������ҽѧӰ���ֶ�,������������Ϣ
 * ��������,���ʱ��,�����Ա�,���ǲ��̶���,��Щ��ϢҪ����ÿ�μ����䶯,
 * ����Dicom��,������Ϊÿ��������Ψһ��PatientID��Ϊʶ��,ֻҪID��ͬ,���н���PACSϵͳ��ҽѧӰ�񶼽�
 * ����ͬһ��Ŀ¼������
 * �������ǲ����ǲ��˸��������
 */
class DicomPatient
{
public:
	DicomPatient();//����
	DicomPatient(DicomPatient*old);//�������캯��
	~DicomPatient();
public:
	std::string PatientName;
	std::string PatientID;
	std::vector<DicomStudy*> StudyList; //������˵�Study�б�
	int indexOfCurrentStudy; //��ǰ���ڶ�ȡ��Study��������.��0��ʼ,Ĭ��Ϊ0
	int indexOfCurrentSeries;//��ǰ���ڶ�ȡ��Series��������,��0��ʼ,Ĭ��Ϊ0
	int indexOfCurrentImage; //��ǰ���ڶ�ȡ��Image��������,��0��ʼ,Ĭ��Ϊ0

	void reset();//��λ

	DicomStudy*getCurrentDicomStudy();//��ȡ��ǰStudy
	void setCurrentDicomStudyById(std::string &studyid);//ͨ��studyid���õ�ǰ��study

	DicomSeries* getCurrentDicomSeries();//��ȡ��ǰSeries
	void setCurrentDicomSeriesById(std::string &seriesnum);//ͨ��seriesnum���õ�ǰ��Series

	DicomImage* getCurrentDicomImage();//��ȡ��ǰImage
	DicomImage* getNextDicomImage();//��ȡ��һ��Image,���ƶ�ָ��,Խ���Զ��ص���һ��
	DicomImage* getDicomImageByRfid(std::string &rfid);//ͨ��RFID�ڵ�ǰ���˵���Ϣ�м�����һ��ͼƬ
	DicomImage* getDicomImageByIndex(int index);//ȡ��ǰstudy,��ǰseries�ĵ�index��ͼƬ,���Խ��,����ԭ��������ָ�����һ��ͼƬ
};

