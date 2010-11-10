// This is gel/mrc/vpgl/icam/icam_cost_func.h
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

#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vil/vil_image_view.h>
#include <icam/icam_depth_transform.h>
#include <vbl/vbl_array_2d.h>

//: A cost function for registering video frames by minimizing square difference in intensities
class icam_cost_func : public vnl_least_squares_function
{
 public:
  //: Constructor
  icam_cost_func( const vil_image_view<float>& source_img,
                  const vil_image_view<float>& dest_img,
                  const icam_depth_transform& dt);


  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  virtual void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //debug purposes
  double error(vnl_vector_fixed<double, 3> rodrigues,
               vgl_vector_3d<double> trans,
               double min_allowed_overlap = 0.01);

  vcl_vector<double> error(vnl_vector<double> const& x,
                           unsigned param_index, double pmin,
                           double pmax, double pinc);

  vbl_array_2d<double> joint_probability(vnl_vector_fixed<double, 3> rodrigues,
                                         vgl_vector_3d<double> trans);

  vbl_array_2d<double> 
    joint_probability(vil_image_view<float> const& map_dest,
                      vil_image_view<float> const& map_mask);

  double entropy(vnl_vector_fixed<double, 3> rodrigues,
                 vgl_vector_3d<double> trans,
                 double min_allowed_overlap = 0.01);

  double mutual_info(vnl_vector_fixed<double, 3> rodrigues,
                     vgl_vector_3d<double> trans,
                     double min_allowed_overlap = 0.01);

  double entropy_diff(vnl_vector_fixed<double, 3> rodrigues,
                      vgl_vector_3d<double> trans,
                      double min_allowed_overlap = 0.01);

  double mutual_info(vil_image_view<float> const& map_dest,
                     vil_image_view<float> const& map_mask,
                     double min_allowed_overlap = 0.01);
  //: the fraction of potential number of samples
  double frac_samples()
    {return (1.0*n_samples_)/max_samples_;}

  void samples(vnl_vector_fixed<double, 3> rodrigues,
               vgl_vector_3d<double> trans,
               vnl_vector<double>& trans_source,
               vnl_vector<double>& mask,
               vnl_vector<double>& dest);

  vil_image_view<float> mapped_dest(vnl_vector_fixed<double, 3> rodrigues,
                                    vgl_vector_3d<double> trans);
 protected:
  vil_image_view<float> source_image_;
  vil_image_view<float> dest_image_;
  vnl_vector<double> dest_samples_;
  icam_depth_transform dt_;
  unsigned max_samples_;
  unsigned n_samples_;
  vbl_array_2d<double> joint_probability(vnl_vector<double> const& samples,
                                         vnl_vector<double> const& mask);
  double minfo(vbl_array_2d<double>& joint_prob);
  double entropy_diff(vbl_array_2d<double>& joint_prob);
};

#endif // icam_cost_func_h_

