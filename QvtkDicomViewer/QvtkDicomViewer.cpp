#include "myVtkInteractorStyleImage.h"
#include "vtkBiDimensionalCallback.h"
#include "DicomDir.h"
#include "Segmenter.h"
#include <QListView>
#include "QvtkDicomViewer.h"
#include <QMessageBox>
#include <qDebug>
#include <QComboBox>
#include <QStandardItem>
#include <QTableView>
#include <QAction>
#include <QMenu>

#include <vtkActor2D.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>
#include <vtkDistanceWidget.h>
#include <vtkDistanceRepresentation.h>
#include <vtkAngleWidget.h>
#include <vtkImageChangeInformation.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include "vtkImageData.h"
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkImageIterator.h>
#include <vtkStringArray.h>
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolume.h"
#include <vtkColorTransferFunction.h>
#include "vtkVolumeRayCastMapper.h"
#include <vtkPiecewiseFunction.h>
#include "vtkStringArray.h"
#include "vtkUnicodeStringArray.h"
#include "vtkImageAppend.h"

#include <dcmtk\config\osconfig.h>
#include <dcmtk\dcmdata\dctk.h>
#include "dcmtk\dcmdata\dcistrmf.h"
#include "DicomDataBase.h"
#include <QFileSystemModel>
#include "DicomDirTreeModel.h"

#include <vtkImageSegmentCallback.h>
#include <vtkDecimatePro.h>
#include "ThreeD_Reconstruction.h"


//���췽��
QvtkDicomViewer::QvtkDicomViewer(QWidget *parent)
	: QMainWindow(parent)
{
	//�ޱ߿�,�����ڶ���
	//setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint);
	ui.setupUi(this);
	ui.action_Pointer->setChecked(true);
	CursorType = CURSOR::POINTRE;
	//AppState = Idel;//��ʼ״̬,�����Դ��ڿ���״̬
	connect(this, SIGNAL(AppStateChanged()), this, SLOT(OnChangeAppState()));
	setAppState(Idel);
	//��ع�����͵��޸�
	connect(this, SIGNAL(CursorValueChanged()), this, SLOT(OnChangeCursorValue()));
	ui.action_SwitchOfProperty->setChecked(true);
	ui.dockWidget_Dir->setHidden(false);
	icon_Play.addFile(QStringLiteral(":/QvtkDicomViewer/Resources/play_128px_1197036_easyicon.net.ico"), QSize(), QIcon::Normal, QIcon::Off);
	icon_Pause.addFile(QStringLiteral(":/QvtkDicomViewer/Resources/pause_128px_1197034_easyicon.net.ico"), QSize(), QIcon::Normal, QIcon::Off);
	PlayFlag = false;
	ui.action_Stop->setEnabled(false);
	//�����Ĳ˵�
	CreateContextMenu();
	//DirTreeRefresh(NULL);
	setContentsMargins(0, 0, 0, 0);
	ui.centralWidget->setContentsMargins(0, 0, 0, 0);
	ui.qvtkWidget->setContentsMargins(0, 0, 0, 0);
	
	m_pTimer = new QTimer();
	m_pTimer->setInterval(30);//�������
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(OnPlayerTimerOut()));

	this->m_myWidget = new myWidget(this);
	connect(this, SIGNAL(SigOpenStlFile()), this->m_myWidget, SLOT(slot_openStl()));

	connect(this->m_myWidget, SIGNAL(signal_loadStl(QString)), this, SLOT(OnProjectLoadFinish(QString)));
	connect(this->m_myWidget, SIGNAL(SigClearTree()), this, SLOT(OnClearTree()));

	QHBoxLayout *hlayout = new QHBoxLayout;//����һ��ˮƽ���ֹ�����

	//hlayout->addWidget(ui.centralWidget);
	hlayout->addWidget(this->m_myWidget);
	//
	this->ui.StlWidget->setLayout(hlayout);//��ˮƽ����������������
	this->ui.StlWidget->setVisible(false);
}
//��������ʱ����
void QvtkDicomViewer::OnPlayerTimerOut()
{
	ShowImageByIndex(current);
	current++;
	if (current>max)
	{
		current = min;
	}
}

