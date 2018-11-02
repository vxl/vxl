#include <cmath>
#include <limits>
#include <utility>
#include "vpgl_rational_adjust_multipt.h"
//:
// \file
#include <cassert>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

double compute_projection_error(std::vector<vpgl_rational_camera<double> > const& cams,
                                std::vector<vgl_point_2d<double> > const& corrs, vgl_point_3d<double>& intersection)
{
  auto cit = cams.begin();
  auto rit = corrs.begin();
  double error = 0.0;
  for (; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit)
  {
    vgl_point_2d<double> uvp = (*cit).project(intersection);
    vgl_point_2d<double> uv = *rit;
    double err = std::sqrt(std::pow(uv.x()-uvp.x(), 2.0) + std::pow(uv.y()-uvp.y(), 2));
    error += err;
  }
  return error;
}

double error_corr(vpgl_rational_camera<double> const& cam, vgl_point_2d<double> const& corr, vgl_point_3d<double> const& intersection)
{
  vgl_point_2d<double> uvp = cam.project(intersection);
  return std::sqrt(std::pow(corr.x()-uvp.x(), 2.0) + std::pow(corr.y()-uvp.y(), 2));
}

//: assumes an initial estimate for intersection values, only refines the intersection and computes a re-projection error
double re_projection_error(std::vector<vpgl_rational_camera<double> > const& cams,
                           std::vector<float> const& cam_weights,
                           std::vector<std::vector<vgl_point_2d<double> > > const& corrs, // for each 3d corr (outer vector), 2d locations for each cam (inner vector)
                           std::vector<vgl_point_3d<double> > const& intersections,
                           std::vector<vgl_point_3d<double> >& finals)
{
  double error = 100000.0;

  finals.clear();
  for (unsigned int i = 0; i < corrs.size(); ++i) {
    vgl_point_3d<double> final;
    if (!vpgl_rational_adjust_onept::refine_intersection_pt(cams, cam_weights, corrs[i],intersections[i], final))
      return error;
    finals.push_back(final);
  }

  error = 0;
  for (unsigned int i = 0; i < corrs.size(); ++i) {
    error += compute_projection_error(cams, corrs[i], finals[i]);
  }
  return error;
}

//: assumes an initial estimate for intersection values, only refines the intersection and computes a re-projection error for each corr separately
void re_projection_error(std::vector<vpgl_rational_camera<double> > const& cams,
                         std::vector<float> const& cam_weights,
                         std::vector<std::vector<vgl_point_2d<double> > > const& corrs, // for each 3d corr (outer vector), 2d locations for each cam (inner vector)
                         std::vector<vgl_point_3d<double> > const& intersections,
                         std::vector<vgl_point_3d<double> >& finals,
                         vnl_vector<double>& errors)
{
  double error = 100000.0;
  errors.fill(error);

  finals.clear();
  for (unsigned int i = 0; i < corrs.size(); ++i) {
    vgl_point_3d<double> final;
    if (!vpgl_rational_adjust_onept::refine_intersection_pt(cams, cam_weights, corrs[i],intersections[i], final))
      return;
    finals.push_back(final);
  }

  unsigned k = 0;
  // return an error value for each cam for each corr
  for (unsigned int i = 0; i < corrs.size(); ++i) {
    for (unsigned int j = 0; j < cams.size(); ++j) {
      errors[k] = error_corr(cams[j], corrs[i][j], intersections[i]);
      k++;
    }
  }
}


void print_perm(std::vector<unsigned>& params_indices)
{
  for (unsigned int params_indice : params_indices)
    std::cout << params_indice << ' ';
  std::cout << std::endl;
}

