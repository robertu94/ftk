#include "vtkDoubleGyreFlowSource.h"
#include "vtkInformation.h"
#include "vtkSmartPointer.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkCellArray.h"
#include "vtkImageData.h"
#include "vtkSphereSource.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include <ftk/filters/critical_point_tracker_2d_regular.hh>
#include <ftk/ndarray/synthetic.hh>
#include <ftk/ndarray/grad.hh>
#include <ftk/ndarray/conv.hh>

vtkStandardNewMacro(vtkDoubleGyreFlowSource);

vtkDoubleGyreFlowSource::vtkDoubleGyreFlowSource() : 
  DW(32), DH(32), DT(10),
  StartTime(0.0), TimeScale(1.0),
  A(0.1), Omega(M_PI*2), Epsilon(0.25)
{
  SetNumberOfInputPorts(0);
  SetNumberOfOutputPorts(1);
}

vtkDoubleGyreFlowSource::~vtkDoubleGyreFlowSource()
{
}

int vtkDoubleGyreFlowSource::FillOutputPortInformation(int, vtkInformation *info)
{
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkImageData");
  return 1;
}

int vtkDoubleGyreFlowSource::RequestInformation(
    vtkInformation*, 
    vtkInformationVector**, 
    vtkInformationVector* outVec)
{
  int extent[6] = {0, DW-1, 0, DH-1, 0, 0};
  double cell_lengths[3] = {1.0, 1.0, 1.0}, 
         origins[3] = {0.0, 0.0, 0.0};

  vtkInformation *outInfo = outVec->GetInformationObject(0);

  // time varying data
  double timeRange[2] = {0.0, DT - 1.0};
  std::vector<double> timeSteps;
  for (int i = 0; i < DT; i ++)
    timeSteps.push_back(static_cast<double>(i));

  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), timeRange, 2);
  outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &timeSteps[0], DT);

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);
  outInfo->Set(vtkDataObject::SPACING(), cell_lengths, 3);
  outInfo->Set(vtkDataObject::ORIGIN(), origins, 3);

  return 1;
}

int vtkDoubleGyreFlowSource::RequestData(
    vtkInformation*, 
    vtkInformationVector** inputVector, 
    vtkInformationVector* outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkImageData *imageData = 
    vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  double currentTime;
  if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()))
    currentTime = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());
  else 
    currentTime = StartTime;

  auto vector_field = ftk::synthetic_double_gyre<float>(DW, DH, currentTime, true, A, Omega, Epsilon);
  auto imageData1 = vector_field.to_vtk_image_data(true);
  imageData->ShallowCopy(imageData1);

#if 0
  int extent[6] = {0, DW-1, 0, DH-1, 0, 0};
  double cell_lengths[3] = {1.0, 1.0, 1.0}, 
         origins[3] = {0.0, 0.0, 0.0};

  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent, 6);
  outInfo->Set(vtkDataObject::SPACING(), cell_lengths, 3);
  outInfo->Set(vtkDataObject::ORIGIN(), origins, 3);
#endif

  return 1;
}
