#ifndef __vtkDoubleGyreFlowSource_h
#define __vtkDoubleGyreFlowSource_h

#include "vtkImageAlgorithm.h"
#include "vtkPolyDataAlgorithm.h"

class vtkDataSet;

class vtkDoubleGyreFlowSource : public vtkImageAlgorithm
{
public:
  static vtkDoubleGyreFlowSource *New();
  vtkTypeMacro(vtkDoubleGyreFlowSource, vtkImageAlgorithm);

  void setA(double a) {A = a;}
  void setOmega(double omega) {Omega = omega;}
  void setEpsilon(double epsilon) {Epsilon = epsilon;}
  void setStartTime(double t) {StartTime = t;}
  void setTimeScale(double ts) {TimeScale = ts;}
  void setW(int w) {DW = w;}
  void setH(int h) {DH = h;}
  void setT(int t) {DT = t;}

protected:
  vtkDoubleGyreFlowSource();
  ~vtkDoubleGyreFlowSource();

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int FillOutputPortInformation(int, vtkInformation*) override;

private:
  vtkDoubleGyreFlowSource(const vtkDoubleGyreFlowSource&);
  void operator=(const vtkDoubleGyreFlowSource&);

private:
  double A, Omega, Epsilon;
  double StartTime, TimeScale;
  int DW, DH, DT;
};

#endif