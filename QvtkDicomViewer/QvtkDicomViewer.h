#pragma once
//�ڲ�
#include "ui_QvtkDicomViewer.h"
//Qt
#include <QtWidgets/QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QTimer>
//vtk
#include <vtkRenderWindow.h>
#include <vtkDICOMImageReader.h>
#include <vtkImageActor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkImageViewer2.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkAutoInit.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDICOMImageReader.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <vtkTextProperty.h>
#include <vtkTextMapper.h>
#include <vtkActor2D.h>
#include <myVtkInteractorStyleImage.h>
#include <vtkDistanceWidget.h>
#include <vtkAngleWidget.h>
#include <vtkContourWidget.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkBiDimensionalWidget.h>
#include "vtkBiDimensionalCallback.h"
#include "DicomPatient.h"
#include "DicomDirTreeModel.h"
#include <vtkImageCast.h>

#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkSmartPointer.h>  
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>  
#include <vtkGPUVolumeRayCastMapper.h>  
#include <vtkVolumeProperty.h>  
#include <vtkPiecewiseFunction.h>  
#include <vtkColorTransferFunction.h>  
#include <vtkVolume.h>  
#include <vtkRenderer.h>  
#include <vtkRenderWindow.h>  
#include <vtkRenderWindowInteractor.h>  
#include <vtkCamera.h>
#include "vtkUnsignedCharArray.h"    
#include "vtkPiecewiseFunction.h"    
#include "vtkColorTransferFunction.h"    
#include "vtkVolumeRayCastCompositeFunction.h"    
#include "vtkVolumeRayCastMapper.h"    
#include "vtkImageData.h"       
#include "vtkRenderWindow.h"    
#include "vtkWin32OpenGLRenderWindow.h"  
#include "vtkInteractorStyleTrackballActor.h"  
#include "vtkImageChangeInformation.h"  
#include "vtkGPUVolumeRayCastMapper.h"  
#include "vtkGPUInfoList.h"  
#include "vtkGPUInfo.h"  
#include <vtkStructuredPoints.h>  
#include <vtkStructuredPointsReader.h>   
#include <vtkPiecewiseFunction.h>  
#include <vtkRenderer.h>  
#include <vtkRenderWindow.h>  
#include <vtkRenderWindowInteractor.h>  
#include <vtkVolumeProperty.h>  
#include <vtkVolumeRayCastIsosurfaceFunction.h>  
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <QComboBox>
#include "vtkMyDICOMImageReader.h"
#include "Register.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVector3D>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include "face.h"
#include "mywidget.h"

class DicomDataBase;
/*
 * ����˵��:
 * 1.����������:��Qt��ص��������ڽ������->����������
 *				���������������Թ�����->Debug_x64,Release_x64������
 * 2.��Ŀ¼�Ͱ���Ŀ¼:�ڽ������->����������
 * 3.�����汾:Qt5.9.1
 *			  vtk8.0.0
 *			  dcmtk3.6.2(64λmsvc15-2017)
 * 4.QtĿ¼:
 *			�ڽ������Ŀ¼->QvtkDicomViewer->QvtkDicomViewer.vcxproj.user�ļ���
 *			����Qt��Ŀ¼
 */
VTK_MODULE_INIT(vtkRenderingFreeType)
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
#include <vtkAutoInit.h>
#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeType,vtkRenderingOpenGL2) 
//#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)

#define ROLE_MARK Qt::UserRole + 1
#define ROLE_MARK_FOLDER Qt::UserRole + 2
#define ROLE_MARK_ITEM Qt::UserRole + 3
#define ROLE_MARK_LEAF Qt::UserRole + 4

#define MARK_PROJECT 1
#define MARK_FOLDER 2
#define MARK_ITEM 3
#define MARK_LEAF 4