// ��Ӧ����״̬�ĸ���,ִ��һЩ����/����/ˢ�²���
void QvtkDicomViewer::OnChangeAppState()
{
	switch (AppState)
	{
	case Idel:		  //����״̬
		ui.action_PreviousPatient->setEnabled(false);
		ui.action_LatterPatient->setEnabled(false);
		ui.action_back->setEnabled(false);
		ui.action_Play->setEnabled(false);
		ui.action_next->setEnabled(false);
		ui.action_Stop->setEnabled(false);
		ui.action_Pointer->setEnabled(false);
		ui.action_Zoom->setEnabled(false);
		ui.action_GrayLevel->setEnabled(false);
		ui.action_Move->setEnabled(false);
		ui.action_Protractor->setEnabled(false);
		ui.action_Ruler->setEnabled(false);
		ui.action_Contour->setEnabled(false);
		ui.action_BiDimensional->setEnabled(false);
		ui.action_Negative->setEnabled(false);
		ui.action_Reset->setEnabled(false);
		ui.SliceScrollBar->setEnabled(false);
		ui.menu_WindowSizeLevel->setEnabled(false);
		break;
	case SingleImage: //����ͼƬ״̬
		ui.action_PreviousPatient->setEnabled(false);
		ui.action_LatterPatient->setEnabled(false);
		ui.action_back->setEnabled(false);
		ui.action_Play->setEnabled(false);
		ui.action_next->setEnabled(false);
		ui.action_Stop->setEnabled(false);
		ui.action_Pointer->setEnabled(true);
		ui.action_Zoom->setEnabled(true);
		ui.action_GrayLevel->setEnabled(true);
		ui.action_Move->setEnabled(true);
		ui.action_Protractor->setEnabled(true);
		ui.action_Ruler->setEnabled(true);
		ui.action_Contour->setEnabled(true);
		ui.action_BiDimensional->setEnabled(true);
		ui.action_Negative->setEnabled(true);
		ui.action_Reset->setEnabled(true);
		ui.SliceScrollBar->setEnabled(false);
		ui.menu_WindowSizeLevel->setEnabled(true);
		break;
	case Folder:	  //��series�ļ���״̬
		ui.action_PreviousPatient->setEnabled(false);
		ui.action_LatterPatient->setEnabled(false);
		ui.action_back->setEnabled(true);
		ui.action_Play->setEnabled(true);
		ui.action_next->setEnabled(true);
		ui.action_Stop->setEnabled(true);
		ui.action_Pointer->setEnabled(true);
		ui.action_Zoom->setEnabled(true);
		ui.action_GrayLevel->setEnabled(true);
		ui.action_Move->setEnabled(true);
		ui.action_Protractor->setEnabled(true);
		ui.action_Ruler->setEnabled(true);
		ui.action_Contour->setEnabled(true);
		ui.action_BiDimensional->setEnabled(true);
		ui.action_Negative->setEnabled(true);
		ui.action_Reset->setEnabled(true);
		ui.SliceScrollBar->setEnabled(true);
		ui.menu_WindowSizeLevel->setEnabled(true);
		break;	
	case Dir:		  //��DICOMDIR״̬
		ui.action_PreviousPatient->setEnabled(true);
		ui.action_LatterPatient->setEnabled(true);
		ui.action_back->setEnabled(true);
		ui.action_Play->setEnabled(true);
		ui.action_next->setEnabled(true);
		ui.action_Stop->setEnabled(true);
		ui.action_Pointer->setEnabled(true);
		ui.action_Zoom->setEnabled(true);
		ui.action_GrayLevel->setEnabled(true);
		ui.action_Move->setEnabled(true);
		ui.action_Protractor->setEnabled(true);
		ui.action_Ruler->setEnabled(true);
		ui.action_Contour->setEnabled(true);
		ui.action_BiDimensional->setEnabled(true);
		ui.action_Negative->setEnabled(true);
		ui.action_Reset->setEnabled(true);
		ui.SliceScrollBar->setEnabled(true);
		ui.menu_WindowSizeLevel->setEnabled(true);
		break;	
	case Err:		  //�������ش����״̬
		ui.action_PreviousPatient->setEnabled(false);
		ui.action_LatterPatient->setEnabled(false);
		ui.action_back->setEnabled(false);
		ui.action_Play->setEnabled(false);
		ui.action_next->setEnabled(false);
		ui.action_Stop->setEnabled(false);
		ui.action_Pointer->setEnabled(false);
		ui.action_Zoom->setEnabled(false);
		ui.action_GrayLevel->setEnabled(false);
		ui.action_Move->setEnabled(false);
		ui.action_Protractor->setEnabled(false);
		ui.action_Ruler->setEnabled(false);
		ui.action_Contour->setEnabled(false);
		ui.action_BiDimensional->setEnabled(false);
		ui.action_Negative->setEnabled(false);
		ui.action_Reset->setEnabled(false);
		ui.SliceScrollBar->setEnabled(false);
		ui.menu_WindowSizeLevel->setEnabled(false);
		break;		
	case Debug:		  //����״̬(ȫ������)
		ui.action_PreviousPatient->setEnabled(true);
		ui.action_LatterPatient->setEnabled(true);
		ui.action_back->setEnabled(true);
		ui.action_Play->setEnabled(true);
		ui.action_next->setEnabled(true);
		ui.action_Stop->setEnabled(true);
		ui.action_Pointer->setEnabled(true);
		ui.action_Zoom->setEnabled(true);
		ui.action_GrayLevel->setEnabled(true);
		ui.action_Move->setEnabled(true);
		ui.action_Protractor->setEnabled(true);
		ui.action_Ruler->setEnabled(true);
		ui.action_Contour->setEnabled(true);
		ui.action_BiDimensional->setEnabled(true);
		ui.action_Negative->setEnabled(true);
		ui.action_Reset->setEnabled(true);
		ui.SliceScrollBar->setEnabled(true);
		ui.menu_WindowSizeLevel->setEnabled(true);
		break;	
	default:
		break;
	}
}

//��Ӧ���ֵ���޸�,ִ��һЩˢ�ºͽ��ò���
void QvtkDicomViewer::OnChangeCursorValue()
{
	biDimensionalWidget->EnabledOff();
	contourWidget->EnabledOff();
	distanceWidget->EnabledOff();
	angleWidget->EnabledOff();

	ui.action_BiDimensional->setChecked(false);
	ui.action_Contour->setChecked(false);
	ui.action_Ruler->setChecked(false);
	ui.action_Protractor->setChecked(false);
	ui.action_Pointer->setChecked(false);

	ui.action_GrayLevel->setChecked(false);
	ui.action_Zoom->setChecked(false);
	ui.action_Move->setChecked(false);
	switch (CursorType)
	{
	case POINTRE://Ĭ��ָ��
		ui.action_Pointer->setChecked(true);
		break;
	case ZOOM://����
		ui.action_Zoom->setChecked(true);
		break;
	case GRAYLEVEL://�ҽ�
		ui.action_GrayLevel->setChecked(true);
		break;//������
	case PROTRACTOR:
		angleWidget->EnabledOn();
		angleWidget->SetWidgetStateToStart();
		ui.action_Protractor->setChecked(true);
		break;
	case RULER://����
		distanceWidget->EnabledOn();
		distanceWidget->SetWidgetStateToStart();
		ui.action_Ruler->setChecked(true);
		break;
	case CONTOUR://����
		contourWidget->EnabledOn();
		//splineDistanceWidget->EnabledOn();
		ui.action_Contour->setChecked(true);
		break;
	case BIDI://��ά��
		biDimensionalWidget->EnabledOn();
		biDimensionalWidget->SetWidgetStateToStart();
		ui.action_BiDimensional->setChecked(true);
		break;
	case MOVE:
		ui.action_Move->setChecked(true);
		break;
	default:
		break;
	}
}

//��Ӧ����λģʽֵ���޸�,ִ��һЩ���ú�ѡ������
void QvtkDicomViewer::OnChangeWindowsWL()
{

	ui.action_WindowWL_Default->setChecked(false);
	ui.action_WindowWL_All->setChecked(false);
	ui.action_WindowWL_CT_Abdomen->setChecked(false);
	ui.action_WindowWL_CT_BloodVessel->setChecked(false);
	ui.action_WindowWL_CT_Bones->setChecked(false);
	ui.action_WindowWL_CT_Brain->setChecked(false);
	ui.action_WindowWL_CT_Medias->setChecked(false);
	ui.action_WindowWL_CT_Lungs->setChecked(false);

	switch (ImageWindow)
	{
	case Default:
		ui.action_WindowWL_Default->setChecked(true);
		break;
	case All:
		ui.action_WindowWL_All->setChecked(true);
		break;
	case Abdomen:
		ui.action_WindowWL_CT_Abdomen->setChecked(true);
		break;
	case BloodVessel:
		ui.action_WindowWL_CT_BloodVessel->setChecked(true);
		break;
	case Bones:
		ui.action_WindowWL_CT_Bones->setChecked(true);
		break;
	case Brain:
		ui.action_WindowWL_CT_Brain->setChecked(true);
		break;
	case Medias:
		ui.action_WindowWL_CT_Medias->setChecked(true);
		break;
	case Lungs:
		ui.action_WindowWL_CT_Lungs->setChecked(true);
		break;
	}
}

