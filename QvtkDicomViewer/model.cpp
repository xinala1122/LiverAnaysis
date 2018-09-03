#include "model.h"
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include "IniAccess.h"

STLModel::STLModel(QObject *parent) :
    QObject(parent)
{   
    model_clear();
	InitColorParam();
}

void STLModel::InitColorParam()
{
	QSettings setting("config.ini", QSettings::IniFormat);
	QString curPath = QCoreApplication::applicationDirPath();

	QString iniFilePath = curPath + "/setup.ini";

	QFile file(iniFilePath);
	if (file.exists())
	{
		IniAccess iniAccess(iniFilePath);

		QMap<QString, QString> colorMap;
		QString key;
		QString value;
		value = QString::fromLocal8Bit("下腔静脉");
		key = QString::fromLocal8Bit("肝静脉血管");

		QString value1 = QString::fromLatin1("下腔静脉");
		//std::string value2 = value.toStdString();
		//value = QString::fromStdString(value2);
		QString keyItem = "STLFile";
		QString valueItem = iniAccess.Read("Path", keyItem);
		QString keyItem2 = "DicomFile";
		QString valueItem2 = iniAccess.Read("Path", keyItem2);
		QString xiaqiangjingmai = iniAccess.Read("Param", value);

		QString keyItemS = u8"下腔静脉";
		QString valueItemS = iniAccess.Read("Param", keyItemS);

		QString keyItemSTL = "xiaqiangjingmai";
		QString valueItemSTL = iniAccess.Read("Param", keyItemSTL);
		std::string strValueItemSTL = valueItemSTL.toStdString();
		//value = QString::fromLocal8Bit("肝静脉血管");
		//QString ganjingmaixueguan = iniAccess.Read("Param", value);

		//key = QString::fromLocal8Bit("肝静脉血管");
		//value = QString::fromLocal8Bit("肝静脉血管");
		//colorMap.insert(key, iniAccess.Read("Param", value));
		colorMap.insert("xiaqiangjingmai", iniAccess.Read("Param", "xiaqiangjingmai"));
		colorMap.insert("ganjingmaixueguan", iniAccess.Read("Param", "ganjingmaixueguan"));
		colorMap.insert("ganzang", iniAccess.Read("Param", "ganzang"));
		colorMap.insert("youbangan", iniAccess.Read("Param", "youbangan"));
		colorMap.insert("zuobangan", iniAccess.Read("Param", "zuobangan"));
		colorMap.insert("yishinangzhong", iniAccess.Read("Param", "yishinangzhong"));
		colorMap.insert("menjingmaixueguan", iniAccess.Read("Param", "menjingmaixueguan"));
		colorMap.insert("zhanwei2", iniAccess.Read("Param", "zhanwei2"));
		colorMap.insert("weizhimidu", iniAccess.Read("Param", "weizhimidu"));
		colorMap.insert("dongmaixueguan", iniAccess.Read("Param", "dongmaixueguan"));
		colorMap.insert("dannang", iniAccess.Read("Param", "dannang"));
		colorMap.insert("danzongguan", iniAccess.Read("Param", "danzongguan"));
		colorMap.insert("linbajie", iniAccess.Read("Param", "linbajie"));
		colorMap.insert("pizang", iniAccess.Read("Param", "pizang"));
		colorMap.insert("yizang", iniAccess.Read("Param", "yizang"));
		colorMap.insert("yiguan", iniAccess.Read("Param", "yiguan"));
		colorMap.insert("dannangweizhi", iniAccess.Read("Param", "dannangweizhi"));
		colorMap.insert("zhanwei1", iniAccess.Read("Param", "zhanwei1"));
		colorMap.insert("xueguanliu", iniAccess.Read("Param", "xueguanliu"));
		colorMap.insert("yishizhanwei", iniAccess.Read("Param", "yishizhanwei"));

		QStringList colorContentList;
		int sum;
		PerColorContent perColorContent;
		QList<QString> organNameList = colorMap.keys();
		for (int i = 0; i < organNameList.count();++i)
		{
			if (colorMap.value(organNameList.at(i)).length() > 3)
			{
				colorContentList = colorMap.value(organNameList.at(i)).split('.');
				if (colorContentList.length() == 4)
				{
					//m_colorContenList
					sum = colorContentList.at(1).toUInt() + colorContentList.at(2).toUInt() + colorContentList.at(3).toUInt();
					perColorContent.fileName = colorContentList.at(0);
					perColorContent.red = colorContentList.at(1).toUInt() / (float)sum;
					perColorContent.green = colorContentList.at(2).toUInt() / (float)sum;
					perColorContent.blue = colorContentList.at(3).toUInt() / (float)sum;
					this->m_colorContenMap.insert(organNameList.at(i), perColorContent);
				}
			}
		}

	}
	else
	{
		qDebug() << "Error: cannot find envconfig.ini file!";
	}
}

