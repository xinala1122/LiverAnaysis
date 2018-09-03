#include "mywidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>


myWidget::myWidget(QvtkDicomViewer *mainWindow, QWidget *parent) :
    QWidget(parent)
{
    //this->m_MainWindow = mainWindow;
    opengl = new openglWidget(this);
    //openstl = new QPushButton(this);
    //openstl->setText(tr("打开文件"));
    moveLeft = new QPushButton(tr("左移"));
    moveRight = new QPushButton(tr("右移"));
    moveUp = new QPushButton(tr("上移"));
    moveDown = new QPushButton(tr("下移"));
    //clearDown = new QPushButton(tr("清除"));

    connect(moveLeft,SIGNAL(clicked()),
            opengl,SLOT(slot_moveLeft()));
    connect(moveRight,SIGNAL(clicked()),
            opengl,SLOT(slot_moveRight()));
    connect(moveUp,SIGNAL(clicked()),
            opengl,SLOT(slot_moveUp()));
    connect(moveDown,SIGNAL(clicked()),
            opengl,SLOT(slot_moveDown()));
    //connect(clearDown, SIGNAL(clicked()),
    //        opengl, SLOT(slot_clear()));


    QHBoxLayout *hbox = new QHBoxLayout;
    //hbox->addWidget(openstl);
    hbox->addWidget(moveLeft);
    hbox->addWidget(moveRight);
    hbox->addWidget(moveUp);
    hbox->addWidget(moveDown);
    //hbox->addWidget(clearDown);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(opengl);
    vbox->addLayout(hbox);
    setLayout(vbox);


    //connect(openstl,SIGNAL(clicked()),
    //        this,SLOT(slot_openStl()));

    connect(this,SIGNAL(signal_loadStl(QString, int, int)),
            opengl,SLOT(slot_recieveStl(QString, int, int)));

    connect(opengl,SIGNAL(SigClearTree()),
            this,SIGNAL(SigClearTree()));
}

void myWidget::slot_openStl(QString stlFilePath)
{
    //加载一个stl文件  设置打开文件的过滤器
    //QString path;
    //path = QFileDialog::getOpenFileName(this,"open file dialog", "/","STL files (*.stl *.ini)");
	//QString dir = QFileDialog::getExistingDirectory(this, QStringLiteral("打开Series目录"), "F:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	QString dir = stlFilePath;
	if (dir.isEmpty() == true)
		return;
	QDir  folder(dir);
	QStringList files = folder.entryList(QDir::Files | QDir::Readable, QDir::Name);//获取文件夹中的全部文件的名字
	//QString Prefix = folder->absolutePath();//构造文件名前缀
	//Prefix.append("\\");
	//Prefix.replace(QChar('\\'), QChar('/'));
	QStringList filters;
	filters << QString("*.stl");
	folder.setFilter(QDir::Files | QDir::NoSymLinks); //设置类型过滤器，只为文件格式
	folder.setNameFilters(filters);  //设置文件名称过滤器，只为filters格式

								  //统计cfg格式的文件个数
	int dir_count = folder.count();
	if (dir_count <= 0)
		return;
    //然后将路径传递给  opengl，opengl里面在调用model->load
	for (int i = 0; i < dir_count; i++)
	{
		QString file_name = dir + QChar('/') + folder[i];  //文件名称
		if (!file_name.isEmpty())
		{
			emit signal_loadStl(file_name, i, dir_count);
		}
	}


}

void myWidget::openOutside(QString path)
{
    //emit signal_loadStl(path);
}
