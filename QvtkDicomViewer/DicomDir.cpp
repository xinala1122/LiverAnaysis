#include "DicomDir.h"

#include <dcmdata/dcdicdir.h>
#include <dcmdata/dcdeftag.h>
#include <QTableWidget>
#include <QDebug>
#include<QFileSystemModel>

#include<iomanip>
#include<fstream>
#include "DicomDataBase.h"

/*
 *Ĭ�Ϲ��췽��
 */

DicomDir::DicomDir(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}
//��dirfile����·����ʼ��
/*
 * ע��,ֻ������������²���Ҫ��ʾ���沢������
 */
void DicomDir::InitDirExplorerFromDirPath(QString DicomDirFidlePath)
{
	m_database = DicomDataBase::getInstance();
	m_database->Init(DicomDirFidlePath.toStdString());
	ConstructsTable();
}
//�ӵ�ͼƬ�ļ��ľ���·����ʼ��
void DicomDir::InitDirExplorerFromSingleFilePath(QString ImageFilePath)
{
	m_database = DicomDataBase::getInstance();
	m_database->InitFromSingleImage(ImageFilePath.toStdString());
	emit sendData(QString::fromStdString(m_database->PatientList.at(0)->PatientID));
	//��ȡPatientID���ֶ����ź�
	//ConstructsTable();
}
//��series���ļ���·����ʼ��
void DicomDir::InitDirExplorerFromSeriesPath(QString SeriesPath)
{
	m_database = DicomDataBase::getInstance();
	m_database->InitFromSeriesFolder(SeriesPath.toStdString());
	emit sendData(QString::fromStdString(m_database->PatientList.at(0)->PatientID));
	//��ȡPatientID���ֶ����ź�
	//ConstructsTable();
}
/*
 * ��������
 */
DicomDir::~DicomDir()
{

}
//������
void DicomDir::ConstructsTable()
{
	/*
	*������
	*/
	ui.tableWidget->clear();
	if (m_database->PatientList.size() == 0)
	{
		return;
	}
	else
	{
		//����,������
		ui.tableWidget->setColumnCount(4);
		ui.tableWidget->setRowCount(m_database->PatientList.size());//����
		int size = m_database->PatientList.size();
		ui.tableWidget->setHorizontalHeaderLabels(
			QStringList() << "Patient ID" << "Patient Name" << "Birth Date" << "Gender");
		ui.tableWidget->verticalHeader()->setVisible(false); // ����ˮƽheader
															 //ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);   // ����ѡ��
		ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
		//ui.tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);  // ����ѡ�ж�� 
		ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
		/* ���� Id Name �ֶε�ֵ�����޸� */
		/* ���ò����޸ĵ�Id Name���еı�����ɫΪ��ɫ */
		for (int i = 0; i < 4; i++) {
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setBackground(QBrush(QColor(Qt::lightGray)));
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(i, 0, item);
		}
		for (int i = 0; i < 4; i++) {
			QTableWidgetItem *item = new QTableWidgetItem();
			item->setBackground(QBrush(QColor(Qt::lightGray)));
			item->setFlags(item->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(i, 1, item);
		}
		//������ݿ�ʼ
		for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
			QTableWidgetItem *item_2 = new QTableWidgetItem();
			item_2->setText(QString::fromStdString(m_database->PatientList[i]->PatientID));
			ui.tableWidget->setItem(i, 0, item_2);
		}

		for (int i = 0; i < ui.tableWidget->rowCount(); i++) {
			QTableWidgetItem *item_max = new QTableWidgetItem();
			item_max->setText(QString::fromStdString(m_database->PatientList[i]->PatientName));
			item_max->setBackground(QBrush(QColor(Qt::lightGray)));
			item_max->setFlags(item_max->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(i, 1, item_max);

			QTableWidgetItem *item_min = new QTableWidgetItem();
			item_min->setText(QString::fromStdString(m_database->PatientList[i]->StudyList[0]->PatientBirthDate));
			item_min->setBackground(QBrush(QColor(Qt::lightGray)));
			item_min->setFlags(item_min->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(i, 2, item_min);

			QTableWidgetItem *item_type = new QTableWidgetItem();
			item_type->setText(QString::fromStdString(m_database->PatientList[i]->StudyList[0]->PatientSex));
			item_type->setBackground(QBrush(QColor(Qt::lightGray)));
			item_type->setFlags(item_type->flags() & (~Qt::ItemIsEditable));
			ui.tableWidget->setItem(i, 3, item_type);
		}
	}
}
/*
 * ���ȷ������
 */
void DicomDir::OnPushOk()
{
	/*
	 * ����ǰѡ���ж�Ӧ�Ĳ��˵�PatientID��DICOMDIR�ľ���·�����ַ������ص����յ�
	 */
	this->close();
	emit sendData(ui.tableWidget->item(ui.tableWidget->currentItem()->row(), 0)->text());
	
}
/*
 * ���ȡ������
 */
void DicomDir::OnPushCancel()
{
	this->close();
}