//: to generate the permutations, always increment the one at the very end by one; if it exceeds max, then increment the one before as well, etc.
bool increment_perm(std::vector<unsigned>& params_indices, unsigned size)
{
  if (!params_indices.size())  // no need to permute!!
    return true;

  params_indices[params_indices.size()-1] += 1;
  if (params_indices[params_indices.size()-1] == size) {  // carry on
    params_indices[params_indices.size()-1] = 0;

    if (params_indices.size() < 2)
      return true;  // we're done

    int current_i = (int)params_indices.size()-2;
    while (current_i >= 0) {
      params_indices[current_i] += 1;
      if (params_indices[current_i] < size)
        break;
      if (current_i == 0)
        return true;
      params_indices[current_i] = 0;
      current_i -= 1;
    }
  }
  return false;
}

//: performs an exhaustive search in the parameter space of the cameras.
bool vpgl_rational_adjust_multiple_pts::
adjust(std::vector<vpgl_rational_camera<double> > const& cams,
       std::vector<float> const& cam_weights,
       std::vector<std::vector< vgl_point_2d<double> > > const& corrs,  // a vector of correspondences for each cam
       double radius, int n,       // divide radius into n intervals to generate camera translation space
       std::vector<vgl_vector_2d<double> >& cam_translations,          // output translations for each cam
       std::vector<vgl_point_3d<double> >& intersections)
{
  cam_translations.clear();
  intersections.clear();
  intersections.resize(corrs.size());
  if (!cams.size() || !corrs.size() || cams.size() != corrs.size())
    return false;
  if (!corrs[0].size())
    return false;
  auto cnt_corrs_for_each_cam = (unsigned)corrs[0].size();
  for (unsigned int i = 1; i < corrs.size(); ++i)
    if (corrs[i].size() != cnt_corrs_for_each_cam)  // there needs to be same number of corrs for each cam
      return false;

  // turn the correspondences into the format that we'll need
  std::vector<vgl_point_2d<double> > temp(cams.size());
  std::vector<std::vector<vgl_point_2d<double> > > corrs_reformatted(cnt_corrs_for_each_cam, temp);

  for (unsigned int i = 0; i < cnt_corrs_for_each_cam; ++i) { // for each corr
    for (unsigned int j = 0; j < corrs.size(); ++j) // for each cam (corr size and cams size are equal)
      corrs_reformatted[i][j] = corrs[j][i];
  }
  // find the best intersections for all the correspondences using the given cameras to compute good initial estimates for z of each correspondence
  std::vector<vgl_point_3d<double> > intersections_initial;
  for (const auto & i : corrs_reformatted) {
    vgl_point_3d<double> pt;
    if (!vpgl_rational_adjust_onept::find_intersection_point(cams, cam_weights, i, pt))
      return false;
    intersections_initial.push_back(pt);
  }

  // search the camera translation space
  int param_cnt = 2*(int)cams.size();
  double increment = radius/n;
  std::vector<double> param_values;
  param_values.push_back(0.0);
  for (int i = 1; i <= n; ++i) {
    param_values.push_back(i*increment);
    param_values.push_back(-i*increment);
  }
  for (double param_value : param_values)
    std::cout << param_value << ' ';
  std::cout << '\n';

  // now for each param go through all possible param values
  std::vector<unsigned> params_indices(param_cnt, 0);
  int cnt = (int)std::pow((float)param_cnt, (float)param_values.size());
  std::cout << "will try: " << cnt << " param combinations: ";
  std::cout.flush();
  bool done = false;
  double big_value = 10000000.0;
  double min_error = big_value;
  std::vector<unsigned> params_indices_best(params_indices);
  while (!done) {
    std::cout << '.';
    std::cout.flush();
    std::vector<vpgl_rational_camera<double> > current_cams(cams);
    // translate current cams
    for (unsigned int i = 0; i < current_cams.size(); ++i) {
      double u_off,v_off;
      current_cams[i].image_offset(u_off, v_off);
      current_cams[i].set_image_offset(u_off + param_values[params_indices[i*2]], v_off + param_values[params_indices[i*2+1]]);
    }

    // use the initial estimates to compute re-projection errors
    std::vector<vgl_point_3d<double> > finals;
    double err = re_projection_error(current_cams, cam_weights, corrs_reformatted, intersections_initial, finals);

    if (err < min_error) {
      min_error = err;
      params_indices_best = params_indices;
      intersections = finals;
    }
    done = increment_perm(params_indices, (unsigned)param_values.size());
  }
  if (min_error < big_value) {
    std::cout << " done! found global min! min error: " << min_error << '\n';
    std::vector<vpgl_rational_camera<double> > current_cams(cams);
    // return translations
    std::cout << "translations for each camera:" << std::endl;
    for (unsigned int i = 0; i < current_cams.size(); ++i) {
      vgl_vector_2d<double> tr(param_values[params_indices_best[i*2]], param_values[params_indices_best[i*2+1]]);
      std::cout << tr << std::endl;
      cam_translations.push_back(tr);
      double u_off,v_off;
      current_cams[i].image_offset(u_off,v_off);
      current_cams[i].set_image_offset(u_off + param_values[params_indices_best[i*2]], v_off + param_values[params_indices_best[i*2+1]]);
    }
  }
  else {
    std::cout << " done! no global min!\n";
    return false;
  }
  return true;
}

