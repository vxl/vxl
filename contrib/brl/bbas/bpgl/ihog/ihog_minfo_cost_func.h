// This is brl/bbas/bpgl/ihog/ihog_minfo_cost_func.h
#ifndef ihog_minfo_cost_func_h_
#define ihog_minfo_cost_func_h_
//:
// \file
// \brief A least squares cost function for registering video frames by maximizing mutual information between two frames
// \author Ozge C. Ozcanli
// \date Sep 27, 2011
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <vnl/vnl_vector.h>
#include <vnl/vnl_cost_function.h>
#include <vil/vil_image_view.h>
#include <ihog/ihog_world_roi.h>
#include <ihog/ihog_image.h>
#include <ihog/ihog_transform_2d.h>

//: A least squares cost function for registering video frames by minimizing square difference in intensities
class ihog_minfo_cost_func : public vnl_cost_function
{
 public:
  //: Constructor (no masks)
  ihog_minfo_cost_func(const ihog_image<float>& image1,
                       const ihog_image<float>& image2,
                       ihog_world_roi  roi,
                       const ihog_transform_2d& init_xform,
                       unsigned nbins = 16);
  //: Constructor (one mask)
  ihog_minfo_cost_func(const ihog_image<float>& image1,
                       const ihog_image<float>& image2,
                       const ihog_image<float>& mask,
                       ihog_world_roi  roi,
                       const ihog_transform_2d& init_xform, bool image1_mask = false,
                       unsigned nbins = 16);
  //: Constructor (two masks)
  ihog_minfo_cost_func(const ihog_image<float>& image1,
                       const ihog_image<float>& image2,
                       const ihog_image<float>& mask1,
                       const ihog_image<float>& mask2,
                       ihog_world_roi  roi,
                       const ihog_transform_2d& init_xform,
                       unsigned nbins = 16);


  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  //virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);
  double f(vnl_vector<double> const& x) override;

  //: Returns the transformed second image
  vil_image_view<float> last_xformed_image();

  static double entropy_diff(vnl_vector<double>& mask_samples, vnl_vector<double>& from_samples, vnl_vector<double>& to_samples, int nbins);

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

  unsigned nbins_;
};

#endif // ihog_minfo_cost_func_h_
