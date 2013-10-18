#include "vpgl_rational_adjust_multipt.h"
//:
// \file
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>

#include <vcl_limits.h>

double compute_projection_error(vcl_vector<vpgl_rational_camera<double> > const& cams,
                                vcl_vector<vgl_point_2d<double> > const& corrs, vgl_point_3d<double>& intersection)
{
  vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin();
  vcl_vector<vgl_point_2d<double> >::const_iterator rit = corrs.begin();
  double error = 0.0;
  for (; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit)
  {
    vgl_point_2d<double> uvp = (*cit).project(intersection);
    vgl_point_2d<double> uv = *rit;
    double err = vcl_sqrt(vcl_pow(uv.x()-uvp.x(), 2.0) + vcl_pow(uv.y()-uvp.y(), 2));
    error += err;
  }
  return error;
}

double error_corr(vpgl_rational_camera<double> const& cam, vgl_point_2d<double> const& corr, vgl_point_3d<double> const& intersection)
{
  vgl_point_2d<double> uvp = cam.project(intersection);
  return vcl_sqrt(vcl_pow(corr.x()-uvp.x(), 2.0) + vcl_pow(corr.y()-uvp.y(), 2));
}

//: assumes an initial estimate for intersection values, only refines the intersection and computes a re-projection error
double re_projection_error(vcl_vector<vpgl_rational_camera<double> > const& cams,
                           vcl_vector<float> const& cam_weights,
                           vcl_vector<vcl_vector<vgl_point_2d<double> > > const& corrs, // for each 3d corr (outer vector), 2d locations for each cam (inner vector)
                           vcl_vector<vgl_point_3d<double> > const& intersections,
                           vcl_vector<vgl_point_3d<double> >& finals)
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
void re_projection_error(vcl_vector<vpgl_rational_camera<double> > const& cams,
                         vcl_vector<float> const& cam_weights,
                         vcl_vector<vcl_vector<vgl_point_2d<double> > > const& corrs, // for each 3d corr (outer vector), 2d locations for each cam (inner vector)
                         vcl_vector<vgl_point_3d<double> > const& intersections,
                         vcl_vector<vgl_point_3d<double> >& finals,
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


void print_perm(vcl_vector<unsigned>& params_indices)
{
  for (unsigned int i = 0; i < params_indices.size(); ++i)
    vcl_cout << params_indices[i] << ' ';
  vcl_cout << vcl_endl;
}

//: to generate the permutations, always increment the one at the very end by one; if it exceeds max, then increment the one before as well, etc.
bool increment_perm(vcl_vector<unsigned>& params_indices, unsigned size)
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
adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
       vcl_vector<float> const& cam_weights,
       vcl_vector<vcl_vector< vgl_point_2d<double> > > const& corrs,  // a vector of correspondences for each cam
       double radius, int n,       // divide radius into n intervals to generate camera translation space
       vcl_vector<vgl_vector_2d<double> >& cam_translations,          // output translations for each cam
       vcl_vector<vgl_point_3d<double> >& intersections)
{
  cam_translations.clear();
  intersections.clear();
  intersections.resize(corrs.size());
  if (!cams.size() || !corrs.size() || cams.size() != corrs.size())
    return false;
  if (!corrs[0].size())
    return false;
  unsigned int cnt_corrs_for_each_cam = (unsigned)corrs[0].size();
  for (unsigned int i = 1; i < corrs.size(); ++i)
    if (corrs[i].size() != cnt_corrs_for_each_cam)  // there needs to be same number of corrs for each cam
      return false;

  // turn the correspondences into the format that we'll need
  vcl_vector<vgl_point_2d<double> > temp(cams.size());
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs_reformatted(cnt_corrs_for_each_cam, temp);

  for (unsigned int i = 0; i < cnt_corrs_for_each_cam; ++i) { // for each corr
    for (unsigned int j = 0; j < corrs.size(); ++j) // for each cam (corr size and cams size are equal)
      corrs_reformatted[i][j] = corrs[j][i];
  }
  // find the best intersections for all the correspondences using the given cameras to compute good initial estimates for z of each correspondence
  vcl_vector<vgl_point_3d<double> > intersections_initial;
  for (unsigned int i = 0; i < corrs_reformatted.size(); ++i) {
    vgl_point_3d<double> pt;
    if (!vpgl_rational_adjust_onept::find_intersection_point(cams, cam_weights, corrs_reformatted[i], pt))
      return false;
    intersections_initial.push_back(pt);
  }

  // search the camera translation space
  int param_cnt = 2*(int)cams.size();
  double increment = radius/n;
  vcl_vector<double> param_values;
  param_values.push_back(0.0);
  for (int i = 1; i <= n; ++i) {
    param_values.push_back(i*increment);
    param_values.push_back(-i*increment);
  }
  for (unsigned int i = 0; i < param_values.size(); ++i)
    vcl_cout << param_values[i] << ' ';
  vcl_cout << '\n';

  // now for each param go through all possible param values
  vcl_vector<unsigned> params_indices(param_cnt, 0);
  int cnt = (int)vcl_pow((float)param_cnt, (float)param_values.size());
  vcl_cout << "will try: " << cnt << " param combinations: ";
  vcl_cout.flush();
  bool done = false;
  double big_value = 10000000.0;
  double min_error = big_value;
  vcl_vector<unsigned> params_indices_best(params_indices);
  while (!done) {
    vcl_cout << '.';
    vcl_cout.flush();
    vcl_vector<vpgl_rational_camera<double> > current_cams(cams);
    // translate current cams
    for (unsigned int i = 0; i < current_cams.size(); ++i) {
      double u_off,v_off;
      current_cams[i].image_offset(u_off, v_off);
      current_cams[i].set_image_offset(u_off + param_values[params_indices[i*2]], v_off + param_values[params_indices[i*2+1]]);
    }

    // use the initial estimates to compute re-projection errors
    vcl_vector<vgl_point_3d<double> > finals;
    double err = re_projection_error(current_cams, cam_weights, corrs_reformatted, intersections_initial, finals);

    if (err < min_error) {
      min_error = err;
      params_indices_best = params_indices;
      intersections = finals;
    }
    done = increment_perm(params_indices, (unsigned)param_values.size());
  }
  if (min_error < big_value) {
    vcl_cout << " done! found global min! min error: " << min_error << '\n';
    vcl_vector<vpgl_rational_camera<double> > current_cams(cams);
    // return translations
    vcl_cout << "translations for each camera:" << vcl_endl;
    for (unsigned int i = 0; i < current_cams.size(); ++i) {
      vgl_vector_2d<double> tr(param_values[params_indices_best[i*2]], param_values[params_indices_best[i*2+1]]);
      vcl_cout << tr << vcl_endl;
      cam_translations.push_back(tr);
      double u_off,v_off;
      current_cams[i].image_offset(u_off,v_off);
      current_cams[i].set_image_offset(u_off + param_values[params_indices_best[i*2]], v_off + param_values[params_indices_best[i*2+1]]);
    }
  }
  else {
    vcl_cout << " done! no global min!\n";
    return false;
  }
  return true;
}