vpgl_cam_trans_search_lsqr::
vpgl_cam_trans_search_lsqr(std::vector<vpgl_rational_camera<double> > const& cams,
                           std::vector<float>  cam_weights,
                           std::vector< std::vector<vgl_point_2d<double> > > const& image_pts,  // for each 3D corr, an array of 2D corrs for each camera
                           std::vector< vgl_point_3d<double> >  initial_pts)
  :  vnl_least_squares_function(2*(unsigned)cams.size(), (unsigned)(cams.size()*image_pts.size()), vnl_least_squares_function::no_gradient),
     initial_pts_(std::move(initial_pts)),
     cameras_(cams),
     cam_weights_(std::move(cam_weights)),
     corrs_(image_pts)
{}

void vpgl_cam_trans_search_lsqr::f(vnl_vector<double> const& translation,   // size is 2*cams.size()
                                   vnl_vector<double>& projection_errors)  // size is cams.size()*image_pts.size() --> compute a residual for each 3D corr point
{
  // compute the new set of cameras with the current cam parameters
  std::vector<vpgl_rational_camera<double> > current_cams(cameras_);
  // translate current cams
  for (unsigned int i = 0; i < current_cams.size(); ++i) {
    double u_off,v_off;
    current_cams[i].image_offset(u_off, v_off);
    current_cams[i].set_image_offset(u_off + translation[i*2], v_off + translation[i*2+1]);
  }
  // compute the projection error for each cam for each corr
  // use the initial estimates to compute re-projection errors
  re_projection_error(current_cams, cam_weights_, corrs_, initial_pts_, finals_, projection_errors);
}

void vpgl_cam_trans_search_lsqr::get_finals(std::vector<vgl_point_3d<double> >& finals)
{
  finals = finals_;
}