//��Series�ļ���
void QvtkDicomViewer::OnOpenSeriesFolder()
{
	//��ȡSeries�ļ��еľ���·����
	QString dir = QFileDialog::getExistingDirectory(this, QStringLiteral("��SeriesĿ¼"), "F:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (dir.isEmpty() == true)
		return;
	this->ui.qvtkWidget->setVisible(true);
	this->ui.StlWidget->setVisible(false);

	DicomDir *m_dicomdir = new DicomDir();
	connect(m_dicomdir, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
	m_dicomdir->InitDirExplorerFromSeriesPath(dir);

	//m_dicomdir->show();
	setAppState(Folder);//�������Folder״̬
}

//�򿪵���Dicom�ļ�
void QvtkDicomViewer::OnOpenDicomFile()
{
	//���ļ�ѡ��ҳ��
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��DICOM�ļ�"), ".", QStringLiteral("ȫ������(*.*)"));
	if (path.isEmpty() == true)
		return;
	/*
	 * ��֤�ǲ���������DICOMͼƬ�ļ�
	 */
	this->ui.qvtkWidget->setVisible(true);
	this->ui.StlWidget->setVisible(false);

	DicomDir *m_dicomdir = new DicomDir();
	connect(m_dicomdir, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
	m_dicomdir->InitDirExplorerFromSingleFilePath(path);
	setAppState(SingleImage);//�������SingleImage״̬
}

//��DICOMDIR�ļ�
void QvtkDicomViewer::OnOpenDicomDirFile()
{							  
	this->ui.qvtkWidget->setVisible(false);
	this->ui.StlWidget->setVisible(true);
	emit SigOpenStlFile();
	////���ļ�ѡ��ҳ��
	//QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��DICOMDIR�ļ�"), ".", QStringLiteral("ȫ������(*.*)"));
	//if (path.isEmpty() == true)
	//	return;
	///*
	// * ��У��path�ǲ���DICOMDIR�ļ���·��,�������,�������沢�˳�
	// * �����,ִ��������������
	// */
	//DicomDir *m_dicomdir = new DicomDir();
	//m_dicomdir->InitDirExplorerFromDirPath(path);
	//connect(m_dicomdir, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
	//m_dicomdir->show();
	//setAppState(Dir);//�������dir״̬
}

//��Ӳ���
void QvtkDicomViewer::addDistanceWidget()
{
	distanceWidget = vtkSmartPointer<vtkDistanceWidget>::New();
	distanceWidget->SetInteractor(renderWindowInteractor);
	distanceWidget->CreateDefaultRepresentation();
	//reader->SetDataSpacing(50, 50, 1.5);
	//static_cast<vtkDistanceRepresentation *>(distanceWidget->GetRepresentation())->SetScale(1.42f);//?
	//VTK��������ϵ�ĵ�λ������ĵ�λ=SetScale()
	static_cast<vtkDistanceRepresentation *>(distanceWidget->GetRepresentation())->SetLabelFormat("%-#6.3g mm");
}

//���������
void QvtkDicomViewer::addAngleWidget()
{
	angleWidget = vtkSmartPointer<vtkAngleWidget>::New();
	angleWidget->SetInteractor(renderWindowInteractor);
	angleWidget->CreateDefaultRepresentation();
}

//�����������
void QvtkDicomViewer::addContourWidget()
{
	contourWidget =vtkSmartPointer<vtkContourWidget>::New();

	///�����ߵ���ɫ
	//contourRepresentation = vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
	//contourRepresentation->GetLinesProperty()->SetColor(1, 0, 0); // Set color to red
	//contourWidget->SetRepresentation(contourRepresentation);
	// Generate a set of points arranged in a circle
	int numPts = 10;
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	for (int i = 0; i < numPts; i++)
	{
		// Create numPts points evenly spread around a circumference of radius 0.1
		const double angle = 2.0*vtkMath::Pi()*i / numPts;
		points->InsertPoint(static_cast<vtkIdType>(i), 10*cos(angle), 10*sin(angle), 0.0);
	}

	// Create a cell array to connect the points into meaningful geometry
	vtkIdType* vertexIndices = new vtkIdType[numPts + 1];
	for (int i = 0; i < numPts; i++) { vertexIndices[i] = static_cast<vtkIdType>(i); }
	// Set the last vertex to 0; this means the last line segment will join the 19th point (vertices[19])
	// with the first one (vertices[0]), thus closing the circle.
	vertexIndices[numPts] = 0;
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(numPts + 1, vertexIndices);

	// Create polydata to hold the geometry just created, and populate it
	vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
	polydata->SetPoints(points);
	polydata->SetLines(lines);

	contourWidget->SetInteractor(renderWindowInteractor);
}

//�����Ƭҳ��
void QvtkDicomViewer::SetSliceText(int current,int max)
{
	m_pImageViewer->GetRenderer()->RemoveActor(sliceTextActor);
	vtkSmartPointer<vtkTextProperty> sliceTextProp = vtkSmartPointer<vtkTextProperty>::New();
	sliceTextProp->SetFontFamilyToCourier();
	sliceTextProp->SetFontSize(20);
	sliceTextProp->SetVerticalJustificationToBottom();
	sliceTextProp->SetJustificationToLeft();

	vtkSmartPointer<vtkTextMapper> sliceTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	std::string msg = StatusMessage::Format(current-1, max-1);//��ʾ6/21
	sliceTextMapper->SetInput(msg.c_str());
	sliceTextMapper->SetTextProperty(sliceTextProp);

	/*vtkSmartPointer<vtkActor2D>*/ sliceTextActor = vtkSmartPointer<vtkActor2D>::New();
	sliceTextActor->SetMapper(sliceTextMapper);
	sliceTextActor->SetPosition(15, 10);
	m_pImageViewer->GetRenderer()->AddActor2D(sliceTextActor);
	
}

//���Dicom�ļ�ͷ��Ϣ
void QvtkDicomViewer::SetUsageText(std::string imagefilename)
{
	DcmFileFormat fileformat;
	OFCondition status = fileformat.loadFile(imagefilename.c_str());
	if(status.bad())
	{
		//�쳣
		return;	
	}
	m_pImageViewer->GetRenderer()->RemoveActor(usageTextActor1);//���
	m_pImageViewer->GetRenderer()->RemoveActor(usageTextActor2);//���
	m_pImageViewer->GetRenderer()->RemoveActor(usageTextActor3);//���
#if _DEBUG
	//std::filebuf lb;
	//lb.open("metainfo.log", std::ios::out);
	//std::ostream out2(&lb);
	//fileformat.print(out2);
	//lb.close();
#endif

	OFString temp_OFString;
	std::string TopLeftCorner = "";
	std::string TopRightCorner = "";
	std::string LowerRightCorner = "";

#pragma region ���Ͻ���Ϣ
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_PatientName, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Patient Name:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_PatientID, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Patient ID:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_PatientSex, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Patient Sex:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_InstitutionName, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("InstitutionName:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_StudyID, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Study ID:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_StudyDescription, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Study Description:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_SeriesDescription, temp_OFString, OFTrue).good())
	{
		//TopRightCorner.append("Series Description:");
		TopRightCorner.append(temp_OFString.c_str());
		TopRightCorner.append("\n");
	}
#pragma endregion

#pragma region ���Ͻ���Ϣ
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_SeriesNumber, temp_OFString, OFTrue).good())
	{
		TopLeftCorner.append("Series Number:");
		TopLeftCorner.append(temp_OFString.c_str());
		TopLeftCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_WindowCenter, temp_OFString, OFTrue).good())
	{//��λ
		TopLeftCorner.append("Window Center:");
		TopLeftCorner.append(temp_OFString.c_str());
		TopLeftCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_WindowWidth, temp_OFString, OFTrue).good())
	{//����
		TopLeftCorner.append("Window Width:");
		TopLeftCorner.append(temp_OFString.c_str());
		TopLeftCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_SpacingBetweenSlices, temp_OFString, OFTrue).good())
	{//���,mm
		TopLeftCorner.append("Spacing Between Slices:");
		TopLeftCorner.append(temp_OFString.c_str());
		TopLeftCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_SliceLocation, temp_OFString, OFTrue).good())
	{//����mm
		TopLeftCorner.append("Slice Location:");
		TopLeftCorner.append(temp_OFString.c_str());
		TopLeftCorner.append("\n");
	}
