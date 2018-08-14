#pragma once
#include <string>
#include <vector>
#include "DicomSeries.h"

/*
 * �洢����study������
 * ��Dicom��׼��,Studyָ����һϵ��series��ɵļ���,
 * һ��study�е�����series��ͬ���豸(��CT,MRI,PETCT��)���ɵ�,
 * ͬһ�����˿��ܻ�ͬʱ���ж����豸�ļ��,����ʱ��ÿ�����˾Ͷ�Ӧ���Study,
 * �������ֻ������һ�ּ��,��ֻ��һ��Study.
 */
class DicomStudy
{
public:
	DicomStudy();
	~DicomStudy();
public:
	std::string ModulationType;    //�������
	std::string InstitutionName;
	std::string PatientBirthDate;
	std::string PatientSex;
	std::string PatientAge;
	std::string StudyId;
	std::vector<DicomSeries*> SeriesList;//��Study�е�Series�б�
};

