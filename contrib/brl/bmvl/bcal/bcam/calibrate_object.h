//------------------------------------------------------------------------------
// FileName             : E:\...\bcam\calibrate_object.h
// Author               : Kongbin Kang (kk@lems.brown.edu)
// Company              : Brown University
// Purpose              : To represent calibration object
// Date Of Creation     : 3/23/2003
// Modification History :
// Date             Modifications
//------------------------------------------------------------------------------


#ifndef AFX_CALIBRATIONOBJECT_H__FDECCBAC_345C_4A9A_BB15_3975763F8F2F__INCLUDED_
#define AFX_CALIBRATIONOBJECT_H__FDECCBAC_345C_4A9A_BB15_3975763F8F2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vcsl/vcsl_spatial.h>

class calibrate_object : public vcsl_spatial
{
 public:
  calibrate_object();
  virtual ~calibrate_object();
};

#endif // AFX_CALIBRATIONOBJECT_H__FDECCBAC_345C_4A9A_BB15_3975763F8F2F__INCLUDED_
