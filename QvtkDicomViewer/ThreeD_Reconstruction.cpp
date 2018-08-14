#include "ThreeD_Reconstruction.h"
#include <vtkDecimatePro.h>
#include "vtkPiecewiseFunction.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkImageAppend.h>
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"
#include <vtkDICOMImageReader.h>


/*
 * Ĭ�Ϲ���
 */
ThreeD_Reconstruction::ThreeD_Reconstruction(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}
/*
 * �����ݹ��췽��,������������Ⱦ����
 */
ThreeD_Reconstruction::ThreeD_Reconstruction(std::vector<std::string>* filenames_v, QWidget* parent)
{
	//
	ui.setupUi(this);
	m_Filenames = vtkSmartPointer<vtkStringArray>::New();
	for(int i=0;i<filenames_v->size();i++)
	{
		m_Filenames->InsertNextValue(filenames_v->at(i));
	}
	setWindowTitle(QStringLiteral("��ά�ؽ�-���ڽ���..."));
}
/*
 * ����,�ͷ����з�����ָ������
 */
ThreeD_Reconstruction::~ThreeD_Reconstruction()
{

}
/*
 * ������Ⱦ
 */
void ThreeD_Reconstruction::OnReconstruction()
{
	//RenderPipeline_CPU(m_Filenames);
	RenderPipeline_GPU(m_Filenames);
}
/*
 * ����CPU��Ⱦ����
 * ����:vtkStringArray *_FilesYouWant һ��series��ȫ�� ��˳���źõ��ļ��� �����ļ���
 */
