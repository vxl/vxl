//------------------------------------------------------------------------------
// FileName   : bcal_calibrate_plane.h
// Author     : Kongbin Kang (kk@lems.brown.edu)
// Company    : Brown University
// Purpose    : Planar Calibration Object
// Date Of Creation: 3/25/2003
// Modification History :
// Date             Modifications
//------------------------------------------------------------------------------

#ifndef AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_
#define AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_

#if defined(_MSC_VER) && ( _MSC_VER > 1000 )
#pragma once
#endif // _MSC_VER > 1000

#include <vcl_vector.h>
#include <vgl/vgl_homg_point_2d.h>

#include "bcal_calibrate_object.h"

class bcal_calibrate_plane : public bcal_calibrate_object
{
  vcl_vector<vgl_homg_point_2d<double> > pts_;
 public:
  int read_data(vcl_vector<vgl_homg_point_2d<double> > &pts);
  vcl_vector<vgl_homg_point_2d<double> >& get_points() { return pts_;}
  int readData(const char* fname);
  bcal_calibrate_plane();
  virtual ~bcal_calibrate_plane();
};

#endif // AFX_CALIBRATEPLANE_H__22DD25A2_6BFF_46DE_83C2_0285C7008E46__INCLUDED_
