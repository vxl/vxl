// This is algo/bapl/bapl_keypoint_extractor.h
#ifndef bapl_keypoint_extractor_h_
#define bapl_keypoint_extractor_h_
//:
// \file
// \brief Extract Lowe keypoints from an image
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date Sat Nov 8 2003
//
// \verbatim
//  Modifications
// \endverbatim

#include <vxl_config.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <bapl/bapl_keypoint_sptr.h>
#include <bapl/bapl_lowe_keypoint.h>
#include <bapl/bapl_lowe_pyramid_set_sptr.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>


//: Extract the lowe keypoints from an image
bool bapl_keypoint_extractor( const vil_image_resource_sptr & image,
                              vcl_vector<bapl_keypoint_sptr> & keypoints,
                              float curve_ratio = 10.0f );

//: Find the peaks in the DoG pyramid
void bapl_dog_peaks( vcl_vector<vgl_point_3d<float> >& peak_pts,
                     bapl_lowe_pyramid_set_sptr pyramid_set,
                     float curve_ratio = 10.0f);

class bapl_lowe_orientation
{
public:
  //: Constructor
  bapl_lowe_orientation(float sigma, unsigned num_bins);
  //: Compute the orientation at (x,y) using the gradient orientation and magnitude images
  void orient_at( float x, float y, float scale,
                  const vil_image_view<float> & grad_orient,
                  const vil_image_view<float> & grad_mag,
                  vcl_vector<float> & orientations  );
private:
  float sigma_;
  unsigned num_bins_;
  float bin_scale_;
};



#endif // bapl_keypoint_extractor_h_
