#include "DicomTreeItem.h"

/*
 * DicomTreeItem.cpp
 * DicomDir�����Զ���Item
 * ��DicomDirTreeModel���MVģʽ
 * Ԥ������ͼ���ܽ�ʹ���������ʵ��
 */

#include <QStringList>

/*
 * ���췽��
 */
DicomTreeItem::DicomTreeItem(const QVector<QVariant> &data, DicomTreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}
/*
 * ��������
 */
DicomTreeItem::~DicomTreeItem()
{
	qDeleteAll(childItems);
}
/*
 * ���ص�number���ӽڵ�
 */
DicomTreeItem *DicomTreeItem::child(int number)
{
	return childItems.value(number);
}
/*
 * �����ӽڵ������
 */
int DicomTreeItem::childCount() const
{
	return childItems.count();
}
/*
 * ��ǰ�ڵ����ֵܽڵ��е�����
 */
int DicomTreeItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<DicomTreeItem*>(this));

	return 0;
}
/*
 * ���ص�ǰ�ڵ��м�������
 */
int DicomTreeItem::columnCount() const
{
	return itemData.count();
}
/*
 *���ص�ǰ�ڵ������еĵ�column������
 */
QVariant DicomTreeItem::data(int column) const
{
	return itemData.value(column);
}
/*
 * �ڵ�ǰ�ڵ���ӽڵ��б�ĵ�positionλ�ò���count��ӵ��columns�������е��ӽڵ�
 * ע��ղ��ȥ���ӽڵ��ǿյ�
 */
bool DicomTreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		DicomTreeItem *item = new DicomTreeItem(data, this);
		childItems.insert(position, item);
	}

	return true;
}
/*
 * �ڵ�ǰ�ڵ�������е�positionλ�ò���columns������
 */
bool DicomTreeItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	foreach(DicomTreeItem *child, childItems)
		child->insertColumns(position, columns);

	return true;
}
/*
 * ���ص�ǰ�ڵ�ĸ��ڵ�
 */
DicomTreeItem *DicomTreeItem::parent()
{
	return parentItem;
}
/*
 * ɾ����ǰ�ڵ���ӽڵ��б�Ĵ�positionλ�ÿ�ʼ��count���ӽڵ�
 */
bool DicomTreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}
/*
 * ɾ����ǰ�ڵ������д�position��ʼ��columns��
 */
bool DicomTreeItem::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.remove(position);

	foreach(DicomTreeItem *child, childItems)
		child->removeColumns(position, columns);

	return true;
}
/*
 * ����ǰ�ڵ������е�column�д�д������value
 */
bool DicomTreeItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}
