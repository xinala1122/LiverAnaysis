#include "Register.h"
#include <QFileDialog>

#include<head_all.h>//����һ�����ɿص�

#include "itkGDCMImageIO.h"  
#include "itkImageRegistrationMethodv4.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"
#include "itkRegularStepGradientDescentBaseOptimizer.h"
#include <QMessageBox>

/*
 * ����
 */
Register::Register(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	for(int i=0;i<4;i++ )
	{
		actor[i] = vtkSmartPointer<vtkImageActor>::New();
		renderer[i] = vtkSmartPointer<vtkRenderer>::New();
		renderWindowInteractor[i] = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		style[i] = vtkSmartPointer<vtkInteractorStyleImage>::New();
		connector[i] = ConnectorType::New();
	}
	m_output_widgets[0]=ui.qvtkWidget_Registration_UL;
	m_output_widgets[1]=ui.qvtkWidget_Registration_UR;
	m_output_widgets[2]=ui.qvtkWidget_Registration_DL;
	m_output_widgets[3]=ui.qvtkWidget_Registration_DR;
	m_CurrentRegFunc = RegFunc_Translation;//Ĭ��
	fixedImageReader = FixedImageReaderType::New();
	movingImageReader = MovingImageReaderType::New();
	defaultImageReader= FixedImageReaderType::New();
	/*
	 * ����һ��Ĭ��ͼƬˢ������
	 */

	defaultImageReader->SetFileName("./Resources/default.png");
	for(int i=0;i<4;i++)
	{
		connector[i]->SetInput(defaultImageReader->GetOutput());
		connector[i]->Update();
		actor[i]->GetMapper()->SetInputData(connector[i]->GetOutput());
		renderer[i]->AddActor(actor[i]);
		m_output_widgets[i]->GetRenderWindow()->AddRenderer(renderer[i]);
		renderWindowInteractor[i]->SetRenderWindow(m_output_widgets[i]->GetRenderWindow());
		renderWindowInteractor[i]->SetInteractorStyle(style[i]);
		renderWindowInteractor[i]->Initialize();
		m_output_widgets[i]->GetRenderWindow()->Render();
	}
	//renderWindowInteractor[0]->Start();
	//renderWindowInteractor[1]->Start();
	//renderWindowInteractor[2]->Start();
	//renderWindowInteractor[3]->Start();
}
/*
 * ����
 */
Register::~Register()
{
}
/*
 * ƽ�Ʊ任
 */
