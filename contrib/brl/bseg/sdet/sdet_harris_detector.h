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
//  over a 2n+1 x 2n+1 neigborhood.
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
// J.L. MUndy December 28, 2004 - added iterface for vil images
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vil/vil_image_resource.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <sdet/sdet_harris_detector_params.h>

class sdet_harris_detector : public sdet_harris_detector_params
{
 public:
  //Constructors/destructor
  sdet_harris_detector(sdet_harris_detector_params& rpp);

  ~sdet_harris_detector();
  //Accessors
  void set_image(vil1_image const& image);
  void set_image_resource(vil_image_resource_sptr const& image);

  vcl_vector<vsol_point_2d_sptr>& get_points(){return points_;}

  //Utility Methods
  void extract_corners();
  void clear();

 protected:
  //protected methods
  bool extract_corners_vil1(vcl_vector<float>& x_pos,
                            vcl_vector<float>& y_pos,
                            vcl_vector<float>& val);

  bool extract_corners_vil(vcl_vector<float>& x_pos,
                           vcl_vector<float>& y_pos,
                           vcl_vector<float>& val);
  //members
  bool points_valid_;      //process state flag
  bool use_vil_image_;
  vil1_image image_;  //input image
  vil_image_resource_sptr vimage_;  //input image
  vcl_vector<vsol_point_2d_sptr> points_; //resulting corners
};

#endif // sdet_harris_detector_h_
