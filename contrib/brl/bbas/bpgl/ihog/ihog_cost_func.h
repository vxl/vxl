// This is bbas/bpgl/ihog/ihog_cost_func.h
#ifndef ihog_cost_func_h_
#define ihog_cost_func_h_
//:
// \file
// \brief A cost function for registering video frames by minimizing square difference in intensities
// \author Matt Leotta
// \date April 13, 2004
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vil/vil_image_view.h>
#include "ihog_world_roi.h"
#include "ihog_image.h"
#include "ihog_transform_2d.h"

//: A cost function for registering video frames by minimizing square difference in intensities
class ihog_cost_func : public vnl_cost_function
{
 public:
  //: Constructor
  ihog_cost_func( const vil_image_view<float>& image1,
                  const vil_image_view<float>& image2,
                  ihog_world_roi  roi,
                  const ihog_transform_2d& init_xform );
  //: Constructor
  ihog_cost_func( const vil_image_view<float>& image1,
                  const vil_image_view<float>& image2,
                  const vil_image_view<float>& mask,
                  ihog_world_roi  roi,
                  const ihog_transform_2d& init_xform );
  //: The main function.
  double f(vnl_vector<double> const& x) override;

  //: Returns the transformed second image
  vil_image_view<float> last_xformed_image();

 protected:
  ihog_image<float> from_image_;
  ihog_image<float> to_image_;
  ihog_world_roi roi_;
  ihog_transform_2d::Form form_;
  vil_image_view<bool> mask_image_;
};

#endif // ihog_cost_func_h_