void Register::TranslationReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader)
{
	typedef itk::TranslationTransform< double, 2 > TransformType;//��׼����
	typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;//�Ż������
	typedef itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType >  MetricType;
	//���ڹ���������������
	typedef itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TransformType >  RegistrationType;
	//ʵ����ʹ������ָ��͹�������
	MetricType::Pointer         metric = MetricType::New();
	OptimizerType::Pointer      optimizer = OptimizerType::New();
	RegistrationType::Pointer   registration = RegistrationType::New();
	//��ص�������ص�registration��
	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);
	//�������Բ�ֵ��
	typedef itk::LinearInterpolateImageFunction<FixedImageType, double > FixedLinearInterpolatorType;
	typedef itk::LinearInterpolateImageFunction<MovingImageType, double > MovingLinearInterpolatorType;
	FixedLinearInterpolatorType::Pointer fixedInterpolator = FixedLinearInterpolatorType::New();
	MovingLinearInterpolatorType::Pointer movingInterpolator = MovingLinearInterpolatorType::New();
	metric->SetFixedInterpolator(fixedInterpolator);
	metric->SetMovingInterpolator(movingInterpolator);
	//��ͼ�����׼��
	//��������ͼƬ�ļ���ȡ��(ע��˴�ʹ�õ���ITK������)
	registration->SetFixedImage(_fixedImageReader->GetOutput());
	registration->SetMovingImage(_movingImageReader->GetOutput());
	//����ƽ�Ʊ任��(���ƶ���������ͼ�õ�)
	TransformType::Pointer movingInitialTransform = TransformType::New();
	TransformType::ParametersType initialParameters(movingInitialTransform->GetNumberOfParameters());
	initialParameters[0] = 0.0;  // Initial offset in mm along X
	initialParameters[1] = 0.0;  // Initial offset in mm along Y
	movingInitialTransform->SetParameters(initialParameters);
	registration->SetMovingInitialTransform(movingInitialTransform);

	//�����ȱ任��(��û�ƶ���������ͼ�õ�)
	TransformType::Pointer   identityTransform = TransformType::New();
	identityTransform->SetIdentity();
	registration->SetFixedInitialTransform(identityTransform);

	//�ݶ��½�������ĳ�ʼ��
	optimizer->SetLearningRate(4);
	optimizer->SetMinimumStepLength(0.001);
	optimizer->SetRelaxationFactor(0.5);
	optimizer->SetNumberOfIterations(200);

	//ƽ��
	const unsigned int numberOfLevels = 1;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

	//����registration
	try
	{
		registration->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return /*EXIT_FAILURE*/;
	}
	//��ȡ���,������������ϵı任
	TransformType::ConstPointer transform = registration->GetTransform();
	TransformType::ParametersType finalParameters = transform->GetParameters();
	const double TranslationAlongX = finalParameters[0];
	const double TranslationAlongY = finalParameters[1];

	//��ѯ��������ĵ�������,�����������������ѧϰ�ʹ�С���µ�(��������,û�к���ʹ��)
	const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	//���ֵ(��������,û�к���ʹ��)
	const double bestValue = optimizer->GetValue();
	//���ϱ任���͵Ķ����ʵ���Ķ����Լ���
	//���յĽ���ǿ�����������
	typedef itk::CompositeTransform<double, 2 > CompositeTransformType;
	CompositeTransformType::Pointer outputCompositeTransform = CompositeTransformType::New();
	outputCompositeTransform->AddTransform(movingInitialTransform);
	outputCompositeTransform->AddTransform(registration->GetModifiableTransform());

	//�ز���,�Դ���׼ͼ��ʹ�������任���в���
	typedef itk::ResampleImageFilter<MovingImageType, FixedImageType >  ResampleFilterType;
	ResampleFilterType::Pointer resampler = ResampleFilterType::New();
	resampler->SetInput(_movingImageReader->GetOutput());
	resampler->SetTransform(outputCompositeTransform);
	//���ز����������������
	FixedImageType::Pointer fixedImage = _fixedImageReader->GetOutput();
	resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	resampler->SetOutputOrigin(fixedImage->GetOrigin());
	resampler->SetOutputSpacing(fixedImage->GetSpacing());
	resampler->SetOutputDirection(fixedImage->GetDirection());
	resampler->SetDefaultPixelValue(100);
	//׼�����

	typedef itk::CastImageFilter<FixedImageType, OutputImageType >          CastFilterType;
	typedef itk::ImageFileWriter< OutputImageType >  WriterType;
	//ת����

	//���1:���������׼���
	//WriterType::Pointer      writer = WriterType::New();
	CastFilterType::Pointer  caster = CastFilterType::New();
	connector[3]->SetInput(caster->GetOutput());//��ʾ������
	//writer->SetFileName(argv[3]);
	caster->SetInput(resampler->GetOutput());
	//writer->SetInput(caster->GetOutput());
	//writer->Update();

	//���2:�Ӳ�ͼA
	typedef itk::SubtractImageFilter<FixedImageType, FixedImageType, FixedImageType > DifferenceFilterType;
	DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	difference->SetInput1(_fixedImageReader->GetOutput());
	difference->SetInput2(resampler->GetOutput());
	typedef itk::RescaleIntensityImageFilter<FixedImageType, OutputImageType > RescalerType;
	RescalerType::Pointer intensityRescaler = RescalerType::New();
	intensityRescaler->SetInput(difference->GetOutput());
	intensityRescaler->SetOutputMinimum(0);
	intensityRescaler->SetOutputMaximum(255);
	resampler->SetDefaultPixelValue(1);
	connector[2]->SetInput(intensityRescaler->GetOutput());//��ʾ�������
	//WriterType::Pointer writer2 = WriterType::New();
	//writer2->SetInput(intensityRescaler->GetOutput());
	//writer2->SetFileName(argv[4]);
	//writer2->Update();

	//���3:�Ӳ�ͼB
	resampler->SetTransform(identityTransform);
	//writer2->SetFileName(argv[5]);
	//writer2->Update();

	updateOutputImage();
}
/*
 * �������ƶ�ά�任
 */