//Qt��������
class QvtkDicomViewer : public QMainWindow
{
	Q_OBJECT

public:
	QvtkDicomViewer(QWidget *parent = Q_NULLPTR);
	//QComboBox* reg_combo;
	Register * m_Reg_Window;
	enum CURSOR		
	{	POINTRE,		//Ĭ��ָ��
		ZOOM,			//����
		GRAYLEVEL,		//�ҽ�
		PROTRACTOR,		//������
		RULER,			//����
		CONTOUR,		//����
		BIDI,			//��ά��
		MOVE			//�ƶ�
	}CursorType;
	enum WINDOWWL
	{
		Default,		//Ĭ�ϴ���λ
		All,			//ȫ����̬��Χ
		Abdomen,		//����
		BloodVessel,	//Ѫ��
		Bones,			//����
		Brain,			//��
		Medias,			//����
		Lungs			//��
	}ImageWindow;
	enum STATE
	{
		Idel,			//����״̬
		SingleImage,	//����ͼƬ״̬
		Folder,		//���ļ���״̬
		Dir,			//��DICOMDIR״̬
		Err,			//�������ش����״̬
		Debug			//����״̬
	}AppState;
private:
	//CURSOR CursorType;//�������
	Ui::QvtkDicomViewerClass ui;
	vtkSmartPointer<vtkImageViewer2 > m_pImageViewer;
	vtkSmartPointer<vtkRenderer > m_pRenderder;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSmartPointer<vtkDICOMImageReader> reader;
	//vtkSmartPointer<vtkTextProperty> sliceTextProp;
	//vtkSmartPointer<vtkTextMapper> sliceTextMapper;
	vtkSmartPointer<vtkActor2D> sliceTextActor;//������ȫ�ֵ�.�����޷�������һ�εõ��ۼ�
	//vtkSmartPointer<vtkTextProperty> usageTextProp;
	//vtkSmartPointer<vtkTextMapper> usageTextMapper;
	vtkSmartPointer<vtkActor2D> usageTextActor1;
	vtkSmartPointer<vtkActor2D> usageTextActor2;
	vtkSmartPointer<vtkActor2D> usageTextActor3;
	vtkSmartPointer<myVtkInteractorStyleImage> myInteractorStyle;
	vtkSmartPointer<vtkDistanceWidget> distanceWidget;//���
	vtkSmartPointer<vtkAngleWidget> angleWidget;//�����Ƕ�
	vtkSmartPointer<vtkContourWidget> contourWidget;//����ѡ��
	vtkSmartPointer<vtkAxesActor> axes;
	vtkSmartPointer<vtkOrientedGlyphContourRepresentation> contourRepresentation;
	vtkSmartPointer<vtkOrientationMarkerWidget> widget;
	vtkSmartPointer<vtkBiDimensionalWidget> biDimensionalWidget;
	vtkSmartPointer<vtkBiDimensionalCallback> biDimensionalCallback;

	QIcon icon_Play;//����ͼ��
	QIcon icon_Pause;//��ͣͼ��
	bool PlayFlag;//false:ͼ��ӦΪ����,����׼������״̬,true:ͼ��ӦΪ��ͣ,���ڲ���״̬��׼����ͣ
	QTimer * m_pTimer;//��ʱ��
	int min;//��һ��ͼ�ǵڼ���
	int max;//���һ��ͼ�ǵڼ���
	int current;//Ŀǰ�ŵ��ڼ���

	QPoint PrePosition;					//ǰһ�ε��Ҽ����λ��
	QMenu * TreeViewMenu_OnEmpty;		//���Ҽ��˵�->��Ϊ��
	QMenu * TreeViewMenu_OnPatient;		//���Ҽ��˵�->���˽ڵ�
	QMenu * TreeViewMenu_OnSeries;		//���Ҽ��˵�->Series�ڵ�
	QMenu * TreeViewMenu_OnImage;		//���Ҽ��˵�->Image�ڵ�

	std::string Current_patientId;//��ǰ�Ĳ���ID
	DicomPatient * CurrentPatient;		//��ǰ����
	QModelIndex indexSelect;//����ͼ��
	DicomDirTreeModel *m_dicomdirtreemodel;

