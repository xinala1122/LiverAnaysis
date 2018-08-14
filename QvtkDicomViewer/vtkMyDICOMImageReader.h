#pragma once
#ifndef vtkMyDICOMImageReader_h
#define vtkMyDICOMImageReader_h
#include "vtkIOImageModule.h" // For export macro
#include "vtkImageReader2.h"
#include "vtk-8.0/DICOMAppHelper.h"
#include "vtk-8.0/DICOMParser.h"
/*
 * ��һ�����������ע��:
 * 
 * DICOMParser.h��DICOMAppHelper.h���������vtk��itk�Ľ����й�
 * itk��vtk�ж���ʵ��,����ʵ����������
 * ���һ�������Ҫ��itk��Ҫ��vtk,�������������й�ϵ
 * �������������ǲ���Ҫȥ���ֵ����������Ǹ����������
 * ����Ŀǰ�������һ��,������������vtk�е�һ����,��ô���Ǳ���ȷ������������Ǵ�vtk �Ŀ������ó�����
 * ��Ȼ��Щ������û��ʵ�ֵĻ�����ʵ�ֲ�һ����
 * ��������ܴ���resharper������,����VS����������,
 * �����������ڱ��ƴ��н�����������C++�����ʱ��,��Ҫ�����������ع����ߵ���ʾ��Ҫ�������ļ��ԱȺ�ֱ�Ӵ�������
 */
/*
 * �����������²�һ��VTK�ٷ�ʵ�ֵ�"vtkDicomImageReadere"
 * �������һ�����Ե�©��:
 * ����ֻ�ܴ��ļ��������ļ��е���������ʼ�������
 * ��Ҫ������Ҫ��Ȼ����ֻ�ܶ�һ��ͼ,Ҫ��Ȼ���Ƕ�һ���ļ������������ͼ,
 * ����,����Ҫ������Щ�ļ������ֱ�����ĳЩ����,
 * �����������������������,������û����
 * ʵ������ֻ��Ҫ������һ��"SetFileNames"�ķ���,Ȼ�����ҵ���,������������ĸ���ImageReader2�и���ʵ��,
 * �����Լ���ȴû��ʵ��,
 * ���˷���Ҳ���鷳�����Լ�дһ���а�,�Ҵ����в���....
 * �պ������������һ����ȥ����github�������
 */


class vtkDICOMImageReaderVector;
class DICOMParser;
class DICOMAppHelper;

class /*VTKIOIMAGE_EXPORT*/ vtkMyDICOMImageReader:public vtkImageReader2
{
public:
	//@{
	/**
	* ��̬���칤������,�����������ָ��ʹ��
	*/
	static vtkMyDICOMImageReader *New();
	vtkTypeMacro(vtkMyDICOMImageReader, vtkImageReader2);
	//@}
	unsigned long getImageDateLength();//��ȡͼƬ���ݵĳ���,�������Ϊ0,˵���ⲻ����Ч��DICOMͼƬ�ļ�
	/**
	* ������־���
	*/
	void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

	//@{
	/**
	 *�ӵ����ļ���ʼ��,ע�����к������ļ�����صķ�����������
	*/
	void SetFileName(const char* fn) VTK_OVERRIDE
	{
		delete[] this->DirectoryName;
		delete[] this->FileName;
		this->DirectoryName = NULL;
		this->FileName = NULL;
		this->vtkImageReader2::SetFileName(fn);
	}
	//@}

	/**
	* Set the directory name for the reader to look in for DICOM
	* files. If this method is used, the reader will try to find
	* all the DICOM files in a directory. It will select the subset
	* corresponding to the first series UID it stumbles across and
	* it will try to build an ordered volume from them based on
	* the slice number. The volume building will be upgraded to
	* something more sophisticated in the future.
	*/
	void SetDirectoryName(const char* dn);