void Register::CenteredSimilarityTransformReg(FixedImageReaderType::Pointer _fixedImageReader, 
	MovingImageReaderType::Pointer _movingImageReader, double initialScale, double initialAngle, double steplength)
{
	//�������ƶ�ά�任
	typedef itk::CenteredSimilarity2DTransform< double > TransformType;
	typedef itk::RegularStepGradientDescentOptimizerv4<double>         OptimizerType;
	typedef itk::MeanSquaresImageToImageMetricv4< FixedImageType, MovingImageType >    MetricType;
	typedef itk::ImageRegistrationMethodv4< FixedImageType, MovingImageType, TransformType > RegistrationType;

	MetricType::Pointer         metric = MetricType::New();
	OptimizerType::Pointer      optimizer = OptimizerType::New();
	RegistrationType::Pointer   registration = RegistrationType::New();
	//registration�������������
	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);
	//��ͼ�����׼��
	//typedef unsigned char                            OutputPixelType;
	//�任��
	TransformType::Pointer  transform = TransformType::New();
	//���������ļ�

	registration->SetFixedImage(_fixedImageReader->GetOutput());
	registration->SetMovingImage(_movingImageReader->GetOutput());
	//�任����ʼ��
	typedef itk::CenteredTransformInitializer<TransformType, FixedImageType, MovingImageType > TransformInitializerType;
	TransformInitializerType::Pointer initializer = TransformInitializerType::New();
	initializer->SetTransform(transform);
	initializer->SetFixedImage(_fixedImageReader->GetOutput());
	initializer->SetMovingImage(_movingImageReader->GetOutput());
	initializer->MomentsOn();
	initializer->InitializeTransform();
	transform->SetScale(initialScale);
	transform->SetAngle(initialAngle);
	//�󶨱任��
	registration->SetInitialTransform(transform);
	registration->InPlaceOn();
	//�Ż��������ʼ��
	typedef OptimizerType::ScalesType OptimizerScalesType;
	OptimizerScalesType optimizerScales(transform->GetNumberOfParameters());
	const double translationScale = 1.0 / 100.0;
	optimizerScales[0] = 10.0;
	optimizerScales[1] = 1.0;
	optimizerScales[2] = translationScale;
	optimizerScales[3] = translationScale;
	optimizerScales[4] = translationScale;
	optimizerScales[5] = translationScale;
	optimizer->SetScales(optimizerScales);
	optimizer->SetLearningRate(steplength);
	optimizer->SetMinimumStepLength(0.0001);
	optimizer->SetNumberOfIterations(500);
	//������׼
	const unsigned int numberOfLevels = 1;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

	try
	{
		registration->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return /*EXIT_FAILURE*/;
	}
	//һ�������Ϣ(��������,��������������)
	TransformType::ParametersType finalParameters = transform->GetParameters();
	const double finalScale = finalParameters[0];
	const double finalAngle = finalParameters[1];
	const double finalRotationCenterX = finalParameters[2];
	const double finalRotationCenterY = finalParameters[3];
	const double finalTranslationX = finalParameters[4];
	const double finalTranslationY = finalParameters[5];
	const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	const double bestValue = optimizer->GetValue();
	const double finalAngleInDegrees = finalAngle * 180.0 / itk::Math::pi;

	//�ز���
	typedef itk::ResampleImageFilter< MovingImageType, FixedImageType > ResampleFilterType;
	ResampleFilterType::Pointer resampler = ResampleFilterType::New();

	resampler->SetTransform(transform);
	resampler->SetInput(_movingImageReader->GetOutput());

	FixedImageType::Pointer fixedImage = _fixedImageReader->GetOutput();

	resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	resampler->SetOutputOrigin(fixedImage->GetOrigin());
	resampler->SetOutputSpacing(fixedImage->GetSpacing());
	resampler->SetOutputDirection(fixedImage->GetDirection());
	resampler->SetDefaultPixelValue(100);

	/*typedef  unsigned char  OutputPixelType;//png���������unsigned char
	typedef itk::Image< OutputPixelType, Dimension > OutputImageType;*/
	typedef itk::CastImageFilter< FixedImageType, OutputImageType >CastFilterType;
	typedef itk::ImageFileWriter< OutputImageType >  WriterType;

	//���1
	//WriterType::Pointer      writer = WriterType::New();
	CastFilterType::Pointer  caster = CastFilterType::New();
	//writer->SetFileName(argv[3]);
	connector[3]->SetInput(caster->GetOutput());//���׷�casterǰ�滹�Ǻ���
	caster->SetInput(resampler->GetOutput());
	
	//writer->SetInput(caster->GetOutput());
	//writer->Update();

	//���2
	typedef itk::SubtractImageFilter<FixedImageType, FixedImageType, FixedImageType > DifferenceFilterType;
	DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	typedef itk::RescaleIntensityImageFilter<FixedImageType, OutputImageType >   RescalerType;
	RescalerType::Pointer intensityRescaler = RescalerType::New();
	intensityRescaler->SetInput(difference->GetOutput());
	intensityRescaler->SetOutputMinimum(0);
	intensityRescaler->SetOutputMaximum(255);
	difference->SetInput1(_fixedImageReader->GetOutput());
	difference->SetInput2(resampler->GetOutput());
	resampler->SetDefaultPixelValue(1);
	
	connector[2]->SetInput(intensityRescaler->GetOutput());

	//WriterType::Pointer writer2 = WriterType::New();
	//writer2->SetInput(intensityRescaler->GetOutput());
	//writer2->SetFileName(argv[5]);
	//writer2->Update();

	//���3
	typedef itk::IdentityTransform< double, 2 > IdentityTransformType;
	IdentityTransformType::Pointer identity = IdentityTransformType::New();
	resampler->SetTransform(identity);
	//writer2->SetFileName(argv[4]);
	//writer2->Update();


	updateOutputImage();


	return /*EXIT_SUCCESS*/;
}
/*
 * ����任
 */
