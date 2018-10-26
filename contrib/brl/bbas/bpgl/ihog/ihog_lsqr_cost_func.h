// This is bbas/bpgl/ihog/ihog_lsqr_cost_func.h
#ifndef ihog_lsqr_cost_func_h_
#define ihog_lsqr_cost_func_h_
//:
// \file
// \brief A least squares cost function for registering video frames by minimizing square difference in intensities
// \author Matt Leotta
// \date April 13, 2004
//
// \verbatim
//  Modifications
//   G. Tunali - Aug 2010 - removed dependency on vimt
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_transform_2d.h>

//: A least squares cost function for registering video frames by minimizing square difference in intensities
class ihog_lsqr_cost_func : public vnl_least_squares_function
{
 public:
  //: Constructor (no masks)
  ihog_lsqr_cost_func(const ihog_image<float>& image1,
                      const ihog_image<float>& image2,
                      ihog_world_roi  roi,
                      const ihog_transform_2d& init_xform );
  //: Constructor (one mask)
  ihog_lsqr_cost_func(const ihog_image<float>& image1,
                      const ihog_image<float>& image2,
                      const ihog_image<float>& mask,
                      ihog_world_roi  roi,
                      const ihog_transform_2d& init_xform, bool image1_mask = false );
  //: Constructor (two masks)
  ihog_lsqr_cost_func(const ihog_image<float>& image1,
                      const ihog_image<float>& image2,
                      const ihog_image<float>& mask1,
                      const ihog_image<float>& mask2,
                      ihog_world_roi  roi,
                      const ihog_transform_2d& init_xform);


  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  //: Returns the transformed second image
  vil_image_view<float> last_xformed_image();

 protected:
  ihog_image<float> from_image_;
  ihog_image<float> to_image_;
  ihog_image<float> from_mask_image_;
  ihog_image<float> to_mask_image_;

  ihog_world_roi roi_;
  ihog_transform_2d::Form form_;
  vnl_vector<double> from_samples_;

  bool from_mask_; //!< true if mask associated with from_image_
  bool to_mask_;   //!< true if mask associsted with to_image_
};

#endif // ihog_lsqr_cost_func_h_
