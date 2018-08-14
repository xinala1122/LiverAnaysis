#pragma once

#include <QWidget>
#include "ui_DicomDir.h"
#include "DicomDataBase.h"

/*
 * �����Ŀǰ�ǹ���DCMTK�ĸ��ֲ��Դ�������ڵ�
 * �ع�֮�󽫻��Ϊ��ȡDICOMDIR�ļ�����DicomDateBase�������ʵ�����͸�ֵ
 * �ڵ�һ���ع���ʱ��,���е���������֤������������
 * �ڵڶ����ع���ʱ��,�����Է�װ���е�DicomDatabase����,������������֤�ƶ�����������
 * �ڵ������ع���ʱ��,�ѽ����ҵ���߼�����
 */
class PatientMsg
{
public:
	QString FileID;
	QString PatientID;
	QString PatientName;
	QString BirthDate;
	QString Gender;
public:
	PatientMsg()
	{
		FileID = "NULL";
		PatientID = "NULL";
		PatientName = "NULL";
		BirthDate = "NULL";
		Gender = "NULL";
	}
};
class DicomDir : public QWidget
{
	Q_OBJECT

public:
	DicomDir(QWidget *parent = Q_NULLPTR);
	void InitDirExplorerFromDirPath(QString DicomDirFidlePath);//��dirfile����·����ʼ��
	void InitDirExplorerFromSingleFilePath(QString ImageFilePath);//�ӵ�ͼƬ�ļ��ľ���·����ʼ��
	void InitDirExplorerFromSeriesPath(QString SeriesPath);//��series���ļ���·����ʼ��
	~DicomDir();
private:
	Ui::DicomDir ui;
	DicomDataBase* m_database;
	void ConstructsTable();//������
signals:
	void sendData(QString Id);//�ź�,ͨ�����źŷ������˵�ID
public slots:
	void OnPushOk();
	void OnPushCancel();
};
