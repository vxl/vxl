//------------------------------------------------------------------------------
// FileName   : CalibratePlane.h
// Author     : Kongbin Kang (kk@lems.brown.edu)
// Company    : Brown University
// Purpose    : Planar Calibration Object
// Date Of Creation: 3/25/2003
// Modification History :
// Date             Modifications
//------------------------------------------------------------------------------

#ifndef AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_
#define AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>

#include "CalibrationObject.h"

class CalibratePlane : public CalibrationObject
{
  vcl_vector<vgl_homg_point_2d<double> > pts_;
 public:
  int readData(char* fname);
  CalibratePlane();
  virtual ~CalibratePlane();
};

#endif // AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_
