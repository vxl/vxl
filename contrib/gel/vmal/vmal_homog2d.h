#ifndef vmal_homog2d_h_
#define vmal_homog2d_h_

//:
//  \file
// \author
//   L. Guichard
//--------------------------------------------------------------------------------

#include <iostream>
#include <vector>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vmal_homog2d
{
//***************************************************************************
// Initialization
//***************************************************************************
public:
//---------------------------------------------------------------------------
//: Constructors
//---------------------------------------------------------------------------
  vmal_homog2d();

//---------------------------------------------------------------------------
//: Destructor
//---------------------------------------------------------------------------
  virtual ~vmal_homog2d();

//---------------------------------------------------------------------------
//: Compute the 2d homography from to set of matched points.
// A point pima1[i] in the first image match with the point pima2[i]
// in the second image.
//---------------------------------------------------------------------------
  void compute_homo(const std::vector<vnl_double_3 > & pima1,
            const std::vector<vnl_double_3 > & pima2,
            vnl_double_3x3 &hmatrix);
};

#endif // vmal_homog2d_h_
