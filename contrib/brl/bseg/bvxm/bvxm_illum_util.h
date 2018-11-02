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

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
class bvxm_illum_util
{
 public:
  static bool
    load_surface_nhbds(std::string const& path,
                       std::vector<std::vector<vnl_matrix<float> > >& nhds);
  static bool
    load_illumination_dirs(std::string const& path,
                           std::vector<vnl_double_3>& ill_dirs);

  //:
  // Given a set of illumination directions and image intensities,
  // find the surface normal and ambient illumination factor, where
  // both are weighted by the surface albedo. A minimum of 4 directions
  // are required.
  static bool solve_lambertian_model(std::vector<vnl_double_3> const& ill_dirs,
                                     std::vector<double> const& intensities,
                                     vnl_double_4& model_params,
                                     double& fitting_error);
  //:given an illumination direction and Lambertian model, predict the resulting image intensity
  static double expected_intensity(vnl_double_3 const& illum_dir,
                                   vnl_double_4 const& model_params);
};

#endif // bvxm_illum_util_h_