void Register::AffineTransformReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader,double steplength, unsigned int maxNumberOfIterations)
{
	//����任
	typedef itk::AffineTransform< double, 2  > TransformType;
	typedef itk::RegularStepGradientDescentOptimizerv4<double>       OptimizerType;
	typedef itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType > MetricType;
	typedef itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TransformType > RegistrationType;

	MetricType::Pointer         metric = MetricType::New();
	OptimizerType::Pointer      optimizer = OptimizerType::New();
	RegistrationType::Pointer   registration = RegistrationType::New();
	//registration���������е����
	registration->SetMetric(metric);
	registration->SetOptimizer(optimizer);
	//�任��
	TransformType::Pointer  transform = TransformType::New();

	registration->SetFixedImage(_fixedImageReader->GetOutput());
	registration->SetMovingImage(_movingImageReader->GetOutput());

	//��ʼ���任��
	typedef itk::CenteredTransformInitializer<TransformType, FixedImageType, MovingImageType > TransformInitializerType;
	TransformInitializerType::Pointer initializer = TransformInitializerType::New();
	initializer->SetTransform(transform);
	initializer->SetFixedImage(_fixedImageReader->GetOutput());
	initializer->SetMovingImage(_movingImageReader->GetOutput());
	initializer->MomentsOn();
	initializer->InitializeTransform();
	registration->SetInitialTransform(transform);
	registration->InPlaceOn();
	//�Ż��������ʼ��
	double translationScale = 1.0 / 1000.0;
	typedef OptimizerType::ScalesType       OptimizerScalesType;
	OptimizerScalesType optimizerScales(transform->GetNumberOfParameters());
	optimizerScales[0] = 1.0;
	optimizerScales[1] = 1.0;
	optimizerScales[2] = 1.0;
	optimizerScales[3] = 1.0;
	optimizerScales[4] = translationScale;
	optimizerScales[5] = translationScale;
	optimizer->SetScales(optimizerScales);
	optimizer->SetLearningRate(steplength);
	optimizer->SetMinimumStepLength(0.0001);
	optimizer->SetNumberOfIterations(maxNumberOfIterations);
	//ƽ��
	const unsigned int numberOfLevels = 1;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
	//����registration
	try
	{
		registration->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;
		return;
	}
	//һ�������Ϣ(ֻ��������,û�к���ʹ��)
	const TransformType::ParametersType finalParameters = registration->GetOutput()->Get()->GetParameters();
	const double finalRotationCenterX = transform->GetCenter()[0];
	const double finalRotationCenterY = transform->GetCenter()[1];
	const double finalTranslationX = finalParameters[4];
	const double finalTranslationY = finalParameters[5];

	const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	const double bestValue = optimizer->GetValue();


	//Compute the rotation angle and scaling from SVD of the matrix
	// \todo Find a way to figure out if the scales are along X or along Y.
	// VNL returns the eigenvalues ordered from largest to smallest.

	vnl_matrix<double> p(2, 2);
	p[0][0] = (double)finalParameters[0];
	p[0][1] = (double)finalParameters[1];
	p[1][0] = (double)finalParameters[2];
	p[1][1] = (double)finalParameters[3];
	vnl_svd<double> svd(p);
	vnl_matrix<double> r(2, 2);
	r = svd.U() * vnl_transpose(svd.V());
	double angle = std::asin(r[1][0]);

	const double angleInDegrees = angle * 180.0 / itk::Math::pi;

	//std::cout << " Scale 1         = " << svd.W(0) << std::endl;
	//std::cout << " Scale 2         = " << svd.W(1) << std::endl;
	//std::cout << " Angle (degrees) = " << angleInDegrees << std::endl;


	// �ز���
	typedef itk::ResampleImageFilter<MovingImageType, FixedImageType >  ResampleFilterType;
	ResampleFilterType::Pointer resampler = ResampleFilterType::New();
	resampler->SetTransform(transform);
	resampler->SetInput(_movingImageReader->GetOutput());
	FixedImageType::Pointer fixedImage = _fixedImageReader->GetOutput();

	resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	resampler->SetOutputOrigin(fixedImage->GetOrigin());
	resampler->SetOutputSpacing(fixedImage->GetSpacing());
	resampler->SetOutputDirection(fixedImage->GetDirection());
	resampler->SetDefaultPixelValue(100);

	//typedef  unsigned char  OutputPixelType;
	//typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
	typedef itk::CastImageFilter<FixedImageType, OutputImageType > CastFilterType;
	typedef itk::ImageFileWriter< OutputImageType >  WriterType;

	//���1
	//WriterType::Pointer      writer = WriterType::New();
	CastFilterType::Pointer  caster = CastFilterType::New();
	//writer->SetFileName(argv[3]);
	connector[3]->SetInput(caster->GetOutput());
	caster->SetInput(resampler->GetOutput());
	//writer->SetInput(caster->GetOutput());
	//writer->Update();

	//���2
	typedef itk::SubtractImageFilter<FixedImageType, FixedImageType, FixedImageType > DifferenceFilterType;
	DifferenceFilterType::Pointer difference = DifferenceFilterType::New();
	difference->SetInput1(_fixedImageReader->GetOutput());
	difference->SetInput2(resampler->GetOutput());
	//WriterType::Pointer writer2 = WriterType::New();

	typedef itk::RescaleIntensityImageFilter<FixedImageType, OutputImageType >   RescalerType;
	RescalerType::Pointer intensityRescaler = RescalerType::New();
	intensityRescaler->SetInput(difference->GetOutput());
	intensityRescaler->SetOutputMinimum(0);
	intensityRescaler->SetOutputMaximum(255);

	//writer2->SetInput(intensityRescaler->GetOutput());
	connector[2]->SetInput(intensityRescaler->GetOutput());
	resampler->SetDefaultPixelValue(1);
	//writer2->SetFileName(argv[5]);
	//writer2->Update();

	//���3
	typedef itk::IdentityTransform< double, 2 > IdentityTransformType;
	IdentityTransformType::Pointer identity = IdentityTransformType::New();
	resampler->SetTransform(identity);
	//writer2->SetFileName(argv[4]);
	//writer2->Update();
	updateOutputImage();
	return;
}
/*
 * ���ַ�ʽ�任
 */
