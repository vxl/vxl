//--*-c++-*---this tells emacs to use c++-----
#ifndef sdet_harris_detector_h_
#define sdet_harris_detector_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for extracting Harris corners
//
//  The Harris Corner is defined as a local maximum of 
//   Det(A)-k*Trace^2(A)
//  where 
//                         _                           _
//                       | (dI/dx)^2    (dI/dx)(dI/dy) |
//                       |                             |
//  A = Sum(neighborhood)|                             |
//                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
//                       |_                           _|
//
// over a a 2n+1 x 2n+1 neigborhood
//  The value of k is typically 0.04 as originally recommended by Harris
//
// Note:
//  There is a version in osl, but the plan is to build a suite of
//  feature trackers and the gradient matrix is a fundamental concept
//  across many tracking algorithms. Thus this Harris implementation
//  is built on a more generally usable image processing base.
//
// \author
//  J.L. Mundy - February 26, 2003
//
// \verbatim
// Modifications <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <sdet/sdet_harris_detector_params.h>

class sdet_harris_detector : public sdet_harris_detector_params
{
public:
  //Constructors/destructor
  sdet_harris_detector(sdet_harris_detector_params& rpp);

  ~sdet_harris_detector();
  //Accessors
  void set_image(vil_image& image);
  vcl_vector<vsol_point_2d_sptr>& get_points(){return points_;}

  //Utility Methods
  void extract_corners();
  void clear();

protected:
  //protected methods

  //members
  bool points_valid_;      //process state flag
  vil_image image_;  //input image
  vcl_vector<vsol_point_2d_sptr> points_; //resulting corners
};
#endif // sdet_harris_detector_h_
