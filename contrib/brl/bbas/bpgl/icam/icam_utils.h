// This is bbas/bpgl/icam/icam_utils.h
#ifndef icam_utils_h_
#define icam_utils_h_
//:
// \file
// \brief Utilities to support image homography calculations
// \author J.L. Mundy
// \date Sept 04, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
#include <icam/icam_image.h>
#include <icam/icam_transform_2d.h>

class icam_utils
{
 public:
  //: destination image bounds and modified transform under a homography
  //  The resampled image bounds map from a rectangle (the source image)
  //  to a quadrilateral in the destination.
  //  This function finds the destination image
  //  bounds and the modified transform to insure all source
  //  points are mapped to the destination with positive coordinates
  //  lying inside the destination bounds.
  static void image_bounds(unsigned source_ni, unsigned source_nj,
                           icam_transform_2d const& t, unsigned& dest_ni,
                           unsigned& dest_nj, icam_transform_2d& mod_trans);
  //:
  //  When resampling a source image to a destination,
  //  some pixels in the destination image may not be valid.
  //  The returned mask is 0 where the destination image does not contain
  //  valid pixels.  Note that transform t is from the destination
  //  to the source. This transform sense is used in icam_sample_grid_bilin
  static icam_image<float> destination_mask(unsigned source_ni,
                                            unsigned source_nj,
                                            icam_transform_2d const& t);
 private:
  // Private constructor/destructor: only static methods in this class.
  icam_utils() = delete;
  ~icam_utils() = delete;
};

#endif // icam_utils_h_