void Register::MultiTransformReg(FixedImageReaderType::Pointer _fixedImageReader, MovingImageReaderType::Pointer _movingImageReader, PixelType backgroundGrayLevel)
{
	typedef itk::TranslationTransform< double, 2 >              TransformType;
	typedef itk::RegularStepGradientDescentOptimizerv4<double>          OptimizerType;
	typedef itk::MattesMutualInformationImageToImageMetricv4<FixedImageType, MovingImageType > MetricType;
	typedef itk::ImageRegistrationMethodv4<FixedImageType, MovingImageType, TransformType > RegistrationType;
	//registration�������������
	TransformType::Pointer      transform = TransformType::New();
	OptimizerType::Pointer      optimizer = OptimizerType::New();
	MetricType::Pointer         metric = MetricType::New();
	RegistrationType::Pointer   registration = RegistrationType::New();
	registration->SetOptimizer(optimizer);
	registration->SetMetric(metric);
	//��ȡ���������ļ�
	registration->SetFixedImage(_fixedImageReader->GetOutput());
	registration->SetMovingImage(_movingImageReader->GetOutput());
	//��ʼ��
	typedef OptimizerType::ParametersType ParametersType;
	ParametersType initialParameters(transform->GetNumberOfParameters());
	initialParameters[0] = 0.0;  // Initial offset in mm along X
	initialParameters[1] = 0.0;  // Initial offset in mm along Y
	transform->SetParameters(initialParameters);
	registration->SetInitialTransform(transform);
	registration->InPlaceOn();
	metric->SetNumberOfHistogramBins(24);

	optimizer->SetNumberOfIterations(200);
	optimizer->SetRelaxationFactor(0.5);
	//ƽ��
	const unsigned int numberOfLevels = 3;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(3);
	shrinkFactorsPerLevel[0] = 3;
	shrinkFactorsPerLevel[1] = 2;
	shrinkFactorsPerLevel[2] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(3);
	smoothingSigmasPerLevel[0] = 0;
	smoothingSigmasPerLevel[1] = 0;
	smoothingSigmasPerLevel[2] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	//����registration
	try
	{
		registration->Update();
	}
	catch (itk::ExceptionObject & err)
	{
		std::cout << "ExceptionObject caught !" << std::endl;
		std::cout << err << std::endl;
		return /*EXIT_FAILURE*/;
	}
	//һ���������(ֻ��������,û�к���ʹ��)
	ParametersType finalParameters = transform->GetParameters();
	double TranslationAlongX = finalParameters[0];
	double TranslationAlongY = finalParameters[1];
	unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	double bestValue = optimizer->GetValue();

	//�ز���
	typedef itk::ResampleImageFilter<MovingImageType, FixedImageType >    ResampleFilterType;
	ResampleFilterType::Pointer resample = ResampleFilterType::New();
	resample->SetTransform(transform);
	resample->SetInput(_movingImageReader->GetOutput());
	FixedImageType::Pointer fixedImage = _fixedImageReader->GetOutput();
	resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	resample->SetOutputOrigin(fixedImage->GetOrigin());
	resample->SetOutputSpacing(fixedImage->GetSpacing());
	resample->SetOutputDirection(fixedImage->GetDirection());
	resample->SetDefaultPixelValue(backgroundGrayLevel);
	//׼�����
	//typedef  unsigned char  OutputPixelType;
	//typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
	typedef itk::CastImageFilter<FixedImageType, OutputImageType > CastFilterType;
	typedef itk::ImageFileWriter< OutputImageType >  WriterType;

	//���1
	//WriterType::Pointer      writer = WriterType::New();
	CastFilterType::Pointer  caster = CastFilterType::New();
	//writer->SetFileName(outImagefile);
	
	caster->SetInput(resample->GetOutput());
	caster->Update();
connector[3]->SetInput(resample->GetOutput());
	//writer->SetInput(caster->GetOutput());
	//writer->Update();

	// ׼�������̸�ͼ��
	typedef itk::CheckerBoardImageFilter< FixedImageType > CheckerBoardFilterType;
	CheckerBoardFilterType::Pointer checker = CheckerBoardFilterType::New();
	checker->SetInput1(fixedImage);
	checker->SetInput2(resample->GetOutput());
	caster->SetInput(checker->GetOutput());
	
	//writer->SetInput(caster->GetOutput());
	resample->SetDefaultPixelValue(0);

	//���:2 registration֮ǰ�����̸�
	TransformType::Pointer identityTransform = TransformType::New();
	identityTransform->SetIdentity();
	resample->SetTransform(identityTransform);
	//writer->SetFileName(checkerBoardBefore);
	//writer->Update();

	// ���3: registration֮������̸�
	resample->SetTransform(transform);
connector[2]->SetInput(caster->GetOutput());
	//writer->SetFileName(checkerBoardAfter);
	//writer->Update();
	updateOutputImage();
	return /*EXIT_SUCCESS*/;
}
/*
 * ˢ��ͼƬ
 */
