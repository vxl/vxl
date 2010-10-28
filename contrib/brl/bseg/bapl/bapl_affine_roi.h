// This is algo/bapl/bapl_affine_roi.h
#ifndef bapl_affine_roi_h_
#define bapl_affine_roi_h_
//:
// \file
// \brief A class to represent affine transformed region of interest.
// \author Matt Leotta
// \date 10/14/03
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <bapl/bapl_affine_transform.h>
#include <vil/vil_image_view.h>

//:A class to represent affine transformed region of interest.
// This class represents an affine image patch as a base rectangular
// image and an affine transformation which maps the base image into
// the region of interest

class bapl_affine_roi
{
public:
  //: Constructor
  bapl_affine_roi(vil_image_view<vxl_byte> src_image,
                  bapl_affine_transform xform, 
                  unsigned ni, unsigned nj );

  //: Returns the rectified image
  const vil_image_view<vxl_byte>& rectified_image();

  const bapl_affine_transform& xform(){ return xform_; }


private:
  //: Default Constructor
  bapl_affine_roi();

  //: Rectify the image
  void rectify();

  //: flag that indicates if the rectified image is up to date
  bool rectified_valid_;
  //: The source image
  vil_image_view<vxl_byte> src_img_;
  //: The rectified region of interest
  vil_image_view<vxl_byte> rectified_img_;
  //: The affine transformation
  bapl_affine_transform xform_;
};

#endif // bapl_affine_roi_h_
