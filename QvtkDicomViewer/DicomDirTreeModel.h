#pragma once
#include <QStandardItem>
#include "DicomTreeItem.h"
#include "DicomDataBase.h"

/*
 * QAbstractItemModel������
 * �ܹ���default�ļ������ݿ��α��ʼ��
 * "�α�"ָ���ǵ�ǰ������patient����
 * Ϊ�˷����Զ����������͵�һЩ�������,ʵ����һЩ����
 * ����:QAbstractItemModel�ǳ�����,����ʵ�����еĳ��󷽷�
 */
class DicomDirTreeModel:public QAbstractItemModel
{
	Q_OBJECT

public:
	DicomDirTreeModel(const QStringList &headers, const QString &data,QObject *parent = 0);
	DicomDirTreeModel(const QStringList &headers, const DicomPatient &patient, QObject *parent = 0);
	~DicomDirTreeModel();

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation,int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	bool setData(const QModelIndex &index, const QVariant &value,int role = Qt::EditRole) override;
	bool setHeaderData(int section, Qt::Orientation orientation,const QVariant &value, int role = Qt::EditRole) override;
	bool insertColumns(int position, int columns,const QModelIndex &parent = QModelIndex()) override;
	bool removeColumns(int position, int columns,const QModelIndex &parent = QModelIndex()) override;
	bool insertRows(int position, int rows,const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows,const QModelIndex &parent = QModelIndex()) override;
	int getLevel(const QModelIndex &index)const;
	DicomTreeItem *getItem(const QModelIndex &index) const;
private:
	void setupModelData(const QStringList &lines, DicomTreeItem *parent);
	void setupModelData(const DicomPatient &patient, DicomTreeItem *parent);
	
	DicomTreeItem *rootItem;
};