void Register::updateOutputImage()
{
	setWindowTitle(QStringLiteral("��׼����:�������!"));
	for (int i = 2; i<4; i++)
	{
		connector[i]->Update();
		actor[i]->GetMapper()->SetInputData(connector[i]->GetOutput());
		renderer[i]->AddActor(actor[i]);
		m_output_widgets[i]->GetRenderWindow()->AddRenderer(renderer[i]);
		//m_output_widgets[i]->GetRenderWindow()->Render();
		renderWindowInteractor[i]->SetRenderWindow(m_output_widgets[i]->GetRenderWindow());
		renderWindowInteractor[i]->SetInteractorStyle(style[i]);
		renderWindowInteractor[i]->Initialize();
	}
	for (int i = 2; i<4; i++)
	{
		m_output_widgets[i]->GetRenderWindow()->Render();
	}
	//�±ߵ����������,��Ҫ����ѭ����
	renderWindowInteractor[2]->Start();
	renderWindowInteractor[3]->Start();
}

/*
 * ѡ���׼ͼƬ
 */
void Register::OnSelectImageFix()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ���׼ͼ��"), NULL, tr("*.*"));
	ui.lineEdit_ImageFix->setText(fileName);
	if (fileName.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("��ѡ���ļ�ͼƬ!"));
	}
	else
	{
		try
		{
			fixedImageReader->SetFileName(fileName.toStdString());
			fixedImageReader->Update();
			connector[0]->SetInput(fixedImageReader->GetOutput());//��ʾ����1
			connector[0]->Update();
			actor[0]->GetMapper()->SetInputData(connector[0]->GetOutput());
			renderer[0]->AddActor(actor[0]);
			m_output_widgets[0]->GetRenderWindow()->AddRenderer(renderer[0]);
			renderWindowInteractor[0]->SetRenderWindow(m_output_widgets[0]->GetRenderWindow());
			renderWindowInteractor[0]->SetInteractorStyle(style[0]);
			renderWindowInteractor[0]->Initialize();
			m_output_widgets[0]->GetRenderWindow()->Render();
			renderWindowInteractor[0]->Start();
		}
		catch (itk::ExceptionObject & err)
		{
			QString errorMsg= err.GetDescription();
			QMessageBox::information(this, QStringLiteral("�������ش���!"), errorMsg);
		}
		
	}
}
/*
 * ѡ�����׼ͼƬ
 */
