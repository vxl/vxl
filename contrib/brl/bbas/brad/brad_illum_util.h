#ifndef brad_illum_util_h_
#define brad_illum_util_h_
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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_vector_3d.h>
#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>

bool brad_load_surface_nhbds(std::string const& path,
                             std::vector<std::vector<vnl_matrix<float> > >& nhds);

bool brad_load_illumination_dirs(std::string const& path,
                                 std::vector<vnl_double_3>& ill_dirs);
bool brad_load_norm_intensities(std::string const& path,
                                std::vector<double>& norm_ints);


//:
// Given a set of illumination directions and image intensities,
// find the surface normal and ambient illumination factor, where
// both are weighted by the surface albedo. A minimum of 4 directions
// are required.
bool brad_solve_lambertian_model(std::vector<vnl_double_3> const& ill_dirs,
                                 std::vector<double> const& intensities,
                                 vnl_double_4& model_params,
                                 std::vector<double>& fitting_error);

//:given an illumination direction and Lambertian model, predict the resulting image intensity
double brad_expected_intensity(vnl_double_3 const& illum_dir,
                               vnl_double_4 const& model_params);

//:
// Test the error in adding a new image observation to the model
// based on a full solution with the new point. Leave one out method
void brad_solution_error(std::vector<vnl_double_3> const& ill_dirs,
                         std::vector<double> const& intensities,
                         std::vector<double>& fitting_error);

//: Find the nearest illumination orientation
double brad_nearest_ill_dir(std::vector<vnl_double_3> const& ill_dirs,
                            vnl_double_3 const& dir);

// new model requires scene irradiance for each image
void brad_solve_atmospheric_model(const vnl_matrix<double>& illum_dirs,
                                  vnl_matrix<double> intensities,
                                  vnl_vector<double> airlight,
                                  unsigned max_iterations,
                                  double max_fitting_error,
                                  vnl_vector<double>& scene_irrad,
                                  vnl_matrix<double>& surf_normals,
                                  vnl_vector<double>& reflectances);


void brad_solve_atmospheric_model(const vnl_matrix<double>& illum_dirs,
                                  vnl_matrix<double> corr_intens,
                                  unsigned max_iterations,
                                  double max_fitting_error,
                                  vnl_matrix<double>& surf_normals,
                                  vnl_vector<double>& reflectances);

void brad_solution_error(const vnl_matrix<double>& illum_dirs,
                         vnl_matrix<double> intensities,
                         vnl_vector<double> airlight,
                         vnl_vector<double> scene_irrad,
                         const vnl_matrix<double>& surf_normals,
                         vnl_vector<double> reflectances,
                         vnl_matrix<double>& fit_errors,
                         vnl_matrix<double>& pred_intensities);

void brad_solution_error(const vnl_matrix<double>& illum_dirs,
                         vnl_matrix<double> corr_intens,
                         const vnl_matrix<double>& surf_normals,
                         vnl_vector<double> reflectances,
                         vnl_matrix<double>& fit_errors);


double brad_atmos_prediction(vnl_double_3 const& ill_dir,
                             double airlight,
                             double scene_irrad,
                             vnl_double_3 const& surface_norm,
                             double reflectance
                             );

void brad_display_illumination_space_vrml(vnl_matrix<double> illum_dirs,
                                          std::string const& path,
                                          vnl_double_3 degenerate_dir =
                                          vnl_double_3(0,0,0));

double brad_expected_radiance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     brad_image_metadata const& md,
                                     brad_atmospheric_parameters const& atm);

double brad_expected_radiance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     vgl_vector_3d<double> const& sun_dir,
                                     double T_sun,
                                     double T_view,
                                     double solar_irradiance,
                                     double skylight,
                                     double airlight);

double brad_expected_reflectance_chavez(double toa_radiance,
                                        vgl_vector_3d<double> const& normal,
                                        brad_image_metadata const& md,
                                        brad_atmospheric_parameters const& atm);

double brad_expected_reflectance_chavez(double toa_radiance,
                                        vgl_vector_3d<double> const& normal,
                                        vgl_vector_3d<double> const& sun_dir,
                                        double T_sun,
                                        double T_view,
                                        double solar_irradiance,
                                        double skylight,
                                        double airlight);

double brad_radiance_variance_chavez(double reflectance,
                                     vgl_vector_3d<double> const& normal,
                                     brad_image_metadata const& md,
                                     brad_atmospheric_parameters const& atm,
                                     double reflectance_var,
                                     double optical_depth_var,
                                     double skylight_var,
                                     double airlight_var);

#endif // brad_illum_util_h_