	myWidget                        *m_myWidget;
	QVector<QString>				m_treeProjectVector;

private:
	///�ڲ�����
	void setCursor(CURSOR newValue);		//�ı����ָ�뽻��ģʽ
	void setWindowWL(WINDOWWL newWL);		//�ı䴰��λģʽ
	void setAppState(STATE new_state);		//�ı�����״̬
	void RenderInitializer(std::string folder, int NumOfImage = 1);//��Ⱦ����ʼ��
	void RenderRefresh(std::string imagefilename, int currentPagenumber, int maxPageNumber);//������Ⱦ
	void DirTreeRefresh(DicomPatient * patient);
	void addDistanceWidget();
	void addAngleWidget();
	void addContourWidget();
	void SetSliceText(int current, int max);			//������ʾҳ����Ϣ
	void SetUsageText(std::string imagefilename);
	void addOrientationMarker();
	void addBiDimensionalWidget();
	void CreateContextMenu();//����ͼ�����Ĳ˵�
	void ShowImageByIndex(int Index);//��ʾ��ǰseries�еĵ�Index��ͼ,Index��0��ʼ,����������
signals :
	void CursorValueChanged();      //�Զ���ֵ�����ź�,���ڼ�ص�ǰ���ı仯
	void WindowWLChanged();			//�Զ���ֵ�����ź�,���ڼ�ص�ǰ����λģʽ�ı仯
	void AppStateChanged();			//����״̬�����ź�,���ڶ�̬��������״̬
	void SigOpenStlFile();

public slots:
	void OnPlayerTimerOut();		//��������������Ҫ����
	void OnChangeAppState();		//��Ӧ����״̬�ĸ���,ִ��һЩ����/����/ˢ�²���
	void OnChangeCursorValue();		//��Ӧ���ֵ���޸�,ִ��һЩˢ�ºͽ��ò���
	void OnChangeWindowsWL();		//��Ӧ����λģʽֵ���޸�,ִ��һЩ���ú�ѡ������
	void OnOpenSeriesFolder();		//��seriesĿ¼
	void OnOpenDicomFile();			//�򿪵���Dicom�ļ�
	void OnOpenDicomDirFile();		//��DICOMDIR�ļ�
	void OnForward();				//ǰһ�Ű���
	void OnBackward();				//��һ�Ű���
	void OnResetToFirst();			//�ص���һ��
	void OnSelectedPointer();		//ѡ��Ĭ�����ָ�빤��
	void OnSelectedProtractor();	//ѡ������������
	void OnSelectedRuler();			//ѡ�в��߹���
	void OnSelectedContour();		//ѡ����������
	void OnSelectedBiDimensional();	//ѡ�ж�ά��߹���
	void OnSelectedGrayLevel();		//ѡ�лҽ׹���
	void OnSelectedZoom();			//ѡ�����Ź���
	void OnSelectedMove();			//ѡ���ƶ�����
	void OnNegative();				//ʹ�ø�ƬЧ��
	void OnReset();					//��λ
	void OnPlay();					//����/��ͣ
	void OnStop();					//ֹͣ
	void OnSwitchProperty();		//����docking���ڵĿ���
	void on_treeView_customContextMenuRequested(QPoint pos);//����ͼ�����Ĳ˵��ַ�
	void OnShowDicomCurrentTags();	//��ʾ��ǰ���˵�������Ϣ
	void OnShowSelectedSeries();	//��ʾѡ�е�Series
	void OnShowSelectedImage();		//��ʾ��ǰѡ�е�Image
	void OnSliceScrollBarValueChange(int a);//Slice������ֵ�����¼�
	void receiveData(QString data);//��ӦDicomDir�ഫ�͹������ź�,���а�����һ�����˵�ID
	void OnLatterPatient();//��һ������
	void OnPreviousPatient();//��һ������

	void OnWindowWL_Defaut();			//Ĭ�ϴ���λ
	void OnWindowWL_All();				//ȫ����̬��Χ
	void OnWindowWL_CT_Abdomen();		//����
	void OnWindowWL_CT_BloodVessel();	//Ѫ��
	void OnWindowWL_CT_Bones();			//����
	void OnWindowWL_CT_Brain();			//��
	void OnWindowWL_CT_Medias();		//����
	void OnWindowWL_CT_Lungs();		    //��
	///�������
	void OnTestEntrance_01();			//�������1
	void OnTestEntrance_02();			//�������2

	void On3D_Reconstruction();			//��Ӧ3ά�ؽ�
	void OnSegmentImage();				//��Ӧͼ��ָ�
	void OnRegistration();				//������׼����

	void OnProjectLoadFinish(QString filePath);
	void OnClearTree();
	void UpdateTreeForStl();
};