#pragma endregion 

#pragma region ���½���Ϣ
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_MagneticFieldStrength, temp_OFString, OFTrue).good())
	{//�ų�ǿ��
		LowerRightCorner.append("Magnetic Field Strength:");
		LowerRightCorner.append(temp_OFString.c_str());
		LowerRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_RepetitionTime, temp_OFString, OFTrue).good())
	{//�ظ�ʱ��
		LowerRightCorner.append("Repetition Time:");
		LowerRightCorner.append(temp_OFString.c_str());
		LowerRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_EchoTime, temp_OFString, OFTrue).good())
	{//�ز�ʱ��
		LowerRightCorner.append("Echo Time:");
		LowerRightCorner.append(temp_OFString.c_str());
		LowerRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_ContentDate, temp_OFString, OFTrue).good())
	{//����
		LowerRightCorner.append("Content Date:");
		LowerRightCorner.append(temp_OFString.c_str());
		LowerRightCorner.append("\n");
	}
	if (fileformat.getDataset()->findAndGetOFStringArray(DCM_ContentTime, temp_OFString, OFTrue).good())
	{//ʱ��
		LowerRightCorner.append("Content Time:");
		LowerRightCorner.append(temp_OFString.c_str());
		LowerRightCorner.append("\n");
	}
#pragma endregion 

	const char* message_TopLeftCorner = TopLeftCorner.c_str();
	const char* message_TopRightCorner = TopRightCorner.c_str();
	const char* message_LowerRightCorner = LowerRightCorner.c_str();
	//������ʽ:���Ͻ�
	vtkSmartPointer<vtkTextProperty> usageTextProp = vtkSmartPointer<vtkTextProperty>::New();
	usageTextProp->SetFontFamilyToCourier();   
	usageTextProp->SetFontSize(20);				
	usageTextProp->SetColor(1.0, 1.0, 0.0);			
	usageTextProp->SetVerticalJustificationToTop();
	usageTextProp->SetJustificationToLeft();		
	vtkSmartPointer<vtkTextMapper> usageTextMapper = vtkSmartPointer<vtkTextMapper>::New();
	usageTextMapper->SetInput(message_TopLeftCorner);
	usageTextMapper->SetTextProperty(usageTextProp);
	usageTextActor1 = vtkSmartPointer<vtkActor2D>::New();
	usageTextActor1->SetMapper(usageTextMapper);
	usageTextActor1->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	usageTextActor1->GetPositionCoordinate()->SetValue(0.05, 0.95);//����
	m_pImageViewer->GetRenderer()->AddActor2D(usageTextActor1);
	//���Ͻ�
	vtkSmartPointer<vtkTextProperty> usageTextProp2 = vtkSmartPointer<vtkTextProperty>::New();
	usageTextProp2->SetFontFamilyToCourier();
	usageTextProp2->SetFontSize(20);				
	usageTextProp2->SetColor(0.0, 1.0, 0.0);			
	usageTextProp2->SetVerticalJustificationToTop();
	usageTextProp2->SetJustificationToRight();		
	vtkSmartPointer<vtkTextMapper> usageTextMapper2 = vtkSmartPointer<vtkTextMapper>::New();
	usageTextMapper2->SetInput(message_TopRightCorner);
	usageTextMapper2->SetTextProperty(usageTextProp2);
	usageTextActor2 = vtkSmartPointer<vtkActor2D>::New();
	usageTextActor2->SetMapper(usageTextMapper2);
	usageTextActor2->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	usageTextActor2->GetPositionCoordinate()->SetValue(0.95, 0.95);//����
	m_pImageViewer->GetRenderer()->AddActor2D(usageTextActor2);
	//���½�
	vtkSmartPointer<vtkTextProperty> usageTextProp3 = vtkSmartPointer<vtkTextProperty>::New();
	usageTextProp3->SetFontFamilyToCourier();
	usageTextProp3->SetFontSize(20);				
	usageTextProp3->SetColor(1.0, 1.0, 0.0);			
	usageTextProp3->SetVerticalJustificationToBottom();
	usageTextProp3->SetJustificationToRight();		
	vtkSmartPointer<vtkTextMapper> usageTextMapper3 = vtkSmartPointer<vtkTextMapper>::New();
	usageTextMapper3->SetInput(message_LowerRightCorner);
	usageTextMapper3->SetTextProperty(usageTextProp3);
	usageTextActor3 = vtkSmartPointer<vtkActor2D>::New();
	usageTextActor3->SetMapper(usageTextMapper3);
	usageTextActor3->GetPositionCoordinate()->SetCoordinateSystemToNormalizedDisplay();
	usageTextActor3->GetPositionCoordinate()->SetValue(0.95, 0.05);//����
	m_pImageViewer->GetRenderer()->AddActor2D(usageTextActor3);
}

//���������ָʾ
void QvtkDicomViewer::addOrientationMarker()
{
	axes =vtkSmartPointer<vtkAxesActor>::New();
	widget =vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
	widget->SetOrientationMarker(axes);
	widget->SetInteractor(renderWindowInteractor);
	widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	widget->SetEnabled(1);
	widget->InteractiveOff();//���ý���
	//widget->InteractiveOn();
}

//��Ӷ�ά�߶ȱ��
void QvtkDicomViewer::addBiDimensionalWidget()
{
	biDimensionalWidget =vtkSmartPointer<vtkBiDimensionalWidget>::New();
	biDimensionalWidget->SetInteractor(renderWindowInteractor);
	biDimensionalWidget->CreateDefaultRepresentation();
	biDimensionalCallback =vtkSmartPointer<vtkBiDimensionalCallback>::New();
	biDimensionalWidget->AddObserver(vtkCommand::InteractionEvent, biDimensionalCallback);
}