void STLModel::model_load(QString path, int numOfTotal, int total)
{
    //
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray header = file.read(80);
        file.close();

        if (header.trimmed().startsWith("solid") && header.contains("\n"))
        {
            qDebug("start to read text");
            model_readText(path);
        }
        else
        {
            qDebug("model : read Binary");
            model_readBinary(path);
        }

		if (numOfTotal == (total -1))
		{
			emit signal_finishLoad();
		}
    }
//    else
//    {
//        QMessageBox::information(0,tr("错误"),tr("未能成功加载文件"));
//    }
}

void STLModel::model_readText(QString path)
{
    model_clear();

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间

//    qDebug()<< time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式

    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QVector3D vn;
    QVector3D point1;
    QVector3D point2;
    QVector3D point3;
    QVector<QVector3D> triAngel;
    face *tempFace ;


    int count=0;

    QString line;
    QTextStream in(&file);
    line = in.readLine();                   // solid

    while(true)
    {
        line = in.readLine().trimmed();     //facet or endsolid
        if(line.trimmed().startsWith("endsolid"))
        {
            break;
        }
        vn = getCoordinateFromString(line);
        line = in.readLine();               //outer loop
        line = in.readLine().trimmed();     //vertex 1
        point1 = getCoordinateFromString(line);
        line = in.readLine().trimmed();     //vertex 2
        point2 = getCoordinateFromString(line);
        line = in.readLine().trimmed();     //vertex 3
        point3 = getCoordinateFromString(line);
        line = in.readLine();               //endloop
        line = in.readLine();               //endfacet

        triAngel.clear();
        triAngel.append(point1);
        triAngel.append(point2);
        triAngel.append(point3);
        //以下三句是更新模型的大小
        model_getSizeWhileReadPoint(point1);
        model_getSizeWhileReadPoint(point2);
        model_getSizeWhileReadPoint(point3);

        tempFace = new face;
        tempFace->setNormalVector(vn);
        tempFace->setTriAngle(triAngel);

        faceList.append(tempFace);
        count++;
    }


    time = QDateTime::currentDateTime();//获取系统现在的时间
//    qDebug()<< time.toString("yyyy-MM-dd hh:mm:ss ddd"); //设置显示格式
    file.close();
    //emit signal_finishLoad();
}


void STLModel::model_readBinary(QString path)
{
    model_clear();
    //http://qt-project.org/forums/viewthread/14726
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QVector3D vn;
    QVector3D point1;
    QVector3D point2;
    QVector3D point3;
    QVector<QVector3D> triAngel;
    face *tempFace;

    file.seek(0);
    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    quint32 n;
    quint16 control_bytes;
    quint32 count = 0;

    file.seek(80);
    in >> n;
    qDebug() << n << "triangles in the model.";

    float nx, ny, nz, x1, x2, x3, y1, y2, y3, z1, z2, z3;

    while (count < n)
    {
        file.seek(84+count*50+0+0);
        in >> nx;
        file.seek(84+count*50+0+4);
        in >> ny;
        file.seek(84+count*50+0+8);
        in >> nz;
        file.seek(84+count*50+12+0);
        in >> x1;
        file.seek(84+count*50+12+4);
        in >> y1;
        file.seek(84+count*50+12+8);
        in >> z1;
        file.seek(84+count*50+24+0);
        in >> x2;
        file.seek(84+count*50+24+4);
        in >> y2;
        file.seek(84+count*50+24+8);
        in >> z2;
        file.seek(84+count*50+36+0);
        in >> x3;
        file.seek(84+count*50+36+4);
        in >> y3;
        file.seek(84+count*50+36+8);
        in >> z3;
        file.seek(84+count*50+48);
        in >> control_bytes;

        if (control_bytes == 0 && in.status() == QDataStream::Ok)
        {
            vn = QVector3D(nx,ny,nz);
            point1= QVector3D(x1,y1,z1);
            point2= QVector3D(x2,y2,z2);
            point3= QVector3D(x3,y3,z3);

            triAngel.clear();
            triAngel.append(point1);
            triAngel.append(point2);
            triAngel.append(point3);

            //
            model_getSizeWhileReadPoint(point1);
            model_getSizeWhileReadPoint(point2);
            model_getSizeWhileReadPoint(point3);

            tempFace = new face;
            tempFace->setNormalVector(vn);
            tempFace->setTriAngle(triAngel);

            faceList.append(tempFace);

            count++;
        }
        else
        {
            qDebug() << "File read error when readStlBinaryFormat"
                     << (int) in.status();

        }
    }
    file.close();
    qDebug()<<tr("finish load");
    qDebug()<<faceList.size();
	QString file_full = path, file_name, file_path;
	QFileInfo fi;

	fi = QFileInfo(file_full);
	file_name = fi.fileName();
	file_path = fi.absolutePath();
	QStringList fileNameNoFormList = file_name.split('.');
	QString fileNameNoForm;
	if (fileNameNoFormList.length() == 2)
	{
		fileNameNoForm = fileNameNoFormList.at(0);
	}

	m_multifaceMap.insert(fileNameNoForm, faceList);
	faceList.clear();
    //emit signal_finishLoad();
}