void Register::OnSelectImageMove()
{
	QString fileName = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ�����׼ͼ��"), NULL, tr("*.*"));
	ui.lineEdit_ImageMove->setText(fileName);
	if (fileName.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("��ʾ"), QStringLiteral("��ѡ���ļ�ͼƬ!"));
	}
	else
	{
		try
		{
			movingImageReader->SetFileName(fileName.toStdString());
			movingImageReader->Update();
			connector[1]->SetInput(movingImageReader->GetOutput());//��ʾ����1
			connector[1]->Update();
			actor[1]->GetMapper()->SetInputData(connector[1]->GetOutput());
			renderer[1]->AddActor(actor[1]);
			m_output_widgets[1]->GetRenderWindow()->AddRenderer(renderer[1]);
			renderWindowInteractor[1]->SetRenderWindow(m_output_widgets[1]->GetRenderWindow());
			renderWindowInteractor[1]->SetInteractorStyle(style[1]);
			renderWindowInteractor[1]->Initialize();
			m_output_widgets[1]->GetRenderWindow()->Render();
			renderWindowInteractor[1]->Start();
		}
		catch (itk::ExceptionObject & err)
		{
			QString errorMsg = err.GetDescription();
			QMessageBox::information(this, QStringLiteral("�������ش���!"), errorMsg);
		}

	}
}
/*
 * ��ʼ����
 */
