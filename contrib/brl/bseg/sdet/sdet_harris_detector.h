// This is brl/bseg/sdet/sdet_harris_detector.h
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
// \verbatim
//                        _                           _
//                       | (dI/dx)^2    (dI/dx)(dI/dy) |
//                       |                             |
//  A = Sum(neighborhood)|                             |
//                       |(dI/dx)(dI/dy)   (dI/dx)^2   |
//                       |_                           _|
// \endverbatim
//  over a 2n+1 x 2n+1 neighborhood.
//  The value of k is typically 0.04 as originally recommended by Harris.
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
//  Modifications
// J.L. MUndy December 28, 2004 - added interface for vil images
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <sdet/sdet_harris_detector_params.h>

class sdet_harris_detector : public sdet_harris_detector_params
{
 public:
  //Constructors/destructor
  sdet_harris_detector(sdet_harris_detector_params& rpp);

  ~sdet_harris_detector() override;
  //Accessors
  void set_image(vil1_image const& image);
  void set_image_resource(vil_image_resource_sptr const& image);

  std::vector<vsol_point_2d_sptr>& get_points(){return points_;}

  //Utility Methods
  void extract_corners();
  void clear();

 protected:
  //protected methods
  bool extract_corners_vil1(std::vector<float>& x_pos,
                            std::vector<float>& y_pos,
                            std::vector<float>& val);

  bool extract_corners_vil(std::vector<float>& x_pos,
                           std::vector<float>& y_pos,
                           std::vector<float>& val);
  //members
  bool points_valid_;      //process state flag
  bool use_vil_image_;
  vil1_image image_;  //input image
  vil_image_resource_sptr vimage_;  //input image
  std::vector<vsol_point_2d_sptr> points_; //resulting corners
};

#endif // sdet_harris_detector_h_
