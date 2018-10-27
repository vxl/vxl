// This is bbas/bpgl/icam/icam_cost_func.h
#ifndef icam_cost_func_h_
#define icam_cost_func_h_
//:
// \file
// \brief A cost function for registering video cameras by minimizing square difference in intensities
// \author J.L. Mundy
// \date Sept 6, 2010
//
// \verbatim
//  Modifications
//   None
// \endverbatim
// In addition to the least squares cost function, this class also provides
// a method for computing errors based on mutual information. Recall that
// mutual information is defined as,
//     H(mapped_source)+H(dest)-H(mapped_source,dest).
// Since the destination pixels are always the same, it is computationally
// more efficient to define an error measure called entropy_diff which is
//   H(mapped_source)-H(mapped_source,dest). A maximum of this quantity
// will also be a maximum of mutual information
// The error methods are used in exhausitive searches over transform
// parameters, while the least_squares_function is used in
// a Levenberg Marquardt solver, which is applied at the final stage of
// camera transform refinement. It is noted that least squares is only
// appropriate when the source and destination images have the same
// illumination direction, as in a video sequence. Mutual information is
// more appropriate when the images have different capture conditions.
//
// to do: develop a least_squares_function for differing capture conditions.
//
#include <utility>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_cost_function.h>
#include <vil/vil_image_view.h>
#include <icam/icam_depth_transform.h>
#include <vbl/vbl_array_2d.h>


//: A cost function for registering video frames by minimizing square difference in intensities.
class icam_cost_func : public vnl_least_squares_function
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  icam_cost_func( const vil_image_view<float>& source_img,
                  const vil_image_view<float>& dest_img,
                  const icam_depth_transform& dt,
                  unsigned nbins = 16);

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  // === debug purposes ===

  //: error based on absolute difference
  double error(vnl_vector_fixed<double, 3> rodrigues,
               vgl_vector_3d<double> trans,
               double min_allowed_overlap = 0.01);

  //: A set of error values for a range of camera parameter values; the \p param_index indicates which parameter is being varied.
  std::vector<double> error(vnl_vector<double> const& x,
                           unsigned param_index, double pmin,
                           double pmax, double pinc);
  //: The joint probability histogram for source and destination pixel values for a given camera transformation.
  vbl_array_2d<double> joint_probability(vnl_vector_fixed<double, 3> rodrigues,
                                         vgl_vector_3d<double> trans);
  //: Joint probability for a given mapped source image
  vbl_array_2d<double>
    joint_probability(vil_image_view<float> const& map_dest,
                      vil_image_view<float> const& map_mask);

  //: Mutual information between the destination and mapped source image
  double mutual_info(vnl_vector_fixed<double, 3> rodrigues,
                     vgl_vector_3d<double> trans,
                     double min_allowed_overlap = 0.01);

  //: Mutual information leaving out the destination entropy
  double entropy_diff(vnl_vector_fixed<double, 3> rodrigues,
                      vgl_vector_3d<double> trans,
                      double min_allowed_overlap = 0.01);

  //: Mutual information for a source image mapped to the destination frame
  double mutual_info(vil_image_view<float> const& map_dest,
                     vil_image_view<float> const& map_mask,
                     double min_allowed_overlap = 0.01);

  //: the fraction of potential number of samples
  double frac_samples() { return (1.0*n_samples_)/max_samples_; }

  //: the full set of samples for a given transformation (useful for debug)
  void samples(vnl_vector_fixed<double, 3> rodrigues,
               vgl_vector_3d<double> trans,
               vnl_vector<double>& trans_source,
               vnl_vector<double>& mask,
               vnl_vector<double>& dest);

  //: the source image mapped to the destination frame (useful for debug)
  vil_image_view<float> mapped_dest(vnl_vector_fixed<double, 3> rodrigues,
                                    vgl_vector_3d<double> trans);
 protected:
  vil_image_view<float> source_image_;
  vil_image_view<float> dest_image_;
  vnl_vector<double> dest_samples_;
  icam_depth_transform dt_;
  unsigned max_samples_;
  unsigned n_samples_;
  unsigned nbins_;
  vbl_array_2d<double> joint_probability(vnl_vector<double> const& samples,
                                         vnl_vector<double> const& mask);
  double minfo(vbl_array_2d<double>& joint_prob);
  double entropy_diff(vbl_array_2d<double>& joint_prob);
};

//: a scalar version of the least squares const function for scalar minimizers
// Currently used in solving for rotation only
class icam_scalar_cost_func : public vnl_cost_function
{
 public:
  icam_scalar_cost_func(icam_cost_func  cost_func)
    : vnl_cost_function(3), min_allowed_overlap_(0.01), cost_func_(std::move(cost_func)) {}
  void set_min_overlap(double min_overlap) { min_allowed_overlap_=min_overlap; }
  //: compute f given the rotation parameters (Rodrigues vector)
  double f(vnl_vector<double> const& x) override;
 protected:
  double min_allowed_overlap_;
  icam_cost_func cost_func_;
};

#endif // icam_cost_func_h_
