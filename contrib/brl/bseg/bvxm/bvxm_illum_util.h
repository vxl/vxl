#ifndef bvxm_illum_util_h_
#define bvxm_illum_util_h_
//:
// \file
// \brief Various utility methods and classes for modeling illumination
// \author J.L. Mundy
// \date November 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vnl/vnl_matrix.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
class bvxm_illum_util
{
 public:
  static bool
    load_surface_nhbds(vcl_string const& path,
                       vcl_vector<vcl_vector<vnl_matrix<float> > >& nhds);
  static bool
    load_illumination_dirs(vcl_string const& path,
                           vcl_vector<vnl_double_3>& ill_dirs);

  //:
  // Given a set of illumination directions and image intensities,
  // find the surface normal and ambient illumination factor, where
  // both are weighted by the surface albedo. A minimum of 4 directions
  // are required.
  static bool solve_lambertian_model(vcl_vector<vnl_double_3> const& ill_dirs,
                                     vcl_vector<double> const& intensities,
                                     vnl_double_4& model_params,
                                     double& fitting_error);
  //:given an illumination direction and Lambertian model, predict the resulting image intensity
  static double expected_intensity(vnl_double_3 const& illum_dir,
                                   vnl_double_4 const& model_params);
};

#endif // bvxm_illum_util_h_