void STLModel::model_clear(bool clearAll)
{
    if (clearAll)
    {
        faceList.clear();
		m_multifaceMap.clear();
    }

    size.smallX = 0;
    size.largeX = 0;
    size.smallY = 0;
    size.largeY = 0;
    size.smallZ = 0;
    size.largeZ = 0;
}

void STLModel::model_draw()
{
    QVector<QVector3D> triAngle;
    QVector3D normal;
    GLfloat normalVector[3];

	QList<QString> organNameList = m_multifaceMap.keys();
	for (int k = 0; k < organNameList.count();++k)
	{
		QList<QString> organColorList = m_colorContenMap.keys();
		for (int j = 0; j < organColorList.count();++j)
		{
			if (m_colorContenMap.value(organColorList.at(j)).fileName == organNameList.at(k))
			{
				float red = m_colorContenMap.value(organColorList.at(j)).red;
				float green = m_colorContenMap.value(organColorList.at(j)).green;
				float blue = m_colorContenMap.value(organColorList.at(j)).blue;
				QList<face*> faceListTemp = m_multifaceMap.value(organNameList.at(k));

				for(int i=0; i<faceListTemp.size(); i++)
				{
				normal = faceListTemp.at(i)->getNormalVector();
				normal.normalize();
				normalVector[0] = normal.x();
				normalVector[1] = normal.y();
				normalVector[2] = normal.z();
				triAngle = faceListTemp.at(i)->getTriAngle();
				glBegin(GL_TRIANGLES);
				glColor3f(red, green, blue);
				glNormal3fv(normalVector);
				glColor3f(red, green, blue);
				glVertex3f(triAngle.at(0).x(),triAngle.at(0).y(),triAngle.at(0).z() );
				glColor3f(red, green, blue);
				glVertex3f(triAngle.at(1).x(),triAngle.at(1).y(),triAngle.at(1).z() );
				glColor3f(red, green, blue);
				glVertex3f(triAngle.at(2).x(),triAngle.at(2).y(),triAngle.at(2).z() );
				glColor3f(red, green, blue);
				glEnd();
				}

				break;
			}
		}

		
	}

    /*for(int i=0; i<faceList.size(); i++)
    {
        normal = faceList.at(i)->getNormalVector();
        normal.normalize();
        normalVector[0] = normal.x();
        normalVector[1] = normal.y();
        normalVector[2] = normal.z();
        triAngle = faceList.at(i)->getTriAngle();
      glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glNormal3fv(normalVector);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(triAngle.at(0).x(),triAngle.at(0).y(),triAngle.at(0).z() );
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(triAngle.at(1).x(),triAngle.at(1).y(),triAngle.at(1).z() );
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(triAngle.at(2).x(),triAngle.at(2).y(),triAngle.at(2).z() );
        glColor3f(1.0f, 0.0f, 0.0f);
      glEnd();
    }*/

//        glBegin(GL_TRIANGLES);								// Drawing Using Triangles
//            glVertex3f( 0.0f, 1.0f, 0.0f);					// Top
//            glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
//            glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
//        glEnd();
}

void STLModel::model_test()
{
    QVector<QVector3D> test;
    if(faceList.isEmpty())
        return;
    for(int i=0;i<10;i++)
    {
        test = faceList.at(i)->getTriAngle();
    }
}

modelSize STLModel::model_getSize()
{
    return size;
}

void STLModel::model_getSizeWhileReadPoint(QVector3D point)
{
    float pointX = point.x();
    float pointY = point.y();
    float pointZ = point.z();
   //
   if(pointX < size.smallX)
       size.smallX = pointX;
   else if(pointX > size.largeX)
       size.largeX = pointX;
   //
   if(pointY < size.smallY)
       size.smallY = pointY;
   else if(pointY > size.largeY)
       size.largeY = pointY;
   //
   if(pointZ < size.smallZ)
       size.smallZ = pointZ;
   else if(pointZ > size.largeZ)
       size.largeZ = pointZ;
}

QVector3D STLModel::model_getCenter(modelSize size)
{
    QVector3D center;
    center.setX( (size.largeX + size.smallX)/2 );
    center.setY( (size.largeY + size.smallY)/2 );
    center.setZ( (size.largeZ + size.smallZ)/2 );
    return center;
}

int STLModel::model_getFacesCount()
{
    return faceList.size();
}

QVector3D STLModel::getCoordinateFromString(QString line)
{
     QVector3D coordinate;
     QVector<float> point;

     QStringList strList = line.split(" ");

    int count = strList.size();
    for(int i=count-3;i<count;i++)
    {
        point.append(strList[i].toDouble());

    }
    coordinate.setX(point[0]);
    coordinate.setY(point[1]);
    coordinate.setZ(point[2]);
    return coordinate;
}
