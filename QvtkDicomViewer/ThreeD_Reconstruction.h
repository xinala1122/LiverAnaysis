#pragma once

#include <QWidget>
#include "ui_ThreeD_Reconstruction.h"
#include <vtkStringArray.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSmartPointer.h>
#include "vtkMyDICOMImageReader.h"
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkImageCast.h>

/*
 * 3ά�ؽ�
 * �����Ǹ�����ʾ��ά�ؽ�����ĵط�
 * Ϊ�˱���һЩ����Ҫ���鷳,ʹ����ģʽ�����ϳ�ʶ
 * ��ά�ؽ��Ľ������������Ļ�������ʾ
 * �������������ά�������һЩ������ʽ
 *
 * ��ʼ��:����Ϊ�����Ӧ����һ��������ʵ������
 * ����һ��series��ȫ���ļ���,�涨�洢��ʽΪvector<std::string>
 * ���Ƚ�������
 * ���������Ⱦ���߽�����Ⱦ
 * ����������;GPU �� CPU
 * 
 * ע��:�����ĺ����Ի�������һ�ּ���:
 *		"��DicomDir�еõ����ļ����ǰ���ɨ��˳�����е�"
 *		���������ڵ�ԭ������������ļ�����series id �����ǲ����ʵ�,�������γɴ�����������������������
 *		
 *		���δ��������һ������:
 *		"�Ӳ�����DicomDir File��,���з�ɢDCM�ļ����ļ����е��ļ��н�����ά�ؽ�"
 *		��ô���ǵļ����û��������,�Ǹ�ʱ�������ڱ����ʼ��֮ǰ,��������취����������
 *		
 *		��֮����ѡ�����������������ļ���������õ�
 *		
 * ���ⷢ��:
 *     ReShaper���ع��ڴ���δͨ�������ʱ�򲢲��ܱ�֤��������
 */
class ThreeD_Reconstruction : public QWidget
{
	Q_OBJECT

public:
	ThreeD_Reconstruction(QWidget *parent = Q_NULLPTR);//Ĭ�Ϲ���
	ThreeD_Reconstruction(std::vector<std::string>*filenames_v,QWidget *parent = Q_NULLPTR);//�����ݹ���
	~ThreeD_Reconstruction();

	/*
	 * ��Ⱦ���߲�����,������Ҫ����ʾ�����ٵ�����Ⱦ
	 */
	void OnReconstruction();
private:
	vtkRenderer * m_ThreeDRec_Renderer;
	vtkRenderWindow *m_ThreeDRec_RenderWindow;
	vtkImageCast *m_ImageCast;
	vtkPiecewiseFunction *m_PiecewiseFunction;
	vtkColorTransferFunction *m_colorTransferFunction;
	vtkVolumeProperty *m_volumeProperty;
	vtkVolumeRayCastCompositeFunction *m_VolumeRayCastCompositeFunction;
	vtkFixedPointVolumeRayCastMapper *m_FixedPointVolumeRayCastMapper;
	vtkVolume *volume1;
	vtkSmartPointer<vtkMyDICOMImageReader> m_MyDICOMImageReader;
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> TrackballCamera;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkGPUVolumeRayCastMapper *m_volumeMapper_gpu;

	vtkSmartPointer<vtkStringArray> m_Filenames;
private:
	Ui::ThreeD_Reconstruction ui;
	void RenderPipeline_CPU(vtkStringArray*);//CPU��Ⱦ����
	void RenderPipeline_GPU(vtkStringArray*);//GPU��Ⱦ����
};