//��������ͼ�����Ĳ˵�
void QvtkDicomViewer::CreateContextMenu()
{

	QAction* action_New_Open_DICOMDIR_File = new QAction(QStringLiteral("&��DICOMDIR�ļ�"), ui.treeView);
	connect(action_New_Open_DICOMDIR_File, SIGNAL(triggered()), this, SLOT(OnOpenDicomDirFile()));

	QAction* action_New_Query_Patient_Msg = new QAction(QStringLiteral("&��ʾ������˵�ȫ����Ϣ"), ui.treeView);
	connect(action_New_Query_Patient_Msg, SIGNAL(triggered()), this, SLOT(OnShowDicomCurrentTags()));

	QAction* action_New_Render_Series = new QAction(QStringLiteral("&��ʾ���Series"), ui.treeView);
	connect(action_New_Render_Series, SIGNAL(triggered()), this, SLOT(OnShowSelectedSeries()));

	QAction* action_3D_Reconstruction = new QAction(QStringLiteral("&3D�ؽ�"), ui.treeView);
	connect(action_3D_Reconstruction, SIGNAL(triggered()), this, SLOT(On3D_Reconstruction()));

	QAction* action_New_Render_Image = new QAction(QStringLiteral("&��ʾ���Image"), ui.treeView);
	connect(action_New_Render_Image, SIGNAL(triggered()), this, SLOT(OnShowSelectedImage()));

	QAction* action_Segment_Image = new QAction(QStringLiteral("&�Ը�Image���зָ�"), ui.treeView);
	connect(action_Segment_Image, SIGNAL(triggered()), this, SLOT(OnSegmentImage()));

	//���Ҽ��˵�->����
	TreeViewMenu_OnEmpty = new QMenu(ui.treeView);
	TreeViewMenu_OnEmpty->addAction(action_New_Open_DICOMDIR_File);
	//���Ҽ��˵�->Patient�ڵ�
	TreeViewMenu_OnPatient = new QMenu(ui.treeView);
	TreeViewMenu_OnPatient->addAction(action_New_Query_Patient_Msg);
	//���Ҽ��˵�->Series�ڵ�
	TreeViewMenu_OnSeries = new QMenu(ui.treeView);
	TreeViewMenu_OnSeries->addAction(action_New_Render_Series);
	TreeViewMenu_OnSeries->addAction(action_3D_Reconstruction);
	//���Ҽ��˵�->Image�ڵ�
	TreeViewMenu_OnImage = new QMenu(ui.treeView);
	TreeViewMenu_OnImage->addAction(action_New_Render_Image);
	TreeViewMenu_OnImage->addAction(action_Segment_Image);
}

//��ʾ��ǰseries�еĵ�Index��ͼ,Index��0��ʼ,����������
void QvtkDicomViewer::ShowImageByIndex(int Index)
{
	RenderRefresh(CurrentPatient->getDicomImageByIndex(Index)->AbsFilePath, 
				  Index + 1, 
				  CurrentPatient->getCurrentDicomSeries()->ImageList.size());
}

//�޸ĵ�ǰ�������
void QvtkDicomViewer::setCursor(CURSOR newValue)
{
	CursorType = newValue;
	emit CursorValueChanged();//ֵ����,�����ź�
}

//�޸Ĵ���λģʽ
void QvtkDicomViewer::setWindowWL(WINDOWWL newWL)
{
	ImageWindow = newWL;
	//ѡ���
	ui.action_WindowWL_All->setChecked(false);
	ui.action_WindowWL_CT_Abdomen->setChecked(false);
	ui.action_WindowWL_CT_BloodVessel->setChecked(false);
	ui.action_WindowWL_CT_Bones->setChecked(false);
	ui.action_WindowWL_CT_Brain->setChecked(false);
	ui.action_WindowWL_CT_Lungs->setChecked(false);
	ui.action_WindowWL_CT_Medias->setChecked(false);
	ui.action_WindowWL_Default->setChecked(false);
	switch (ImageWindow)
	{
	case Default:
		ui.action_WindowWL_Default->setChecked(true);
		break;
	case All:
		ui.action_WindowWL_All->setChecked(true);
		break;
	case Abdomen:
		ui.action_WindowWL_CT_Abdomen->setChecked(true);
		break;
	case BloodVessel:	
		ui.action_WindowWL_CT_BloodVessel->setChecked(true);
		break;
	case Bones:
		ui.action_WindowWL_CT_Bones->setChecked(true);
		break;
	case Brain:
		ui.action_WindowWL_CT_Brain->setChecked(true);
		break;
	case Medias:
		ui.action_WindowWL_CT_Medias->setChecked(true);
		break;
	case Lungs:
		ui.action_WindowWL_CT_Lungs->setChecked(true);
		break;
	default:
		ui.action_WindowWL_Default->setChecked(true);
		break;
	}
	emit WindowWLChanged();//ֵ����,�����ź�
}

// �ı�����״̬
void QvtkDicomViewer::setAppState(STATE new_state)
{
	AppState = new_state;
	emit AppStateChanged();//ֵ����,�����ź�
}

// ��Ⱦ����ʼ��
/*
 * ����:һ��ͼƬ���ļ�·��
 *	    ��Ҫ��ʾ��ͼƬ������,Ĭ��Ϊ1
 * ˵��:������Ⱦ��,Ҫô����Ⱦ����ͼ,Ҫô����Ⱦһ��ͼ������(����ͼ)
 */
