// \file 
// \brief to represent calibrate object
// \author Kongbin Kang (Kongbin_Kang@Brown.edu)
// \date   4/3/2003
// \verbatim
// Modifications
// \endverbatim

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
