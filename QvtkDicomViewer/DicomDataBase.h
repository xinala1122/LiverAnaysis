#pragma once
#include <string>
#include <vector>
#include "DicomPatient.h"

/*
 *ʵ����,����Dicom�����ļ��Ķ�ȡ���ڴ�洢
 *
 *����:1.��DICOMDIR�ļ�����
 *	   2.�Ӻ���DICOMDIR�ļ����ļ��й���
 *	   3.�Ӻ���һ��Series���ļ��й���
 *	   4.�ӵ���ͼƬ����
 *	   
 *2017��10��28��21:37:38
 *	  Ŀǰֻ���Ǵ�DICOMDIR�ļ���������
 *2018��1��2��16:39:00
 *	  �������Ӵӵ���ͼƬ��ʼ���ʹ�series�ļ��г�ʼ���Ĺ���
 *	  ����������Ҫ�Ż�
 */

class DicomDataBase
{
public:
	static DicomDataBase* getInstance();
	void Init(std::string dir);//��DICOMDIR�ļ���ʼ��
	void InitFromSingleImage(std::string ImageFileName);//�ӵ���ͼƬ��ʼ��
	void InitFromSeriesFolder(std::string SeriesFolder);//��seriesfolder��ʼ��
private:
	//static DicomDataBase* m_pSingleton;
	DicomDataBase();
	~DicomDataBase();
public:
	std::vector<DicomPatient*> PatientList;//���ݿ��б�������еĲ���
	std::string DicomDirFilePath;//DicomDir�ļ��ľ���·��
	std::string DicomForderPath;//DicomDIR�ļ����ڵ��ļ���,�ں������RefFileID�͵õ�ͼƬ�ľ���·��
	DicomPatient*getPatientById(std::string &patientid);//ʹ��patient id��ѯһ��patient����
	DicomPatient*get_previous_patient(DicomPatient*current_patient); //��ȡǰһ������
	DicomPatient*get_latter_patient(DicomPatient*current_patient);//��ȡ��һ������
};
///����ģʽ
//DicomDataBase* DicomDataBase::m_pSingleton = new DicomDataBase();