//: run Lev-Marq optimization to search the param space to find the best parameter setting
bool vpgl_rational_adjust_multiple_pts::
  adjust_lev_marq(std::vector<vpgl_rational_camera<double> > const& cams,
                  std::vector<float> const& cam_weights,
                  std::vector<std::vector< vgl_point_2d<double> > > const& corrs, // a vector of correspondences for each cam
                  std::vector<vgl_vector_2d<double> >& cam_translations,         // output translations for each cam
                  std::vector<vgl_point_3d<double> >& intersections)             // output 3d locations for each correspondence
{
  cam_translations.clear();
  intersections.clear();
  intersections.resize(corrs.size());
  if (!cams.size() || !corrs.size() || cams.size() != corrs.size())
    return false;
  if (!corrs[0].size())
    return false;
  auto cnt_corrs_for_each_cam = (unsigned)corrs[0].size();
  for (unsigned int i = 1; i < corrs.size(); ++i)
    if (corrs[i].size() != cnt_corrs_for_each_cam)  // there needs to be same number of corrs for each cam
      return false;

  // turn the correspondences into the format that we'll need
  std::vector<vgl_point_2d<double> > temp(cams.size());
  std::vector<std::vector<vgl_point_2d<double> > > corrs_reformatted(cnt_corrs_for_each_cam, temp);

  for (unsigned int i = 0; i < cnt_corrs_for_each_cam; ++i) { // for each corr
    for (unsigned int j = 0; j < corrs.size(); ++j) // for each cam (corr size and cams size are equal)
      corrs_reformatted[i][j] = corrs[j][i];
  }
  // find the best intersections for all the correspondences using the given cameras to compute good initial estimates for z of each correspondence
  std::vector<vgl_point_3d<double> > intersections_initial;
  for (const auto & i : corrs_reformatted) {
    vgl_point_3d<double> pt;
    if (!vpgl_rational_adjust_onept::find_intersection_point(cams, cam_weights, i, pt))
      return false;
    intersections_initial.push_back(pt);
  }
  // now refine those using Lev_Marqs
  for (unsigned int i = 0; i < corrs_reformatted.size(); ++i) {
    vgl_point_3d<double> final;
    if (!vpgl_rational_adjust_onept::refine_intersection_pt(cams, cam_weights, corrs_reformatted[i],intersections_initial[i], final))
      return false;
    intersections_initial[i] = final;
  }
  for (const auto & i : intersections_initial)
    std::cout << "before adjustment initial 3D intersection point: " << i << std::endl;

  // search the camera translation space using Lev-Marq
  vpgl_cam_trans_search_lsqr transsf(cams, cam_weights, corrs_reformatted, intersections_initial);
  vnl_levenberg_marquardt levmarq(transsf);
  levmarq.set_verbose(true);
  // Set the x-tolerance.  When the length of the steps taken in X (variables)
  // are no longer than this, the minimization terminates.
  levmarq.set_x_tolerance(1e-10);
  // Set the epsilon-function.  This is the step length for FD Jacobian.
  levmarq.set_epsilon_function(0.01);
  // Set the f-tolerance.  When the successive RMS errors are less than this,
  // minimization terminates.
  levmarq.set_f_tolerance(1e-15);
  // Set the maximum number of iterations
  levmarq.set_max_function_evals(10000);
  vnl_vector<double> translations(2*(unsigned)cams.size(), 0.0);
  std::cout << "Minimization x epsilon: " << levmarq.get_f_tolerance() << std::endl;

  // Minimize the error and get the best intersection point
  levmarq.minimize(translations);
  levmarq.diagnose_outcome();
  transsf.get_finals(intersections);
  std::cout << "final translations:" << std::endl;
  for (unsigned int i = 0; i < cams.size(); ++i) {
    // if cam weight is 1 for one of them, it is a special case, then pass 0 as translation for that one, cause projections still cause a tiny translation, ignore that
    vgl_vector_2d<double> trans(0.0, 0.0);
    if (cam_weights[i] == 1.0f) {
      cam_translations.push_back(trans);
    } else {
      trans.set(translations[2*i], translations[2*i+1]);
      cam_translations.push_back(trans);
    }
    // sanity check
    if (std::abs(trans.x()) > 200 || std::abs(trans.y()) > 200) {
      std::cerr << " trans: " << trans << " failed sanity check! returning false!\n";
      return false;
    }
    std::cout << trans << '\n';
  }
  return true;
}

