#ifndef INIACCESS_H
#define INIACCESS_H
#include <QString.h>
#include <QStringList.h>

class IniAccess
{
public:
    IniAccess(QString filePath);
    virtual ~IniAccess();
    void Write(QString section, QString key, QString value);
    QString Read(QString section, QString key);
    QStringList ReadKeys(QString section);
private:
    QString     m_filePath;
};

#endif // INIACCESS_H