void Register::OnButtonOk()
{
	setWindowTitle(QStringLiteral("��׼����:���ڼ���..."));
	switch (m_CurrentRegFunc)
	{
	case RegFunc_Translation:
		TranslationReg(fixedImageReader, movingImageReader);
		break;
	case RegFunc_CenteredSimilarity:
		CenteredSimilarityTransformReg(fixedImageReader, movingImageReader);
		break;
	case RegFunc_Affine:
		AffineTransformReg(fixedImageReader, movingImageReader);
		break;
	case RegFunc_Multi:
		MultiTransformReg(fixedImageReader, movingImageReader);
		break;
	default:
		break;
	}
	//this->close();
}
/*
 * �˳�
 */
void Register::OnButtonCancel()
{
	this->close();
}
/*
 * ѡ��ƽ�Ʊ任
 */
void Register::OnSelectTranslation()
{
	m_CurrentRegFunc = RegFunc_Translation;
}
/*
 * ѡ�����ĶԳƶ�ά�任
 */
void Register::OnSelectCenteredSimilarity()
{
	m_CurrentRegFunc = RegFunc_CenteredSimilarity;
}
/*
 * ѡ�����任
 */
void Register::OnSelectAffine()
{
	m_CurrentRegFunc = RegFunc_Affine;
}
/*
 * ѡ��Ĥ���任
 */
void Register::OnSelectMulit()
{
	m_CurrentRegFunc = RegFunc_Multi;
}