// run Lev-Marq optimization to search the param spae to find the best parameter setting
bool vpgl_rational_adjust_multiple_pts::adjust_lev_marq(std::vector<vpgl_rational_camera<double> > const& cams,          // cameras that will be corrected
                                                        std::vector<float> const& cam_weights,                           // camera weight parameters
                                                        std::vector<std::vector<vgl_point_2d<double> > > const& corrs,    // a vector of correspondences for each cam
                                                        vgl_point_3d<double> const& initial_pt,                         // initial 3-d point for back-projection
                                                        double const& zmin,                                             // minimum allowed height of the 3-d intersection point
                                                        double const& zmax,                                             // maximum allowed height of the 3-d intersection point
                                                        std::vector<vgl_vector_2d<double> >& cam_translations,           // output translations for each camera
                                                        std::vector<vgl_point_3d<double> >& intersections,               // output 3-d locations for each correspondence
                                                        double const relative_diameter)
{
  cam_translations.clear();
  intersections.clear();
  intersections.clear();
  intersections.resize(corrs.size());
  if (!cams.size() || !corrs.size() || cams.size() != corrs.size())
    return false;
  if (!corrs[0].size())
    return false;
  auto cnt_corrs_for_each_cam = (unsigned)corrs[0].size();
  for (unsigned i = 1; i < corrs.size(); ++i)
    if (corrs[i].size() != cnt_corrs_for_each_cam)
      return false;

  // reformat the correspondences array
  std::vector<vgl_point_2d<double> > temp(cams.size());
  std::vector<std::vector<vgl_point_2d<double> > > corrs_reformatted(cnt_corrs_for_each_cam, temp);

  for (unsigned int i = 0; i < cnt_corrs_for_each_cam; ++i) { // for each corr
    for (unsigned int j = 0; j < corrs.size(); ++j) // for each cam (corr size and cams size are equal)
      corrs_reformatted[i][j] = corrs[j][i];
  }
  // find the best 3-d intersections for all the correspondences using the given cameras to compute good initial estimates for z of each correspondence
  std::vector<vgl_point_3d<double> > intersections_initial;
  for (const auto & i : corrs_reformatted) {
    vgl_point_3d<double> pt;
    if (!vpgl_rational_adjust_onept::find_intersection_point(cams, cam_weights, i, initial_pt, zmin, zmax, pt, relative_diameter))
      return false;
    intersections_initial.push_back(pt);
  }

  // now refine the intersections using Lev_Marqs
  for (unsigned i = 0; i < corrs_reformatted.size(); i++) {
    vgl_point_3d<double> final;
    if (!vpgl_rational_adjust_onept::refine_intersection_pt(cams, cam_weights, corrs_reformatted[i],intersections_initial[i], final, relative_diameter))
      return false;
    intersections_initial[i] = final;
  }
  for (const auto & i : intersections_initial)
    std::cout << "before adjustment initial 3D intersection point: " << i << std::endl;

  // search the camera translation space using Lev-Marq
  vpgl_cam_trans_search_lsqr transsf(cams, cam_weights, corrs_reformatted, intersections_initial);
  vnl_levenberg_marquardt levmarq(transsf);
  levmarq.set_verbose(true);
  // Set the x-tolerance.  Minimization terminates when the length of the steps taken in X (variables) are less than input x-tolerance
  levmarq.set_x_tolerance(1e-10);
  // Set the epsilon-function.  This is the step length for FD Jacobian
  levmarq.set_epsilon_function(0.01);
  // Set the f-tolerance.  Minimization terminates when the successive RSM errors are less then this
  levmarq.set_f_tolerance(1e-15);
  // Set the maximum number of iterations
  levmarq.set_max_function_evals(10000);
  vnl_vector<double> translations(2*(unsigned)cams.size(), 0.0);
  std::cout << "Minimization x epsilon: " << levmarq.get_f_tolerance() << std::endl;

  // Minimize the error and get the best intersection point
  levmarq.minimize(translations);
  levmarq.diagnose_outcome();
  transsf.get_finals(intersections);
  std::cout << "final translations:" << std::endl;
  for (unsigned i = 0; i < cams.size(); i++) {
    // if cam weight is 1 for one of them, it is a special case, then pass 0 as translation for that one, cause projections still cause a tiny translation, ignore that
    vgl_vector_2d<double> trans(0.0, 0.0);
    if (cam_weights[i] == 1.0f) {
      cam_translations.push_back(trans);
    }
    else {
      trans.set(translations[2*i], translations[2*i+1]);
      cam_translations.push_back(trans);
    }
    if (std::abs(trans.x()) > 200 || std::abs(trans.y()) > 200) {
      std::cerr << " trans: " << trans << " failed sanity check! returning false!\n";
      return false;
    }
    std::cout << trans << '\n';
  }
  return true;
}