void QvtkDicomViewer::RenderInitializer(std::string folder,int NumOfImage )
{
	/*
	 * reader��ʼ��
	 */
	reader = vtkSmartPointer<vtkDICOMImageReader>::New();
	reader->SetFileName(folder.c_str());
	/*
	 * reader��������
	 */
	vtkSmartPointer<vtkImageChangeInformation> changer =vtkSmartPointer<vtkImageChangeInformation>::New();
	changer->SetInputData(reader->GetOutput());
	changer->SetOutputOrigin(0, 0, 0);
	changer->SetOutputSpacing(10, 10, 1);
	changer->SetCenterImage(1);
	changer->Update();
	reader->Update();
	/*
	 * renderer��ʼ��
	 */
	m_pRenderder = vtkSmartPointer< vtkRenderer >::New();
	ui.qvtkWidget->GetRenderWindow()->AddRenderer(m_pRenderder);
	/*
	 * Viewer��ʼ��
	 */
	m_pImageViewer = vtkSmartPointer< vtkImageViewer2 >::New();
	m_pImageViewer->SetInputConnection(reader->GetOutputPort());
	/*
	 * ��������
	 */
	SetSliceText(1, NumOfImage);// ��ʼ����Ƭҳ����Ϣ
	SetUsageText(folder.c_str());// ��ʾһЩDicom�ļ�ͷ��Ϣ
	/*
	 * �������ͽ�������ʼ��
	 */
	renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	myInteractorStyle = vtkSmartPointer<myVtkInteractorStyleImage>::New();
	myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::POINTER;
	m_pImageViewer->SetupInteractor(renderWindowInteractor);
	m_pImageViewer->SetRenderWindow(ui.qvtkWidget->GetRenderWindow());
	renderWindowInteractor->SetInteractorStyle(myInteractorStyle);
	/*
	 * ����һЩVTK�ؼ�
	 */
	addDistanceWidget();// ʵ���������ò�����
	addAngleWidget();	// ʵ�������������������
	addContourWidget();	// ʵ�����������������
	addOrientationMarker();//���������ָʾ(û�п���)
	addBiDimensionalWidget();
	/*
	 * �Զ��������Ź��ܳ�ʼ��
	 */
	min = 0; max = NumOfImage - 1; current = ui.SliceScrollBar->sliderPosition();
	/*
	 * ��Ⱦ
	 */
	m_pImageViewer->GetRenderer()->ResetCamera();
	ui.qvtkWidget->GetRenderWindow()->Render();
	/*
	 * ��Ͻ�����
	 */
	ui.SliceScrollBar->setRange(0, NumOfImage - 1);
}

//������Ⱦ/��Ⱦ��һ֡
/*
 * �����������֮ǰ�����ȵ�����Ⱦ����ʼ������,���򽫻ᷢ������ʱ����
 */
void QvtkDicomViewer::RenderRefresh(std::string imagefilename,int currentPagenumber,int maxPageNumber)
{
	//�л��ļ�
	reader->SetFileName(imagefilename.c_str());
	//�л�ҳ����Ϣ
	SetSliceText(currentPagenumber, maxPageNumber);
	//�л�������Ϣ

	//������Ⱦ
	reader->Update();
	ui.qvtkWidget->GetRenderWindow()->Render();
}

void QvtkDicomViewer::OnProjectLoadFinish(QString filePath)
{
	QFileInfo  info(filePath);

	QString fileName = info.fileName();
	this->m_treeProjectVector.append(fileName);
	this->UpdateTreeForStl();
}

void QvtkDicomViewer::OnClearTree()
{
	this->m_treeProjectVector.clear();
	this->UpdateTreeForStl();
}

void QvtkDicomViewer::UpdateTreeForStl()
{
	QString fileName = QStringLiteral("��������");

	QStandardItemModel* model = new QStandardItemModel();
	model->setHorizontalHeaderLabels(QStringList() << QStringLiteral("Project"));

	QStandardItem* root = new QStandardItem(QIcon(":/icon/icon/p.png"), fileName);
	root->setData(MARK_PROJECT, ROLE_MARK);
	root->setData(MARK_FOLDER, ROLE_MARK_FOLDER);

	model->appendRow(root);
	ui.treeView->setModel(model);

	QVector<QString>::iterator ProjectItemTypeIt;
	for (ProjectItemTypeIt = this->m_treeProjectVector.begin(); ProjectItemTypeIt != this->m_treeProjectVector.end(); ++ProjectItemTypeIt)
	{
		QString rootName = *ProjectItemTypeIt;

		QStandardItem* folder = new QStandardItem(QIcon(":/icon/icon/h-f.png"), rootName);
		folder->setData(MARK_FOLDER, ROLE_MARK);
		//folder->setData(folderType, ROLE_MARK_FOLDER);
		root->appendRow(folder);
	}

	ui.treeView->expandAll();
}

//ˢ��DIR��
void QvtkDicomViewer::DirTreeRefresh(DicomPatient * patient)
{
	QStringList headers;
	headers.append(QStringLiteral("ID"));
	headers.append(QStringLiteral("��ϸ��Ϣ"));
	m_dicomdirtreemodel = new DicomDirTreeModel(headers, *patient);
	ui.treeView->setModel(m_dicomdirtreemodel);
	ui.treeView->expandAll();
	for (int column = 0; column < m_dicomdirtreemodel->columnCount(); ++column)
		ui.treeView->resizeColumnToContents(column);
}

//������->ǰһ��
void QvtkDicomViewer::OnForward()
{
	current++;
	ui.SliceScrollBar->setSliderPosition(current);
}

//������->��һ��
void QvtkDicomViewer::OnBackward()
{
	current--;
	ui.SliceScrollBar->setSliderPosition(current);
}

//�ص���һ��
void QvtkDicomViewer::OnResetToFirst()
{
	ui.SliceScrollBar->setSliderPosition(0);
}

//ѡ��Ĭ�����ָ�빤��
void QvtkDicomViewer::OnSelectedPointer()
{
	setCursor(CURSOR::POINTRE);
	myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::POINTER;
}

//ѡ������������
void QvtkDicomViewer::OnSelectedProtractor()
{
	setCursor(CURSOR::PROTRACTOR);
}

//ѡ�в��߹���
void QvtkDicomViewer::OnSelectedRuler()
{
	setCursor(CURSOR::RULER);
}

//ѡ����������
void QvtkDicomViewer::OnSelectedContour()
{
	setCursor(CURSOR::CONTOUR);
}

//ѡ�ж�ά��߹���
void QvtkDicomViewer::OnSelectedBiDimensional()
{
	setCursor(CURSOR::BIDI);
}

//ѡ�лҽ׹���
void QvtkDicomViewer::OnSelectedGrayLevel()
{
	setCursor(CURSOR::GRAYLEVEL);
	myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::GRAYLEVEL;
}

//ѡ�����Ź���
void QvtkDicomViewer::OnSelectedZoom()
{
	setCursor(CURSOR::ZOOM);
	myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::ZOOM;
}

 //ѡ���ƶ�����
void QvtkDicomViewer::OnSelectedMove()
{
	setCursor(CURSOR::MOVE);
	myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::MOVE;
}

