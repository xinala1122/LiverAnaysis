#pragma once
/*
 * DicomTreeItem.cpp
 * DicomDir�����Զ���Item
 * ��DicomDirTreeModel���MVģʽ
 * Ԥ������ͼ���ܽ�ʹ���������ʵ��
 */
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <QStandardItem>


class DicomTreeItem:public QStandardItem
{
public:
	explicit DicomTreeItem(const QVector<QVariant> &data, DicomTreeItem *parent = 0);
	~DicomTreeItem();

	DicomTreeItem *child(int number);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	DicomTreeItem *parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);

private:
	QList<DicomTreeItem*> childItems;
	DicomTreeItem *parentItem;
public:
	QVector<QVariant> itemData;
};


#endif // TREEITEM_H
