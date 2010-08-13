// This is gel/mrc/vpgl/ihog/ihog_lsqr_cost_func.h
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
//   None
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_world_roi.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_transform_2d.h>

//: A least squares cost function for registering video frames by minimizing square difference in intensities
class ihog_lsqr_cost_func : public vnl_least_squares_function
{
 public:
  //: Constructor (no masks)
  ihog_lsqr_cost_func(const vimt_image_2d_of<float>& image1,
                      const vimt_image_2d_of<float>& image2,
                      const ihog_world_roi& roi,
                      const vimt_transform_2d& init_xform );
  //: Constructor (one mask)
  ihog_lsqr_cost_func(const vimt_image_2d_of<float>& image1,
                      const vimt_image_2d_of<float>& image2,
                      const vimt_image_2d_of<float>& mask,
                      const ihog_world_roi& roi,
                      const vimt_transform_2d& init_xform, bool image1_mask = false );
  //: Constructor (two masks)
  ihog_lsqr_cost_func(const vimt_image_2d_of<float>& image1,
                      const vimt_image_2d_of<float>& image2,
                      const vimt_image_2d_of<float>& mask1,
                      const vimt_image_2d_of<float>& mask2,
                      const ihog_world_roi& roi,
                      const vimt_transform_2d& init_xform);


  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Returns the transformed second image
  vil_image_view<float> last_xformed_image();

 protected:
  vimt_image_2d_of<float> from_image_;
  vimt_image_2d_of<float> to_image_;
  vimt_image_2d_of<float> from_mask_image_;
  vimt_image_2d_of<float> to_mask_image_;

  ihog_world_roi roi_;
  vimt_transform_2d::Form form_;
  vnl_vector<double> from_samples_;

  bool from_mask_; //!< true if mask associated with from_image_
  bool to_mask_;   //!< true if mask associsted with to_image_
};

#endif // ihog_lsqr_cost_func_h_