//ʹ�ø�ƬЧ��
void QvtkDicomViewer::OnNegative()
{
	//m_pImageViewer->SetColorLevel(300.0);
	//m_pImageViewer->SetColorWindow(1500.0);
	//m_pImageViewer->Render();
	//m_pImageViewer-
	//double range[2];
	//reader->GetOutput()->GetScalarRange(range);
	//vtkImageShiftScale* shifter = vtkImageShiftScale::New();
	//shifter->SetShift(-1.0*range[0]);//��ȥ��Сֵ
	//shifter->SetScale(255.0*-1 / (range[1] - range[0])); //���ڱ��� //�ҶȻ��㹫ʽ��255*X/(range[1]-range[0])
	//shifter->SetOutputScalarTypeToUnsignedChar();
	//shifter->SetInputConnection(reader->GetOutputPort());
	//���㹫ʽ�� double val = ((double)(*inSI) + shift) * scale;
	//��� = ������ + shift��*scale; shiftΪƫ����, scale�Ҷȱ�
	//////////////////////////////////////////////////////////////////////////////
	//int subRegion[6] = { 0, 511, 0, 511, 0, 61 };//64, 448, 64, 448, 0, 0
	//ui.qvtkWidget->
	//vtkImageIterator<unsigned char> iter(reader->GetOutput(),reader->GetOutput()->GetExtent());
	//											
	//while (!iter.IsAtEnd())//ע������������Դ��Ļ���Ч��					 
	//{								 	
	//	unsigned char *inSI = iter.BeginSpan();									 	
	//	unsigned char *inSIEnd = iter.EndSpan();					 	
	//	while (inSI != inSIEnd)								 	
	//	{								 		
	//		*inSI = 255 - *inSI;								 		
	//		++inSI;									 	
	//	}									 	
	//	iter.NextSpan();								
	//}
	////////////////////////////////////////////////////////////////////////////////
	/*vtkSmartPointer<vtkImageLogic>imageMath =vtkSmartPointer<vtkImageLogic>::New();
	imageMath->SetOperationToNot();
	imageMath->SetInputConnection(reader->GetOutputPort());
	imageMath->Update();*/
	/////////////////////////////////////////////////////////////////////////////////
	reader->Update();//ˢ��
}

//��λ��ť
void QvtkDicomViewer::OnReset()
{
	setCursor(CURSOR::POINTRE);
	//myInteractorStyle->MouseFunction = myVtkInteractorStyleImage::POINTER;
	RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
	ui.SliceScrollBar->setValue(CurrentPatient->indexOfCurrentImage);
}

//����
void QvtkDicomViewer::OnPlay()
{
	if (PlayFlag==true)//��ʱ���ڲ���,����֮��Ӧ�ý�����ͣ,����ͼ���л�Ϊ����
	{
		ui.action_Play->setIcon(icon_Play);
		ui.action_Play->setText(QStringLiteral("����"));		
		m_pTimer->stop();//�رռ�ʱ��
		ui.SliceScrollBar->setSliderPosition(current);//ͬ��������
		ui.SliceScrollBar->setEnabled(true);//����������
		ui.action_Stop->setEnabled(false);
		

	}else//��ʱΪֹͣ״̬,����֮��Ӧ�ý��в���,����ͼ���л�Ϊ��ͣ
	{
		ui.action_Play->setIcon(icon_Pause);
		ui.action_Play->setText(QStringLiteral("��ͣ"));
		m_pTimer->start();//������ʱ��
		ui.SliceScrollBar->setEnabled(false);//����������
		ui.action_Stop->setEnabled(true);
	}
	PlayFlag = !PlayFlag;
}

//ֹͣ
void QvtkDicomViewer::OnStop()
{
	ui.action_Play->setIcon(icon_Play);
	ui.action_Play->setText(QStringLiteral("����"));
	PlayFlag = false;
	m_pTimer->stop();//�رռ�ʱ��
	ui.SliceScrollBar->setSliderPosition(current);//ͬ��������
	ui.SliceScrollBar->setEnabled(true);//����������
	ui.action_Stop->setEnabled(false);
}

//����docking���ڵĿ���
 void QvtkDicomViewer::OnSwitchProperty()
 {
	 //ui.action_SwitchOfProperty->isChecked();
	 if (ui.action_SwitchOfProperty->isChecked()==true)
	 {
		 ui.dockWidget_Dir->setHidden(false);
	 }
	 else
	 {
		 ui.dockWidget_Dir->setHidden(true);
	 }
 }

//����ͼ�����Ĳ˵��ַ�
 void QvtkDicomViewer::on_treeView_customContextMenuRequested(QPoint pos)
 {
	 if (PrePosition != pos) {//��δ�������������
		 PrePosition = pos;
		 if (ui.treeView->model() == NULL) {
			 //��ʱ���ǿյ�
			 TreeViewMenu_OnEmpty->exec(QCursor::pos());//��ʾ�Ҽ��˵�
		 }
		 else//���ǿյ�ʱ������������
		 {
			 indexSelect = ui.treeView->indexAt(pos);  //��ǰ�ڵ�����
			 switch (m_dicomdirtreemodel->getLevel(indexSelect))
			 {
			 case 1://level==1:Patient
				 TreeViewMenu_OnPatient->exec(QCursor::pos());
				 break;
			 case 2://level==2:Study
				 //Ŀǰ��û�ж�Ӧ�Ĳ˵�
				 break;
			 case 3://level==3:Series
				 TreeViewMenu_OnSeries->exec(QCursor::pos());
				
				 break;
			 case 4://level==4:image
				 TreeViewMenu_OnImage->exec(QCursor::pos());

				 break;
			 default://�������������,����
				 //QMessageBox::information(this,QStringLiteral("����!"), QStringLiteral("����!"));
				 break;
			 }
		 }
	 }
	 else
	 {
		 //��δ����Ƿ���������
		 PrePosition.setX(-1);
		 PrePosition.setY(-1);
	 }
 }

//����ͼ�Ҽ��˵�->��ʾ��ǰ���˵�������Ϣ
 void QvtkDicomViewer::OnShowDicomCurrentTags()
 {
	 //QMessageBox::information(this, QStringLiteral("�ٺ�!"), QStringLiteral("����˺���,ʲô��û����!"));
 }

//��ʾѡ�е�Series
 void QvtkDicomViewer::OnShowSelectedSeries()
 {
	 CurrentPatient->setCurrentDicomSeriesById(m_dicomdirtreemodel->getItem(indexSelect)->itemData[0].toString().toStdString());
	 RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
 }

//��ʾ��ǰѡ�е�Image
 void QvtkDicomViewer::OnShowSelectedImage()
 {
	 /*
	  *	������ͼ���ڵ�series������������ͼ
	  */
	 CurrentPatient->getDicomImageByRfid(m_dicomdirtreemodel->getItem(indexSelect)->itemData[1].toString().toStdString());
	 RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
	 ui.SliceScrollBar->setValue(CurrentPatient->indexOfCurrentImage);
 }

//Slice������ֵ�����¼�
 void QvtkDicomViewer::OnSliceScrollBarValueChange(int a)
 {
	 current = a;
	 ShowImageByIndex(a);
 }