void ThreeD_Reconstruction::RenderPipeline_CPU(vtkStringArray *_FilesYouWant)
{
	m_ThreeDRec_Renderer = vtkRenderer::New();             //���û�����(���ƶ���ָ��)
	m_ThreeDRec_RenderWindow = vtkRenderWindow::New();	   //���û��ƴ���
	m_ThreeDRec_RenderWindow->AddRenderer(m_ThreeDRec_Renderer);                       //�������߼�����ƴ���

	ui.qvtkWidget_3DRec->SetRenderWindow(m_ThreeDRec_RenderWindow);

	renderWindowInteractor = vtkRenderWindowInteractor::New();//���û��ƽ����������ڵ�
	renderWindowInteractor->SetRenderWindow(m_ThreeDRec_RenderWindow);    //�����ƴ�����ӵ���������

	vtkDecimatePro *deci = vtkDecimatePro::New(); //�������ݶ�ȡ�㣬���������������ٽ���
	deci->SetTargetReduction(0.3);

	TrackballCamera = vtkInteractorStyleTrackballCamera::New();//���������      
	renderWindowInteractor->SetInteractorStyle(TrackballCamera);

	m_MyDICOMImageReader = vtkSmartPointer<vtkMyDICOMImageReader>::New();
	m_MyDICOMImageReader->SetFileNames(_FilesYouWant);

#if _DEBUG
	std::filebuf fb;
	fb.open("vtkdicomviewer.log", std::ios::out);//�����־�ļ�
	std::ostream out(&fb);
	m_MyDICOMImageReader->Print(out);
	fb.close();
#endif
	m_MyDICOMImageReader->SetFileDimensionality(3);                 //������ʾͼ���ά��											
	m_MyDICOMImageReader->SetDataScalarType(VTK_UNSIGNED_SHORT);
	m_MyDICOMImageReader->SetDataExtent(0, 255, 0, 255, 0, 62);     //ͼƬ����ͼƬ����256x256�������������ʾ��124��ͼ
	m_MyDICOMImageReader->SetDataSpacing(0.9, 0.9, 0.9);            //�������ؼ���
	
	m_ImageCast = vtkImageCast::New();								//��������ת��
	m_ImageCast->SetInputConnection(m_MyDICOMImageReader->GetOutputPort());

	m_ImageCast->SetOutputScalarTypeToUnsignedShort();
	m_ImageCast->ClampOverflowOn();									 //��ֵ

														
	m_PiecewiseFunction = vtkPiecewiseFunction::New();				 //һά�ֶκ����任
	m_PiecewiseFunction->AddPoint(20, 0.0);
	m_PiecewiseFunction->AddPoint(255, 0.2);
	//������ɫ���ݺ���//�ú���ȷ��������ص���ɫֵ���߻Ҷ�ֵ
	m_colorTransferFunction = vtkColorTransferFunction::New();
	m_colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.5, 0.0);        //���ɫ�ʵ㣨��һ������������
	m_colorTransferFunction->AddRGBPoint(60.0, 1.0, 0.0, 0.0);
	m_colorTransferFunction->AddRGBPoint(128.0, 0.2, 0.1, 0.9);
	m_colorTransferFunction->AddRGBPoint(196.0, 0.27, 0.21, 0.1);
	m_colorTransferFunction->AddRGBPoint(255.0, 0.8, 0.8, 0.8);
	//vtkPiecewiseFunction *gradientTransferFunction = vtkPiecewiseFunction::New();//�����ݶȴ��ݺ���
	//gradientTransferFunction->AddPoint(20, 0.0);
	//gradientTransferFunction->AddPoint(255, 2.0);
	//gradientTransferFunction->AddSegment (600, 0.73, 900, 0.9);
	//gradientTransferFunction->AddPoint(1300, 0.1); 
	m_volumeProperty = vtkVolumeProperty::New();					 //�趨һ���������������

	m_volumeProperty->SetColor(m_colorTransferFunction);             //������ɫ
	m_volumeProperty->SetScalarOpacity(m_PiecewiseFunction);		 //��͸����
																	 // m_volumeProperty->SetGradientOpacity(m_PiecewiseFunction);
	m_volumeProperty->ShadeOn();                                     //Ӱ��
	m_volumeProperty->SetInterpolationTypeToLinear();                //ֱ����������ֵ֮���𷢺���
	m_volumeProperty->SetAmbient(0.2);                               //������ϵ��
	m_volumeProperty->SetDiffuse(0.9);                               //������
	m_volumeProperty->SetSpecular(0.2);                              //�߹�ϵ��
	m_volumeProperty->SetSpecularPower(10);                          //�߹�ǿ�� 
																     //���������
																     //�������Ź��ߺϳ�
	// m_VolumeRayCastCompositeFunction = vtkVolumeRayCastCompositeFunction::New();                
	//vtkVolumeRayCastIsosurfaceFunction *m_VolumeRayCastCompositeFunction = vtkVolumeRayCastIsosurfaceFunction::New();

	m_FixedPointVolumeRayCastMapper = vtkFixedPointVolumeRayCastMapper::New();   //�������
	//m_FixedPointVolumeRayCastMapper->SetVolumeRayCastFunction(m_VolumeRayCastCompositeFunction);//������Ʒ���
	m_FixedPointVolumeRayCastMapper->SetInputConnection(m_ImageCast->GetOutputPort());     //ͼ����������
	m_FixedPointVolumeRayCastMapper->SetNumberOfThreads(3);
	//����Volume
	volume1 = vtkVolume::New();										//��ʾ͸ʾͼ�е�һ����ά����
	volume1->SetMapper(m_FixedPointVolumeRayCastMapper);
	volume1->SetProperty(m_volumeProperty);						    //����������
	// //����
#if _DEBUG
	// vtkVolumeWriter *wSP=vtkVolumeWriter::New();
	//// vtkVolume *wSP=vtkVolume::New();
	// wSP->SetInputConnection(m_ImageCast->GetOutputPort());
	// wSP->SetFileName("F://ct/mmmm.vtk");
	// wSP->Write();
	// wSP->Delete();