vpgl_cam_trans_search_lsqr::
vpgl_cam_trans_search_lsqr(vcl_vector<vpgl_rational_camera<double> > const& cams,
                           vcl_vector<float> const& cam_weights,
                           vcl_vector< vcl_vector<vgl_point_2d<double> > > const& image_pts,  // for each 3D corr, an array of 2D corrs for each camera
                           vcl_vector< vgl_point_3d<double> > const& initial_pts)
  :  vnl_least_squares_function(2*(unsigned)cams.size(), (unsigned)(cams.size()*image_pts.size()), vnl_least_squares_function::no_gradient),
     initial_pts_(initial_pts),
     cameras_(cams),
     cam_weights_(cam_weights),
     corrs_(image_pts)
{}

void vpgl_cam_trans_search_lsqr::f(vnl_vector<double> const& translation,   // size is 2*cams.size()
                                   vnl_vector<double>& projection_errors)  // size is cams.size()*image_pts.size() --> compute a residual for each 3D corr point
{
  // compute the new set of cameras with the current cam parameters
  vcl_vector<vpgl_rational_camera<double> > current_cams(cameras_);
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

void vpgl_cam_trans_search_lsqr::get_finals(vcl_vector<vgl_point_3d<double> >& finals)
{
  finals = finals_;
}

//: run Lev-Marq optimization to search the param space to find the best parameter setting
bool vpgl_rational_adjust_multiple_pts::
  adjust_lev_marq(vcl_vector<vpgl_rational_camera<double> > const& cams,
                  vcl_vector<float> const& cam_weights,
                  vcl_vector<vcl_vector< vgl_point_2d<double> > > const& corrs, // a vector of correspondences for each cam
                  vcl_vector<vgl_vector_2d<double> >& cam_translations,         // output translations for each cam
                  vcl_vector<vgl_point_3d<double> >& intersections)             // output 3d locations for each correspondence
{
  cam_translations.clear();
  intersections.clear();
  intersections.resize(corrs.size());
  if (!cams.size() || !corrs.size() || cams.size() != corrs.size())
    return false;
  if (!corrs[0].size())
    return false;
  unsigned int cnt_corrs_for_each_cam = (unsigned)corrs[0].size();
  for (unsigned int i = 1; i < corrs.size(); ++i)
    if (corrs[i].size() != cnt_corrs_for_each_cam)  // there needs to be same number of corrs for each cam
      return false;

  // turn the correspondences into the format that we'll need
  vcl_vector<vgl_point_2d<double> > temp(cams.size());
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs_reformatted(cnt_corrs_for_each_cam, temp);

  for (unsigned int i = 0; i < cnt_corrs_for_each_cam; ++i) { // for each corr
    for (unsigned int j = 0; j < corrs.size(); ++j) // for each cam (corr size and cams size are equal)
      corrs_reformatted[i][j] = corrs[j][i];
  }
  // find the best intersections for all the correspondences using the given cameras to compute good initial estimates for z of each correspondence
  vcl_vector<vgl_point_3d<double> > intersections_initial;
  for (unsigned int i = 0; i < corrs_reformatted.size(); ++i) {
    vgl_point_3d<double> pt;
    if (!vpgl_rational_adjust_onept::find_intersection_point(cams, cam_weights, corrs_reformatted[i], pt))
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
  for (unsigned int i = 0; i < intersections_initial.size(); ++i)
    vcl_cout << "before adjustment initial 3D intersection point: " << intersections_initial[i] << vcl_endl;

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
  vcl_cout << "Minimization x epsilon: " << levmarq.get_f_tolerance() << vcl_endl;

  // Minimize the error and get the best intersection point
  levmarq.minimize(translations);
  levmarq.diagnose_outcome();
  transsf.get_finals(intersections);
  vcl_cout << "final translations:" << vcl_endl;
  for (unsigned int i = 0; i < cams.size(); ++i) {
    // if cam weight is 1 for one of them, it is a special case, then pass 0 as translation for that one, cause projections still cause a tiny translation, ignore that
    vgl_vector_2d<double> trans(0.0, 0.0);
    if (cam_weights[i] == 1.0f) {
      cam_translations.push_back(trans);
    } else {
      trans.set(translations[2*i], translations[2*i+1]);
      cam_translations.push_back(trans);
    }
    vcl_cout << trans << '\n';
  }
  return true;
}