//��Ӧ��ά�ؽ�����
void QvtkDicomViewer::On3D_Reconstruction()
{
	/*
	 * �ȶԵ�ǰ��Series���ļ���������У��,����ĳ����ֵ��ִֹ��.
	 */
	CurrentPatient->setCurrentDicomSeriesById(m_dicomdirtreemodel->getItem(indexSelect)->itemData[0].toString().toStdString());
	//����
	std::vector<std::string> * filenames_v = new std::vector<std::string>;
	for (int i=0;i<CurrentPatient->getCurrentDicomSeries()->ImageList.size();i++)
	{
		filenames_v->push_back(CurrentPatient->getCurrentDicomSeries()->ImageList.at(i)->AbsFilePath);
	}
	if(filenames_v->size()<=25)
	{
		QMessageBox::information(this, QStringLiteral("����!"), QStringLiteral("��ǰ�����е�ͼƬ����û�дﵽ������ά�ؽ������ͼƬ��������Сֵ(25��)!"));
		return;
	}
	ThreeD_Reconstruction * _3d_reconstructer = new ThreeD_Reconstruction(filenames_v);
	_3d_reconstructer->show();
	_3d_reconstructer->OnReconstruction();
}

//��Ӧͼ��ָ����
void QvtkDicomViewer::OnSegmentImage()
{
	/*
	 * ����ʾ����image
	 */
	CurrentPatient->getDicomImageByRfid(m_dicomdirtreemodel->getItem(indexSelect)->itemData[1].toString().toStdString());
	RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
	ui.SliceScrollBar->setValue(CurrentPatient->indexOfCurrentImage);
	/*
	 * �����ָ�߲����ݹؼ�����
	 */
	Segmenter *_segmenter = new Segmenter(CurrentPatient->getCurrentDicomImage()->AbsFilePath);
	_segmenter->show();
}

//��ӦDicomDir�ഫ�͹������ź�,���а�����һ�����˵�ID
 void QvtkDicomViewer::receiveData(QString data)
 {
	DicomDataBase * temp_database = DicomDataBase::getInstance();
	Current_patientId = data.toStdString();//��ǰ�Ĳ���ID
	//��ǰ���˶����,ע����Ӧ����ȫ��Ψһ�İ󶨵�
	CurrentPatient = new DicomPatient(temp_database->getPatientById(Current_patientId));
	DirTreeRefresh(CurrentPatient);//ˢ������ͼ
	std::string str1 = CurrentPatient->getCurrentDicomImage()->AbsFilePath;
	int i1 = CurrentPatient->getCurrentDicomSeries()->ImageList.size();
	RenderInitializer(str1,i1);//failed
 }
 
 //��һ������
 void QvtkDicomViewer::OnLatterPatient()
 {
	 if (CurrentPatient == NULL)
		 return;
	 //1.�����ݿ���ӿ��ϻ���һ������
	 DicomPatient* temp=DicomDataBase::getInstance()->get_latter_patient(CurrentPatient);
	 //2.���°󶨵�ǰ����
	 CurrentPatient = new DicomPatient(temp);;
	 DirTreeRefresh(CurrentPatient);//ˢ������ͼ
	 RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
 }
 
 //��һ������
 void QvtkDicomViewer::OnPreviousPatient()
 {
	 if (CurrentPatient == NULL)
		 return;
	 //1.�����ݿ���ӿ��ϻ���һ������
	 DicomPatient* temp = DicomDataBase::getInstance()->get_previous_patient(CurrentPatient);
	 //2.���°󶨵�ǰ����
	 CurrentPatient = new DicomPatient(temp);;
	 DirTreeRefresh(CurrentPatient);//ˢ������ͼ
	 RenderInitializer(CurrentPatient->getCurrentDicomImage()->AbsFilePath, CurrentPatient->getCurrentDicomSeries()->ImageList.size());
 }


 //Ĭ�ϴ���λ
 void QvtkDicomViewer::OnWindowWL_Defaut()
 {
	 m_pImageViewer->SetColorLevel(40.0);
	 m_pImageViewer->SetColorWindow(400.0);
	 m_pImageViewer->Render();
	 setWindowWL(Default);
 }

//ȫ����̬��Χ
 void QvtkDicomViewer::OnWindowWL_All()
 {
	 m_pImageViewer->SetColorLevel(1024.0);
	 m_pImageViewer->SetColorWindow(4096.0);
	 m_pImageViewer->Render();
	 setWindowWL(All);
 }

//����
 void QvtkDicomViewer::OnWindowWL_CT_Abdomen()
 {
	 m_pImageViewer->SetColorLevel(60.0);
	 m_pImageViewer->SetColorWindow(400.0);
	 m_pImageViewer->Render();
	 setWindowWL(Abdomen);
 }
 
 //Ѫ��
 void QvtkDicomViewer::OnWindowWL_CT_BloodVessel()
 {
	 m_pImageViewer->SetColorLevel(300.0);
	 m_pImageViewer->SetColorWindow(600.0);
	 m_pImageViewer->Render();
	 setWindowWL(BloodVessel);
 }
 
 //����
 void QvtkDicomViewer::OnWindowWL_CT_Bones()
 {
	 m_pImageViewer->SetColorLevel(300.0);
	 m_pImageViewer->SetColorWindow(1500.0);
	 m_pImageViewer->Render();
	 setWindowWL(Bones);
 }
 
//��
 void QvtkDicomViewer::OnWindowWL_CT_Brain()
 {
	 m_pImageViewer->SetColorLevel(40.0);
	 m_pImageViewer->SetColorWindow(80.0);
	 m_pImageViewer->Render();
	 setWindowWL(Brain);
 }
 
 //����
 void QvtkDicomViewer::OnWindowWL_CT_Medias()
 {
	 m_pImageViewer->SetColorLevel(40.0);
	 m_pImageViewer->SetColorWindow(400.0);
	 m_pImageViewer->Render();
	 setWindowWL(Medias);
 }
 
 //��
 void QvtkDicomViewer::OnWindowWL_CT_Lungs()
 {
	 m_pImageViewer->SetColorLevel(-400.0);
	 m_pImageViewer->SetColorWindow(1500.0);
	 m_pImageViewer->Render();
	 setWindowWL(Lungs);
 }
 
 //������׼����
 void QvtkDicomViewer::OnRegistration()
 {
	 //m_Reg_Window = new Register();//���ȳ�ʼ����׼����
	 //m_Reg_Window->show();
	//Ϊ��������Ӧ�ٶ�,��ʼ������Ӧ��ͳһ����,����һ������,�Ժ���𽥸ĳ�����
 }

//�������1
 void QvtkDicomViewer::OnTestEntrance_01()
 {
	/*
	 * ���ڲ��Ե��Ǵ�series�ļ���
	 */

	 //QString dir = "F:\\Dicom\\Test1\\DICOM\\S428090\\S50";

	 //DicomDir *m_dicomdir = new DicomDir();
	 //connect(m_dicomdir, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
	 //m_dicomdir->InitDirExplorerFromSeriesPath(dir);
	
	 ////m_dicomdir->show();
	 //setAppState(Folder);//�������Folder״̬
 }

//�������2
 void QvtkDicomViewer::OnTestEntrance_02()
 {
	//���ڲ��Ե��Ǽ�ʱ��
	/* m_pTimer->start();*/
 }