	/*
	 * ���ظ����е�SetFileNames,ͻ������
	 * ע��:
	 * 1.���ô˷���,Ŀǰֻ������һ������¶����������:����ɢ�ļ���ʼ��reader��������ά�ؽ�
	 * 2.ע�⺯��������,Ϊ���γɸ���Ч��,����ʹ����vtkStringArray,ע��ʹ������ָ��
	 */
	void SetFileNames(vtkStringArray*) override;
	//@{
	/**
	* Returns the directory name.
	*/
	vtkGetStringMacro(DirectoryName);
	//@}

	/**
	* Returns the pixel spacing (in X, Y, Z).
	* Note: if there is only one slice, the Z spacing is set to the slice
	* thickness. If there is more than one slice, it is set to the distance
	* between the first two slices.
	*/
	double* GetPixelSpacing();

	/**
	* Returns the image width.
	*/
	int GetWidth();

	/**
	* Returns the image height.
	*/
	int GetHeight();

	/**
	* Get the (DICOM) x,y,z coordinates of the first pixel in the
	* image (upper left hand corner) of the last image processed by the
	* DICOMParser
	*/
	float* GetImagePositionPatient();

	/**
	* Get the (DICOM) directions cosines. It consist of the components
	* of the first two vectors. The third vector needs to be computed
	* to form an orthonormal basis.
	*/
	float* GetImageOrientationPatient();

	/**
	* Get the number of bits allocated for each pixel in the file.
	*/
	int GetBitsAllocated();

	/**
	* Get the pixel representation of the last image processed by the
	* DICOMParser. A zero is a unsigned quantity.  A one indicates a
	* signed quantity
	*/
	int GetPixelRepresentation();

	/**
	* Get the number of components of the image data for the last
	* image processed.
	*/
	int GetNumberOfComponents();

	/**
	* Get the transfer syntax UID for the last image processed.
	*/
	const char* GetTransferSyntaxUID();

	/**
	* Get the rescale slope for the pixel data.
	*/
	float GetRescaleSlope();

	/**
	* Get the rescale offset for the pixel data.
	*/
	float GetRescaleOffset();

	/**
	* Get the patient name for the last image processed.
	*/
	const char* GetPatientName();

	/**
	* Get the study uid for the last image processed.
	*/
	const char* GetStudyUID();

	/**
	* Get the Study ID for the last image processed.
	*/
	const char* GetStudyID();

	/**
	* Get the gantry angle for the last image processed.
	*/
	float GetGantryAngle();

	//
	// Can I read the file?
	//
	int CanReadFile(const char* fname) VTK_OVERRIDE;

	//
	// What file extensions are supported?
	//
	const char* GetFileExtensions() VTK_OVERRIDE
	{
		return ".dcm";
	}

	/**
	* Return a descriptive name for the file format that might be useful in a GUI.
	*/
	const char* GetDescriptiveName() VTK_OVERRIDE
	{
		return "DICOM";
	}

protected:
	//
	// Setup the volume size
	//
	void SetupOutputInformation(int num_slices);

	void ExecuteInformation() VTK_OVERRIDE;
	void ExecuteDataWithInformation(vtkDataObject *out, vtkInformation *outInfo) VTK_OVERRIDE;

	//
	// Constructor
	//
	vtkMyDICOMImageReader();

	//
	// Destructor
	//
	~vtkMyDICOMImageReader() VTK_OVERRIDE;

	//
	// Instance of the parser used to parse the file.
	//
	DICOMParser* Parser;

	//
	// Instance of the callbacks that get the data from the file.
	//
	DICOMAppHelper* AppHelper;

	//
	// vtkDICOMImageReaderVector wants to be a PIMPL and it will be, but not quite yet.
	//
	vtkDICOMImageReaderVector* DICOMFileNames;
	char* DirectoryName;

	char* PatientName;
	char* StudyUID;
	char* StudyID;
	char* TransferSyntaxUID;

	// DICOMFileNames accessor methods for subclasses:
	int GetNumberOfDICOMFileNames();
	const char* GetDICOMFileName(int index);
private:
	vtkMyDICOMImageReader(const vtkMyDICOMImageReader&) VTK_DELETE_FUNCTION;
	void operator=(const vtkMyDICOMImageReader&) VTK_DELETE_FUNCTION;

};
#endif