#endif
	m_ThreeDRec_Renderer->AddVolume(volume1);						//��Volumeװ�ص���������
	// m_ThreeDRec_Renderer->SetBackground(1, 1, 1);
	//m_ThreeDRec_Renderer->SetBackground(0, 0, 0);
	//m_ThreeDRec_Renderer->SetBackground(255, 255, 255);
	//m_ThreeDRec_RenderWindow->SetSize(600, 600);					//���ñ�����ɫ�ͻ��ƴ��ڴ�С
									//���ڽ��л���
	renderWindowInteractor->Initialize();
	this->setWindowTitle(QStringLiteral("��ά�ؽ�-CPU�������"));
	renderWindowInteractor->Start();								//��ʼ�������н�������
	m_ThreeDRec_Renderer->ResetCameraClippingRange();

	m_ThreeDRec_RenderWindow->Render();
}
/*
 * ����GPU��Ⱦ����
 * ����:vtkStringArray *_FilesYouWant һ��series��ȫ�� ��˳���źõ��ļ��� �����ļ���
 */
void ThreeD_Reconstruction::RenderPipeline_GPU(vtkStringArray *_FilesYouWant)
{
	m_ThreeDRec_Renderer = vtkRenderer::New();						  //���û�����(���ƶ���ָ��)
	m_ThreeDRec_RenderWindow = vtkRenderWindow::New();				  //���û��ƴ���
	m_ThreeDRec_RenderWindow->AddRenderer(m_ThreeDRec_Renderer);      //�������߼�����ƴ���
	ui.qvtkWidget_3DRec->SetRenderWindow(m_ThreeDRec_RenderWindow);
	renderWindowInteractor = vtkRenderWindowInteractor::New();		  //���û��ƽ����������ڵ�
	renderWindowInteractor->SetRenderWindow(m_ThreeDRec_RenderWindow);//�����ƴ�����ӵ���������

	vtkDecimatePro *deci = vtkDecimatePro::New();					  //�������ݶ�ȡ�㣬���������������ٽ���
	deci->SetTargetReduction(0.3);

	TrackballCamera = vtkInteractorStyleTrackballCamera::New();       //����ģʽ:����������������       
	renderWindowInteractor->SetInteractorStyle(TrackballCamera);

	m_MyDICOMImageReader = vtkSmartPointer<vtkMyDICOMImageReader>::New();
	m_MyDICOMImageReader->SetFileNames(_FilesYouWant);
	m_MyDICOMImageReader->SetFileDimensionality(3);                   //������ʾͼ���ά��
	m_MyDICOMImageReader->SetDataScalarType(VTK_UNSIGNED_SHORT);
	m_MyDICOMImageReader->SetDataExtent(0, 255, 0, 255, 0, 123);      //ͼƬ����ͼƬ����256x256�������������ʾ��124��ͼ
	m_MyDICOMImageReader->SetDataSpacing(0.9, 0.9, 0.9);			  //�������ؼ���
	// reader->SetDataOrigin(0.0, 0.0, 0.0);						  //���û�׼�㣬��һ��û���ã���������Ƭʱ���ܻ��õ���
	m_ImageCast = vtkImageCast::New();								  //��������ת��
	m_ImageCast->SetInputConnection(m_MyDICOMImageReader->GetOutputPort());
	m_ImageCast->SetOutputScalarTypeToUnsignedShort();
	m_ImageCast->ClampOverflowOn();									  //��ֵ

	//���ò�͸���ȴ��ݺ���//�ú���ȷ����������ػ�λ����ֵ�Ĳ�͸����
	m_PiecewiseFunction = vtkPiecewiseFunction::New();				  //һά�ֶκ����任
	m_PiecewiseFunction->AddPoint(20, 0.0);
	m_PiecewiseFunction->AddPoint(255, 0.2);
	//������ɫ���ݺ���//�ú���ȷ��������ص���ɫֵ���߻Ҷ�ֵ
	m_colorTransferFunction = vtkColorTransferFunction::New();
	m_colorTransferFunction->AddRGBPoint(0.0, 0.0, 0.5, 0.0);         //���ɫ�ʵ㣨��һ������������
	m_colorTransferFunction->AddRGBPoint(60.0, 1.0, 0.0, 0.0);
	m_colorTransferFunction->AddRGBPoint(128.0, 0.2, 0.1, 0.9);
	m_colorTransferFunction->AddRGBPoint(196.0, 0.27, 0.21, 0.1);
	m_colorTransferFunction->AddRGBPoint(255.0, 0.8, 0.8, 0.8);
	//vtkPiecewiseFunction *gradientTransferFunction = vtkPiecewiseFunction::New();//�����ݶȴ��ݺ���
	//gradientTransferFunction->AddPoint(20, 0.0);
	//gradientTransferFunction->AddPoint(255, 2.0);
	//gradientTransferFunction->AddSegment (600, 0.73, 900, 0.9);
	//gradientTransferFunction->AddPoint(1300, 0.1); 
	m_volumeProperty = vtkVolumeProperty::New();					 //�趨һ���������������

	m_volumeProperty->SetColor(m_colorTransferFunction);             //������ɫ
	m_volumeProperty->SetScalarOpacity(m_PiecewiseFunction);		 //��͸����
	// m_volumeProperty->SetGradientOpacity(m_PiecewiseFunction);
	m_volumeProperty->ShadeOn();                                     //Ӱ��
	m_volumeProperty->SetInterpolationTypeToLinear();                //ֱ����������ֵ֮���𷢺���
	m_volumeProperty->SetAmbient(0.2);                               //������ϵ��
	m_volumeProperty->SetDiffuse(0.9);                               //������
	m_volumeProperty->SetSpecular(0.2);                              //�߹�ϵ��
	m_volumeProperty->SetSpecularPower(10);                          //�߹�ǿ�� 
	//���������
	// m_VolumeRayCastCompositeFunction = vtkVolumeRayCastCompositeFunction::New();                   //�������Ź��ߺϳ�
	//vtkVolumeRayCastIsosurfaceFunction *m_VolumeRayCastCompositeFunction = vtkVolumeRayCastIsosurfaceFunction::New(); //�������Ź��ߺϳ�

	m_volumeMapper_gpu = vtkGPUVolumeRayCastMapper::New();   //�������
															 //m_FixedPointVolumeRayCastMapper->SetVolumeRayCastFunction(m_VolumeRayCastCompositeFunction);              //������Ʒ���
	m_volumeMapper_gpu->SetInputConnection(m_ImageCast->GetOutputPort());     //ͼ����������
	//m_volumeMapper_gpu->SetNumberOfThreads(3);
	//����Volume
	volume1 = vtkVolume::New();       //��ʾ͸ʾͼ�е�һ����ά����
	volume1->SetMapper(m_volumeMapper_gpu);
	volume1->SetProperty(m_volumeProperty);							 //����������
										 
#if _DEBUG
	//����	
	// vtkVolumeWriter *wSP=vtkVolumeWriter::New();
	// //vtkVolume *wSP=vtkVolume::New();
	// wSP->SetInputConnection(m_ImageCast->GetOutputPort());
	// wSP->SetFileName("F://ct/mmmm.vtk");
	// wSP->Write();
	// wSP->Delete();
#endif
	m_ThreeDRec_Renderer->AddVolume(volume1);						//��Volumeװ�ص���������
	// ren->SetBackground(1, 1, 1);
	//ren->SetBackground(0, 0, 0);
	//ren->SetBackground(255, 255, 255);
	//m_ThreeDRec_RenderWindow->SetSize(600, 600);					//���ñ�����ɫ�ͻ��ƴ��ڴ�С
	m_ThreeDRec_RenderWindow->Render();								//���ڽ��л���
	renderWindowInteractor->Initialize();
	this->setWindowTitle(QStringLiteral("��ά�ؽ�-GPU�������"));
	renderWindowInteractor->Start();								//��ʼ�������н�������
	m_ThreeDRec_Renderer->ResetCameraClippingRange();
}
