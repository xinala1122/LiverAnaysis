#include "IniAccess.h"
#include <QSettings.h>

IniAccess::IniAccess(QString filePath)
    : m_filePath(filePath)
{
}

IniAccess::~IniAccess()
{
}

void IniAccess::Write(QString section, QString key, QString value)
{
    QSettings access(this->m_filePath, QSettings::IniFormat);
    access.setValue(section + "/" + key, value);
   // access.beginGroup(section);
   // access.setValue(key,value);
   // access.endGroup();
}

QString IniAccess::Read(QString section, QString key)
{
    QString stvalues;
    QSettings access(this->m_filePath, QSettings::IniFormat);
	access.setIniCodec("UTF-8");
	//access.setIniCodec("GBK");
    stvalues = access.value("/" + section + "/" + key).toString();
//    access.beginGroup(section);
//    stvalues = access.value(key).toString();
//    access.endGroup();
    return stvalues;
}

QStringList IniAccess::ReadKeys(QString section)
{
    QStringList keylist;
    QSettings access(this->m_filePath, QSettings::IniFormat);
    access.beginGroup(section);
    keylist = access.allKeys();
    access.endGroup();
    return keylist;
}
