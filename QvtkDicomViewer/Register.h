#pragma once

#include <QWidget>
#include "ui_Register.h"
#include <QVTKWidget.h>
#include <vtkImageActor.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleImage.h>
#include <itkImage.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImageFileReader.h>

#define ITK_IO_FACTORY_REGISTER_MANAGER

//�����궨�������Ŀǰ���޷���
//ȱ����Ӧ��ʵ��

enum RegistrationFunc
{
	RegFunc_Translation,
	RegFunc_CenteredSimilarity,
	RegFunc_Affine,
	RegFunc_Multi
};

class Register : public QWidget
{
	Q_OBJECT
	typedef  float           PixelType;
	typedef itk::Image< PixelType, 2 > OutputImageType;
	typedef itk::ImageToVTKImageFilter<OutputImageType>   ConnectorType;
	typedef itk::Image< PixelType, 2 >  FixedImageType;
	typedef itk::Image< PixelType, 2 >  MovingImageType;
	typedef itk::ImageFileReader< FixedImageType  >   FixedImageReaderType;
	typedef itk::ImageFileReader< MovingImageType >   MovingImageReaderType;
public:
	Register(QWidget *parent = Q_NULLPTR);
	~Register();
private:
	Ui::Register ui;
private:
	///��׼����
	void TranslationReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader);
	void CenteredSimilarityTransformReg(FixedImageReaderType::Pointer _fixedImageReader,
		MovingImageReaderType::Pointer _movingImageReader, double initialScale=1.0, double initialAngle=0.0, double steplength=1.0);
	void AffineTransformReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader, double steplength=1.0, unsigned int maxNumberOfIterations=300);
	void MultiTransformReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader, PixelType backgroundGrayLevel=128.0);

	void updateOutputImage();
private slots:
	void OnSelectImageFix();		    //ѡ���׼ͼ��
	void OnSelectImageMove();		    //ѡ�����׼ͼ��
	void OnButtonOk();				    //ok-��ʼ����
	void OnButtonCancel();			    //�˳�

	void OnSelectTranslation();			//ѡ��ƽ�Ʊ任
	void OnSelectCenteredSimilarity();	//ѡ���������ƶ�ά�任
	void OnSelectAffine();				//ѡ�����任
	void OnSelectMulit();				//ѡ��Ĥ���任
private:
	vtkSmartPointer<vtkImageActor> actor[4];
	vtkSmartPointer<vtkRenderer> renderer[4];
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor[4];
	vtkSmartPointer<vtkInteractorStyleImage> style[4];
	QVTKWidget * m_output_widgets[4];
	ConnectorType::Pointer connector[4];
	FixedImageReaderType::Pointer   fixedImageReader ;
	MovingImageReaderType::Pointer  movingImageReader ;
	FixedImageReaderType::Pointer   defaultImageReader;//���ڼ���Ĭ��ͼƬ������ʼ��
	RegistrationFunc m_CurrentRegFunc;//ѡ·����

};