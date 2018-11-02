// This is brl/bbas/bpgl/icam/icam_minimizer.cxx
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include "icam_minimizer.h"
//:
// \file

#include <vnl/vnl_inverse.h>
#include <vnl/vnl_numeric_traits.h>
#include <vbl/vbl_local_minima.h>
#include <vbl/vbl_array_3d.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/algo/vnl_powell.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_timer.h>
#include <icam/icam_depth_trans_pyramid.h>
#include <icam/icam_sample.h>
#include <vpgl/algo/vpgl_ray.h>

#include <vil/vil_image_view.h>

static bool smallest_local_minima(vbl_array_3d<double> const& in,
                                  double min_thresh,
                                  int & ix_min, int & iy_min,
                                  int & iz_min, double& min_s)
{
  vbl_array_3d<double> minima = vbl_local_minima(in, min_thresh);
  bool found_minima = minima.get_row1_count() > 0;
  // find minimum with lowest score
  min_s = vnl_numeric_traits<double>::maxval;
  double global_min_s = vnl_numeric_traits<double>::maxval;
  for (unsigned int iz = 0; iz<minima.get_row1_count(); ++iz)
    for (unsigned int iy = 0; iy<minima.get_row2_count(); ++iy)
      for (unsigned int ix = 0; ix<minima.get_row3_count(); ++ix) {
        double s = in[iz][iy][ix];
        if (s<global_min_s)
          global_min_s = s;
        if (minima[iz][iy][ix]>0) {
#if 0
          std::cout << "min(" << ix << ' ' << iy << ' ' << iz << ")= " << s << '\n';
#endif
          if (s<min_s) min_s = s;
          ix_min = ix; iy_min = iy; iz_min = iz;
        }
      }
  if (found_minima) {
    if (min_s>global_min_s)
      std::cout << "Warning! local minimum not global minimum "
               << min_s << " > " << global_min_s << '\n';
  }
  else {
    std::cout << " No local minimum found\n";
  }
  return found_minima;
}

//: Constructor
icam_minimizer::icam_minimizer( const vil_image_view<float>& source_img,
                                const vil_image_view<float>& dest_img,
                                const icam_depth_transform& dt,
                                icam_minimizer_params const& params,
                                bool verbose)
  : params_(params), cam_search_valid_(false), end_error_(0.0), verbose_(verbose)
{
  unsigned n_levels =
    icam_depth_trans_pyramid::required_levels(dest_img.ni(), dest_img.nj(),
                                              params_.min_level_size_);

  vil_image_view_base_sptr source_sptr =
    new vil_image_view<float>(source_img);
  source_pyramid_ = vil_pyramid_image_view<float>(source_sptr,n_levels);
  vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(dest_img);
  dest_pyramid_=vil_pyramid_image_view<float>(dest_sptr,n_levels);
  dt_pyramid_ = icam_depth_trans_pyramid(const_cast<icam_depth_transform&>(dt),n_levels);
}

icam_minimizer::icam_minimizer(const vil_image_view<float>& dest_img,
                               const icam_depth_transform& dt,
                               icam_minimizer_params const& params,
                               bool verbose)
 : params_(params), cam_search_valid_(false), end_error_(0.0), verbose_(verbose)
{
  unsigned n_levels =
    icam_depth_trans_pyramid::required_levels(dest_img.ni(), dest_img.nj(),
                                              params_.min_level_size_);
  vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(dest_img);
  dest_pyramid_=vil_pyramid_image_view<float>(dest_sptr,n_levels);
  dt_pyramid_ =
    icam_depth_trans_pyramid(const_cast<icam_depth_transform&>(dt),n_levels);
}

void icam_minimizer::print_params()
{
  params_.print();
}

void icam_minimizer::set_source_img(const vil_image_view<float>& source_img)
{
  vil_image_view_base_sptr source_sptr = new vil_image_view<float>(source_img);
  source_pyramid_ =
    vil_pyramid_image_view<float>(source_sptr,dest_pyramid_.nlevels());
}

icam_cost_func icam_minimizer::cost_fn(unsigned level)
{
  vil_image_view<float>& source = source_pyramid_(level);
  vil_image_view<float>& dest = dest_pyramid_(level);
  vil_image_view<float> source_sm(source.ni(), source.nj());
  vil_image_view<float> dest_sm(dest.ni(), dest.nj());
#if 0
  vil_gauss_filter_5tap(source,source_sm,
                        vil_gauss_filter_5tap_params(params_.smooth_sigma_));
  vil_gauss_filter_5tap(dest,dest_sm,
                        vil_gauss_filter_5tap_params(params_.smooth_sigma_));

  return icam_cost_func(source_sm,
                        dest_sm,dt_pyramid_.depth_trans(level,
                                                        true, params_.smooth_sigma_),
                        params_.nbins_);
#else
  return icam_cost_func(source, dest,dt_pyramid_.depth_trans(level),
                        params_.nbins_);
#endif
}

//: The main function.
void
icam_minimizer:: minimize(vgl_rotation_3d<double>& rot,
                          vgl_vector_3d<double>& trans,
                          double to_fl)
{
  dt_pyramid_.set_to_fl(to_fl);
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  vnl_vector<double> params, fx;
  for (int L=source_pyramid_.nlevels()-1; L>=0; --L)
  {
#if 0
    vil_image_view<float>& source = source_pyramid_(L);
    vil_image_view<float>& dest = dest_pyramid_(L);
    vil_image_view<float> source_sm(source.ni(), source.nj());
    vil_image_view<float> dest_sm(dest.ni(), dest.nj());

    vil_gauss_filter_5tap(source,source_sm,vil_gauss_filter_5tap_params(2));
    vil_gauss_filter_5tap(dest,dest_sm,vil_gauss_filter_5tap_params(2));
    icam_cost_func cost(source_sm, dest_sm, dt_pyramid_.depth_trans(L),
                        params_.nbins_);
#endif
    // no masks
    icam_cost_func cost = cost_fn(L);
    vnl_levenberg_marquardt minimizer(cost);
    //minimizer.set_x_tolerance(1e-16);
    //minimizer.set_f_tolerance(1.0);
    //minimizer.set_g_tolerance(1e-3);
    minimizer.set_trace(true);
    //minimizer.set_max_iterations(50);
    params = dt_pyramid_.params();
    minimizer.minimize(params);
    end_error_ = minimizer.get_end_error();
    dt_pyramid_.set_params(params);
  }
}

// minimize rotation only using the Powell algorithm. The translation
// parameters are assumed to be correct
void icam_minimizer::minimize_rot(vgl_rotation_3d<double>& rot,
                                  vgl_vector_3d<double> const& trans,
                                  unsigned level,
                                  double min_allowed_overlap)
{
  //Set the initial rotation on the dt pyramid
  dt_pyramid_.set_rotation(rot);
  //Set the translation. Not varied during minimization
  dt_pyramid_.set_translation(trans);
  icam_cost_func cost_func = cost_fn(level);
  icam_scalar_cost_func scal_cost_func(cost_func);
  scal_cost_func.set_min_overlap(min_allowed_overlap);
  vnl_powell powell(&scal_cost_func);
  powell.set_trace(false);
  powell.set_verbose(false);
  //set the initial parameters for Powell
  vnl_vector<double> x = rot.as_rodrigues();
  vnl_nonlinear_minimizer::ReturnCodes code = powell.minimize(x);
  if (!(code>0 && code<5)) {
    std::cout << "rotation minimization failed code = " << code << '\n';
    return;
  }
  //the params are the Rodrigues vector corresponding to the rotation
  rot = vgl_rotation_3d<double>(x);
  end_error_ = powell.get_end_error();
}

principal_ray_scan icam_minimizer::pray_scan(unsigned level, unsigned& n_pts)
{
  vpgl_perspective_camera<double> dcam = this->source_cam(level);
  double pixel_cone_ang, pixel_solid_ang;
  vsph_camera_bounds::pixel_solid_angle(dcam, pixel_cone_ang, pixel_solid_ang);
  double image_cone_ang, image_solid_ang;
  vsph_camera_bounds::image_solid_angle(dcam, image_cone_ang, image_solid_ang);
  n_pts = static_cast<unsigned>(image_solid_ang/pixel_solid_ang);
  return principal_ray_scan(image_cone_ang, n_pts);
}

double icam_minimizer::polar_inc(unsigned level, unsigned& nsteps,
                                 double polar_range)
{
  vpgl_perspective_camera<double> dcam = this->source_cam(level);
  double polar_inc = vsph_camera_bounds::rotation_angle_interval(dcam);
  auto nangle_steps = static_cast<unsigned>(2.0*polar_range/polar_inc );
  // nangle_steps must be even to include ang = 0.0
  if (nangle_steps%2) nangle_steps++;
  // need to include zero polar rotation in the scan so revise polar inc
  polar_inc = 2.0*polar_range/(static_cast<double>(nangle_steps));
  nsteps = nangle_steps+1;// closed interval, e.g. -pi<=x<=pi
  return polar_inc;
}

bool icam_minimizer::
exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                           unsigned level,
                           double min_allowed_overlap,
                           vgl_rotation_3d<double>& min_rot,
                           double& min_cost,
                           double& min_overlap_fraction,
                           bool  /*setup*/,
                           bool finish)
{
  // setup, finish flags used only for GPU implementation
  if (finish) return true;
  unsigned n_rays, npsteps;
  principal_ray_scan prs = this->pray_scan(level, n_rays);
  double polar_range = vnl_math::pi;
  double plar_inc = this->polar_inc(level, npsteps, polar_range);
#if 0
  std::cout << "Searching over "
           << static_cast<unsigned>(n_rays*npsteps)
           << " rotations\n" << std::flush;*/
#endif
  vnl_vector_fixed<double,3> min_rod;
  icam_cost_func cost = this->cost_fn(level);
  vul_timer tim;
  unsigned n_succ = 0;
  min_overlap_fraction = 0.0;
  min_cost = vnl_numeric_traits<double>::maxval;
  for (prs.reset(); prs.next();) {
    for (double ang = -polar_range; ang<=polar_range; ang+=plar_inc)
    {
      vgl_rotation_3d<double> rot = prs.rot(ang);
      vnl_vector_fixed<double, 3> rod = rot.as_rodrigues();
      //double c = cost.error(rod, trans,min_allowed_overlap);
      //double c = cost.entropy(rod, trans,min_allowed_overlap);
      //double c = cost.mutual_info(rod, trans,min_allowed_overlap);
      double c = cost.entropy_diff(rod, trans,min_allowed_overlap);
      if (c==vnl_numeric_traits<double>::maxval)
        continue;
      //c = -c;
      if (c<min_cost) {
        min_cost = c;
        min_rod = rod;
        min_overlap_fraction = cost.frac_samples();
        n_succ++;
      }
    }
  }
  if (verbose_)
    std::cout << "scan took " << tim.real()/1000.0 << " seconds" << std::endl;
  if (n_succ==0) return false;
  min_rot = vgl_rotation_3d<double>(min_rod);
  return true;
}

principal_ray_scan icam_minimizer::
initialized_pray_scan(unsigned initial_level, unsigned search_level,
                      unsigned& n_pts)
{
  vpgl_perspective_camera<double> idcam =
    this->source_cam(initial_level);
  vpgl_perspective_camera<double> sdcam =
    this->source_cam(search_level);
  double initial_pixel_cone_ang, initial_pixel_solid_ang;
  vsph_camera_bounds::
    pixel_solid_angle(idcam, initial_pixel_cone_ang,
                      initial_pixel_solid_ang);
  double search_pixel_cone_ang, search_pixel_solid_ang;
  vsph_camera_bounds::
    pixel_solid_angle(sdcam, search_pixel_cone_ang,
                      search_pixel_solid_ang);

  double search_space_cone_ang = params_.axis_search_cone_multiplier_*initial_pixel_cone_ang;

  double search_space_solid_ang =
    vsph_camera_bounds::solid_angle(search_space_cone_ang);
  double ratio = search_space_solid_ang/search_pixel_solid_ang;
  n_pts = static_cast<unsigned>(ratio);
  return principal_ray_scan(search_space_cone_ang, n_pts);
}

void icam_minimizer::initialized_polar_inc(unsigned initial_level,
                                           unsigned search_level,
                                           unsigned& nsteps,
                                           double& polar_range,
                                           double& polar_inc)
{
  vpgl_perspective_camera<double> idcam =
    this->source_cam(initial_level);
  vpgl_perspective_camera<double> sdcam =
    this->source_cam(search_level);

  double initial_polar_inc = vsph_camera_bounds::rotation_angle_interval(idcam);
  polar_inc =
    vsph_camera_bounds::rotation_angle_interval(sdcam);
  polar_range = params_.polar_range_multiplier_*initial_polar_inc;
  nsteps = static_cast<unsigned>(polar_range/polar_inc);
  // nsteps must be even to include ang = 0.0
  if (nsteps%2) nsteps++;
  polar_inc = polar_range/(static_cast<double>(nsteps));
}

bool icam_minimizer::rot_search(vgl_vector_3d<double> const& trans,
                                vgl_rotation_3d<double>& initial_rot,
                                unsigned n_axis_steps,
                                double axis_cone_half_angle,
                                unsigned n_polar_steps,
                                double polar_range,
                                unsigned search_level,
                                double min_allowed_overlap,
                                vgl_rotation_3d<double>& min_rot,
                                double& min_cost,
                                double& min_overlap_fraction)
{
  double polar_inc = 1.0;
  if (n_polar_steps)
    polar_inc = polar_range/n_polar_steps;

  unsigned n_samples = n_axis_steps;
  principal_ray_scan prs(axis_cone_half_angle, n_samples);

#if 1
  unsigned np = n_polar_steps;
  if (!np) np = 1;
  if (verbose_)
    std::cout << "Searching over "
             << static_cast<unsigned>(n_samples*np)
             << " rotations\n" << std::flush;
#endif
  unsigned n_succ = 0;
  min_overlap_fraction = 0.0;
  min_cost = vnl_numeric_traits<double>::maxval;
  icam_cost_func cost = this->cost_fn(search_level);
  vnl_vector_fixed<double,3> min_rod;
  vul_timer tim;
  unsigned nc = 0;
  for (prs.reset(); prs.next();) {
    for (double ang = -(polar_range/2); ang<=(polar_range/2); ang+=polar_inc)
    {
      vgl_rotation_3d<double> rot = prs.rot(ang);
      // pre or post multiply? Or something else?
      vgl_rotation_3d<double> comp_rot = initial_rot*rot;
      vnl_vector_fixed<double, 3> rod = comp_rot.as_rodrigues();
      //double c = cost.error(rod, trans,min_allowed_overlap);
      //double c = cost.entropy(rod, trans,min_allowed_overlap);
      //double c = cost.mutual_info(rod, trans,min_allowed_overlap);
      double c = cost.entropy_diff(rod, trans,min_allowed_overlap);
      if ((nc++)%10 == 0) std::cout << '.';
      if (c==vnl_numeric_traits<double>::maxval)
        continue;
      //c = -c;
      if (c<min_cost) {
        min_cost = c;
        min_rod = rod;
        min_overlap_fraction = cost.frac_samples();
        n_succ++;
      }
    }
  }
  std::cout << "\nscan took " << tim.real()/1000.0 << " seconds\n" << std::flush;

  if (n_succ==0) return false;
  min_rot = vgl_rotation_3d<double>(min_rod);
  return true;
}

bool icam_minimizer::
initialized_rot_search(vgl_vector_3d<double> const& trans,
                       vgl_rotation_3d<double>& initial_rot,
                       unsigned initial_level,
                       unsigned search_level,
                       double min_allowed_overlap,
                       vgl_rotation_3d<double>& min_rot,
                       double& min_cost,
                       double& min_overlap_fraction,
                       bool  /*setup*/,
                       bool finish)
{
  // setup, finish flags used only for GPU implementation
  if (finish) return true;

  double polar_inc, polar_range;
  unsigned naxis_steps, npolar_steps;
  principal_ray_scan prs =
    this->initialized_pray_scan(initial_level, search_level, naxis_steps);
  this->initialized_polar_inc(initial_level, search_level,
                              npolar_steps, polar_range, polar_inc);
#if 0
  if (verbose_)
    std::cout << "Searching over "
             << static_cast<unsigned>(naxis_steps*npolar_steps)
             << " rotations\n" << std::flush;*/
#endif
  unsigned n_succ = 0;
  min_overlap_fraction = 0.0;
  min_cost = vnl_numeric_traits<double>::maxval;
  icam_cost_func cost = this->cost_fn(search_level);
  vnl_vector_fixed<double,3> min_rod;
  vul_timer tim;
  for (prs.reset(); prs.next();) {
    for (double ang = -(polar_range/2); ang<=(polar_range/2); ang+=polar_inc)
    {
      vgl_rotation_3d<double> rot = prs.rot(ang);
      // pre or post multiply? Or something else?
      vgl_rotation_3d<double> comp_rot = initial_rot*rot;
      vnl_vector_fixed<double, 3> rod = comp_rot.as_rodrigues();
      //double c = cost.error(rod, trans,min_allowed_overlap);
      //double c = cost.entropy(rod, trans,min_allowed_overlap);
      //double c = cost.mutual_info(rod, trans,min_allowed_overlap);
      double c = cost.entropy_diff(rod, trans,min_allowed_overlap);
      if (c==vnl_numeric_traits<double>::maxval)
        continue;
      //c = -c;
      if (c<min_cost) {
        min_cost = c;
        min_rod = rod;
        min_overlap_fraction = cost.frac_samples();
        n_succ++;
      }
    }
  }
  std::cout << "scan took " << tim.real()/1000.0 << " seconds\n" << std::flush;

  if (n_succ==0) return false;
  min_rot = vgl_rotation_3d<double>(min_rod);
  return true;
}

void icam_minimizer::
set_origin_step_delta(vgl_box_3d<double> const& trans_box,
                      vgl_vector_3d<double> const& trans_steps)
{
  double xspan = trans_box.max_x() - trans_box.min_x();
  double yspan = trans_box.max_y() - trans_box.min_y();
  double zspan = trans_box.max_z() - trans_box.min_z();
  auto nx = static_cast<unsigned>(xspan/trans_steps.x());
  auto ny = static_cast<unsigned>(yspan/trans_steps.y());
  auto nz = static_cast<unsigned>(zspan/trans_steps.z());
  //need to include (0 0 0);
  if (nx%2) ++nx;  if (ny%2) ++ny;  if (nz%2) ++nz;
  double dx = xspan/nx, dy = yspan/ny, dz = zspan/nz;
  box_origin_.set(trans_box.min_x(), trans_box.min_y(),trans_box.min_z());
  //cached items for subsequent processing
  step_delta_.set(dx, dy, dz);
  box_scores_ = vbl_array_3d<double>(nz+1, ny+1, nx+1);
  box_rotations_ = vbl_array_3d<vgl_rotation_3d<double> >(nz+1, ny+1, nx+1);
}

bool icam_minimizer::refine_minimum(int mx, int my, int mz,
                                    unsigned /*level*/,              // FIXME: unused
                                    double /* min_allowed_overlap*/, // FIXME: unused
                                    vgl_vector_3d<double>& min_trans,
                                    double& /*min_cost*/)            // FIXME: unused
{
  int nz = box_scores_.get_row1_count(),ny = box_scores_.get_row2_count(),
    nx = box_scores_.get_row3_count();
  double mins = box_scores_[mz][my][mx];
  double min_diff = vnl_numeric_traits<double>::maxval;
  bool valid = false;
  int inx = -1, iny = -1 , inz = -1;
  //find closest neighbor
  for (int kz = -1; kz<=1; ++kz) {
    int iz = mz+kz;
    if (iz<0||iz>=nz)
      continue;
    for (int ky = -1; ky<=1; ++ky) {
      int iy = my+ky;
      if (iy<0||iy>=ny)
        continue;
      for (int kx = -1; kx<=1; ++kx)
      {
        int ix = mx+kx;
        if (ix<0||ix>=nx)
          continue;
        if (kx==0&&ky==0&&kz==0)
          continue;
        double diff = std::fabs(box_scores_[iz][iy][ix]-mins);
        if (diff<min_diff)
        {
          min_diff = diff;
          inx = ix; iny = iy; inz = iz;
          valid = true;
        }
      }
    }
  }
  // refine the estimates for translation and rotation
  // average translation
  double xn = inx*step_delta_.x() + box_origin_.x();
  double yn = iny*step_delta_.y() + box_origin_.y();
  double zn = inz*step_delta_.z() + box_origin_.z();
  double xlmin = mx*step_delta_.x() + box_origin_.x();
  double ylmin = my*step_delta_.y() + box_origin_.y();
  double zlmin = mz*step_delta_.z() + box_origin_.z();
  double xr = 0.5*(xn+xlmin), yr = 0.5*(yn+ylmin), zr = 0.5*(zn+zlmin);
  std::cout << "Refined translation (" << xr << ' ' << yr << ' ' << zr << ")\n";
  min_trans.set(xr, yr, zr);
  return valid;
}

bool icam_minimizer::
exhaustive_camera_search(vgl_box_3d<double> const& trans_box,
                         vgl_vector_3d<double> const& trans_steps,
                         unsigned level,
                         double min_allowed_overlap,
                         vgl_vector_3d<double>& min_trans,
                         vgl_rotation_3d<double>& min_rot,
                         double& min_cost,
                         double& min_overlap_fraction)
{
  bool setup = true;
  bool finish = false;
  //sets cached member variables
  this->set_origin_step_delta(trans_box, trans_steps);
  unsigned nz = box_scores_.get_row1_count(),ny = box_scores_.get_row2_count(),
    nx = box_scores_.get_row3_count();
  double min_trans_cost = vnl_numeric_traits<double>::maxval;
  double min_trans_overlap = min_allowed_overlap; // avoids running with uninitialised value
  vgl_rotation_3d<double> rot;
  double overlap, cost;
  vnl_vector_fixed<double, 3> min_trans_rod;
  vgl_vector_3d<double> t, min_trans_trans;
  unsigned ix = 0, iy = 0, iz = 0;
  double x , y, z;
  for (x=box_origin_.x(), ix = 0; ix<nx; x+=step_delta_.x(), ++ix)
    for (y=box_origin_.y(), iy = 0; iy<ny; y+=step_delta_.y(), ++iy)
      for (z=box_origin_.z(), iz = 0; iz<nz; z+=step_delta_.z(), ++iz)
      {
        t.set(x, y, z);
        if (!exhaustive_rotation_search(t, level, min_allowed_overlap,
                                        rot, cost, overlap, setup, finish) )
          return false;
        setup = false;// only setup on the first call
        vnl_vector_fixed<double, 3> rotr = rot.as_rodrigues();
        if (cost<min_trans_cost) {
          min_trans_trans = t;
          min_trans_cost = cost;
          min_trans_rod = rotr;
          min_trans_overlap = overlap;
        }
        if (verbose_) {
        double t_dist = (t-actual_trans_).length();
        double r_dist = (rotr - (actual_rot_.as_rodrigues())).magnitude();
        std::cout << "t(" << x << ' ' << y << ' ' << z << ")["
                 << t_dist << ' ' << r_dist << "] = " << cost << '\n'
                 << "-------> R(" << rotr[0] << ' ' << rotr[1]
                 << ' ' << rotr[2] << ")\n" << std::flush;
        }
        box_scores_[iz][iy][ix] = cost;
        box_rotations_[iz][iy][ix]= rot;
      }
  //probably can't happen but just in case ...
  if (min_trans_overlap<min_allowed_overlap) return false;
  cam_search_valid_ = true;
  min_trans = min_trans_trans;
  min_rot = vgl_rotation_3d<double>(min_trans_rod);
  min_cost = min_trans_cost;
  min_overlap_fraction = min_trans_overlap;
  if (base_path_!="") {
    std::string path = base_path_ + "/box_top_lev.wrl";
    this->box_search_vrml(path, actual_trans_);
  }
  // call the virtual rotation search method to
  // delete buffers and clean up the context
  finish = true;
  exhaustive_rotation_search(t, level, min_allowed_overlap,
                             rot, cost, overlap, setup, finish);
  return true;
}

// The interval dx is defined by the initial box, initial step delta
// and k the half width of the search space, i.e. 2k + 1 steps with
// the initial translation, center_trans positioned at the k+1st cell
// |<--- k --->|
// [   |   |   | * |   |   |   ]
// |dx |
// |< --initial step delta --->|
void icam_minimizer::
reduce_search_box(vgl_vector_3d<double> const& center_trans,
                  vgl_vector_3d<double> const& initial_step_delta)
{
  double exf = 1.5;//delta expansion range
  //local minimum translation from previous search
  double xc = center_trans.x(),
         yc = center_trans.y(),
         zc = center_trans.z();
  unsigned m = 2*params_.box_reduction_k_;
  unsigned n = m+1;//total number of search steps
  double dx = exf*initial_step_delta.x()/m,
         dy = exf*initial_step_delta.y()/m, dz = exf*initial_step_delta.z()/m;
  double off = params_.box_reduction_k_;
  // center the input translation
  box_origin_.set(xc-off*dx, yc-off*dy, zc-off*dz);
  step_delta_.set(dx, dy, dz);
  // recompute the number of steps
  box_scores_ = vbl_array_3d<double>(n, n, n);
  box_rotations_ = vbl_array_3d<vgl_rotation_3d<double> >(n, n, n);
}

bool icam_minimizer::
pyramid_camera_search(vgl_vector_3d<double> const&
                       /*start_trans*/,
                      vgl_rotation_3d<double> const&
                      start_rotation,
                      vgl_vector_3d<double> const&
                      start_step_delta,
                      unsigned start_level,
                      unsigned final_level,
                      double min_allowed_overlap,
                      bool refine,
                      vgl_vector_3d<double>& min_trans,
                      vgl_rotation_3d<double>& min_rot,
                      double& min_cost,
                      double& min_overlap_fraction)
{
  double min_trans_overlap = min_allowed_overlap; // avoids running with uninitialised value
  vnl_vector_fixed<double, 3> min_trans_rod;
  vgl_vector_3d<double> t, min_trans_trans;
  double x , y, z;
  unsigned init_level = start_level;
  vgl_rotation_3d<double> init_rot = start_rotation;
  vgl_vector_3d<double> stepd = start_step_delta;
  for (unsigned lev = start_level-1; lev>=final_level; --lev)
  {
    if (verbose_) {
      print_axis_search_info(lev+1, actual_rot_, init_rot);
      print_polar_search_info(lev+1, actual_rot_, init_rot);
    }
    vgl_vector_3d<double> smallest_min_trans;
    vgl_rotation_3d<double> smallest_min_rot;
    vgl_rotation_3d<double> rot;
    double overlap, cost;
    //state variables for controlling gpu setup
    // start by setting up context, kernel and buffers
    bool setup = true, finish = false;
    //sets box_origin_ and step delta, resizes box_scores and
    // box rotations for more focused search
    this->reduce_search_box(min_trans, stepd);
    unsigned nz = box_scores_.get_row1_count();
    unsigned ny = box_scores_.get_row2_count();
    unsigned nx = box_scores_.get_row3_count();
    double min_trans_cost = vnl_numeric_traits<double>::maxval;
    unsigned ix = 0, iy = 0, iz = 0;
    for (x=box_origin_.x(),ix = 0; ix<nx; x+=step_delta_.x(), ++ix)
      for (y=box_origin_.y(), iy = 0; iy<ny; y+=step_delta_.y(), ++iy)
        for (z=box_origin_.z(),iz = 0; iz<nz; z+=step_delta_.z(), ++iz)
        {
          t.set(x, y, z);
          if (!initialized_rot_search(t, init_rot,
                                      init_level, lev,
                                      min_allowed_overlap,
                                      rot, cost, overlap, setup, finish))
            return false;
          setup = false; // only setup on the first call during box search
          vnl_vector_fixed<double, 3> rotr = rot.as_rodrigues();
          if (cost<min_trans_cost) {
            min_trans_trans = t;
            min_trans_cost = cost;
            min_trans_rod = rotr;
            min_trans_overlap = overlap;
          }
          if (verbose_) {
            double t_dist = (t-actual_trans_).length();
            double r_dist = (rotr - (actual_rot_.as_rodrigues())).magnitude();

            std::cout << "tp(" << x << ' ' << y << ' ' << z << ")["
                     << t_dist << ' ' << r_dist << "] = " << cost << '\n';
          }
          box_scores_[iz][iy][ix] = cost;
          box_rotations_[iz][iy][ix]= rot;
        }
    //probably can't happen but just in case ...
    if (min_trans_overlap<min_allowed_overlap) return false;
    std::cout << "completed pyramid level " << lev << '\n';
    cam_search_valid_ = true;
    double smallest_min;
#if 0
    if (lev == final_level) {
      std::cout << "Final box scores\n";
      for (ix = 0; ix<nx; ++ix)
        for (iy = 0; iy<ny; ++iy)
          for (iz = 0; iz<nz; ++iz)
            std::cout << "b[" << iz << "][" << iy << "][" << iz << "] = "
                     << box_scores_[iz][iy][ix] << ";\n";
    }
#endif

    int mx = -1, my = -1, mz = -1;
    if (this->smallest_local_minimum(params_.local_min_thresh_, smallest_min,
                                     smallest_min_trans,
                                     smallest_min_rot,
                                     mx, my, mz)) {
      std::cout << "smallest local min cost " << smallest_min
               << " at translation " << smallest_min_trans
               << "\nwith discrete rotation "
               << smallest_min_rot.as_rodrigues() << '\n';
    }
    else {
      std::cout << " no local minimum found in pyramid search\n";
      initialized_rot_search(t, init_rot, init_level, lev,
                             min_allowed_overlap, rot, cost,
                             overlap, setup, true);
      return false;
    }
    if (refine) {
        bool setup = false;
        bool finish = false;
      if (refine_minimum(mx, my, mz, lev, min_allowed_overlap,
                         smallest_min_trans,
                         smallest_min)) {
        if (!initialized_rot_search(smallest_min_trans, smallest_min_rot,
                                    init_level,
                                    lev,
                                    min_allowed_overlap,
                                    smallest_min_rot,
                                    smallest_min,
                                    min_trans_overlap,
                                    setup, finish)) {
          std::cout << "Rotation at level " << lev <<  " failed during refine\n";
          return false;
        }
        std::cout << "refined local min cost " << smallest_min
                 << " at refined translation " << smallest_min_trans
                 << "\nwith discrete rotation "
                 << smallest_min_rot.as_rodrigues() << '\n';
      }
      else {
        std::cout << " refinement failed at level " << lev << '\n';
        initialized_rot_search(t, init_rot, init_level, lev,
                               min_allowed_overlap, rot, cost,
                               overlap, setup, true);
        return false;
      }
    }
    min_trans = smallest_min_trans;
    min_rot = smallest_min_rot;
    min_cost = smallest_min;
    min_overlap_fraction = min_trans_overlap;
    if (!refine)
      std::cout << " minimum translation " << min_trans << '\n'
               << " minimum rotation " << min_trans_rod << '\n'
               << " minimum cost " << smallest_min << '\n';
    if (base_path_!="") {
      std::stringstream strm;
      strm << "/box_pyr_lev_"<< lev << ".wrl" << std::ends;
      std::string path = base_path_ + strm.str();
      this->box_search_vrml(path, actual_trans_);
    }
    init_level = lev;
    init_rot = min_rot;
    stepd = step_delta_;
    finish = true;
    // call the virtual rotation search method to
    // delete buffers and clean up the context
    if (!initialized_rot_search(t, init_rot, init_level, lev,
                               min_allowed_overlap, rot, cost,
                               overlap, setup, finish))
      return false;
  }
  return true;
}

bool icam_minimizer::
camera_search( vgl_box_3d<double> const& trans_box,
               vgl_vector_3d<double> const& trans_steps,
               unsigned final_level,
               double min_allowed_overlap,
               bool refine,
               vgl_vector_3d<double>& min_trans,
               vgl_rotation_3d<double>& min_rot,
               double& min_error,
               double& min_overlap)
{
  unsigned top_level = dt_pyramid_.n_levels()-1;
  if (verbose_) {
    print_axis_search_info(top_level, actual_rot_,
                           vgl_rotation_3d<double>(),true);
    print_polar_search_info(top_level, actual_rot_,
                            vgl_rotation_3d<double>(), true);
  }
  if (!this->exhaustive_camera_search(trans_box,
                                      trans_steps,
                                      top_level,
                                      min_allowed_overlap,
                                      min_trans,
                                      min_rot,
                                      min_error,
                                      min_overlap
                                      ))
    return false;
  double smallest_min;
  vgl_vector_3d<double> smallest_min_trans;
  vgl_rotation_3d<double> smallest_min_rot;
  int mx=-1, my=-1, mz=-1;
  if (this->smallest_local_minimum(params_.local_min_thresh_, smallest_min,
                                   smallest_min_trans,
                                   smallest_min_rot,
                                   mx, my, mz))
  {
    std::cout << "smallest local min cost " << smallest_min
             << " at translation " << smallest_min_trans
             << "\nwith discrete rotation "
             << smallest_min_rot.as_rodrigues() << '\n';
  }
  else {
    std::cout << " no local minimum found in top level search\n";
    return false;
  }
  if (refine) {
    bool setup = false;
    bool finish = false;
    if (refine_minimum(mx, my, mz, top_level, min_allowed_overlap,
                       smallest_min_trans,
                       smallest_min)) {
      if (!exhaustive_rotation_search(smallest_min_trans, top_level,
                                      min_allowed_overlap,
                                      smallest_min_rot,
                                      smallest_min, min_overlap,
                                      setup, finish)) {
        std::cout << "Rotation at top level failed during refine\n";
        return false;
      }
      std::cout << "refined local min cost " << smallest_min
               << " at refined translation " << smallest_min_trans
               << "\nwith discrete rotation "
               << smallest_min_rot.as_rodrigues() << '\n';
    }
    else {
      std::cout << " refinement failed\n";
      return false;
    }
  }

  if (!this-> pyramid_camera_search(smallest_min_trans,
                                    smallest_min_rot,
                                    trans_steps,
                                    top_level,
                                    final_level,
                                    min_allowed_overlap,
                                    refine,
                                    min_trans,
                                    min_rot,
                                    min_error,
                                    min_overlap))
    return false;
  else
    return true;
}

double icam_minimizer::error(vgl_rotation_3d<double>& rot,
                             vgl_vector_3d<double>& trans,
                             unsigned level)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  icam_cost_func cost = cost_fn(level);
  return cost.error(rot.as_rodrigues(), trans);
}


std::vector<double> icam_minimizer::error(vgl_rotation_3d<double>& rot,
                                         vgl_vector_3d<double>& trans,
                                         unsigned level,
                                         unsigned param_index, double pmin,
                                         double pmax, double pinc)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  icam_cost_func cost = cost_fn(level);
  vnl_vector<double> params = dt_pyramid_.params();
  return cost.error(params, param_index, pmin, pmax, pinc);
}

std::vector<vil_image_view<float> > icam_minimizer::views(vgl_rotation_3d<double>& rot,
                                                         vgl_vector_3d<double>& trans,
                                                         unsigned level,
                                                         unsigned param_index, double pmin,
                                                         double pmax, double pinc)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  vnl_vector<double> params = dt_pyramid_.params();
  std::vector<vil_image_view<float> > res;
  vil_image_view<float>& source = source_pyramid_(level);
  unsigned n_samples;
  for (double p = pmin; p<=pmax; p+=pinc)
  {
    params[param_index] = p;
    dt_pyramid_.set_params(params);
    icam_depth_transform dt = dt_pyramid_.depth_trans(level,true,params_.smooth_sigma_);
    vil_image_view<float> trans_view, mask;
    icam_sample::resample(source.ni(), source.nj(), source,
                          dt, trans_view, mask, n_samples);
    res.push_back(trans_view);
  }
  return res;
}

vil_image_view<float> icam_minimizer::view(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans,
                                           unsigned level)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  icam_depth_transform dt = dt_pyramid_.depth_trans(level, true, params_.smooth_sigma_);
  vil_image_view<float>& source = source_pyramid_(level);
  vil_image_view<float> trans_view, mask;
  unsigned n_samples;
  icam_sample::resample(source.ni(), source.nj(), source,
                        dt, trans_view, mask, n_samples);

  return trans_view;
}

vil_image_view<float> icam_minimizer::mask(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans,
                                           unsigned level)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  icam_depth_transform dt = dt_pyramid_.depth_trans(level, true, params_.smooth_sigma_);
  vil_image_view<float>& source = source_pyramid_(level);
  vil_image_view<float> trans_view, mask;
  unsigned n_samples;
  icam_sample::resample(source.ni(), source.nj(), source,
                        dt, trans_view, mask, n_samples);
  return mask;
}

vpgl_perspective_camera<double> icam_minimizer::source_cam(unsigned level)
{
  vgl_rotation_3d<double>& rot = dt_pyramid_.rotation();
  vgl_vector_3d<double>& trans = dt_pyramid_.translation();
  vnl_matrix_fixed<double, 3, 3> Km = dt_pyramid_.calibration_matrix(level);
  vpgl_calibration_matrix<double> K(Km);
  vpgl_perspective_camera<double> cam(K, rot, trans);
  return cam;
}

static void write_vrml_sphere(std::ofstream& str,
                              vgl_point_3d<double> const& center,
                              double radius,
                              const double r, const double g, const double  b,
                              const double transparency)
{
  double x0 = center.x(), y0 = center.y(), z0 = center.z();
  str << "Transform {\n"
      << "translation " << x0 << ' ' << y0 << ' '
      << ' ' << z0 << '\n'
      << "children [\n"
      << "Shape {\n"
      << " appearance Appearance{\n"
      << "   material Material\n"
      << "    {\n"
      << "      diffuseColor " << r << ' ' << g << ' ' << b << '\n'
      << "      transparency " << transparency << '\n'
      << "    }\n"
      << "  }\n"
      << " geometry Sphere\n"
      <<   "{\n"
      << "  radius " << radius << '\n'
      <<  "   }\n"
      <<  "  }\n"
      <<  " ]\n"
      << "}\n";
}

bool icam_minimizer::box_search_vrml(std::string const& vrml_file,
                                     vgl_vector_3d<double> const& trans)
{
  if (!cam_search_valid_) {
    std::cout << " No box search has been performed\n";
    return false;
  }
  std::ofstream str(vrml_file.c_str());
  if (!str.is_open()) {
    std::cout << " Can't open vrml output file " << vrml_file << '\n';
    return false;
  }
  str << "#VRML V2.0 utf8\n"
      << "Background {\n"
      << "  skyColor [ 0 0 0 ]\n"
      << "  groundColor [ 0 0 0 ]\n"
      << "}\n"
      << "PointLight {\n"
      << "  on FALSE\n"
      << "  intensity 1\n"
      << "ambientIntensity 0\n"
      << "color 1 1 1\n"
      << "location 0 0 0\n"
      << "attenuation 1 0 0\n"
      << "radius 100\n"
      << "}\n";
  double max_score = -vnl_numeric_traits<double>::maxval;
  double min_score = vnl_numeric_traits<double>::maxval;
  vbl_array_3d<double>::const_iterator bit = box_scores_.begin();
  for (; bit<box_scores_.end(); ++bit)
  {
    double scr = *bit;
    if (scr>max_score) max_score = scr;
    if (scr<min_score) min_score = scr;
  }
  double radius = step_delta_.x()/5.0;
  double ratio = 1.0f;
  bool varies = true;
  if (max_score>min_score)
    ratio = 1.0f/static_cast<double>((max_score-min_score));
  else varies = false;
  unsigned nz = box_scores_.get_row1_count(),ny = box_scores_.get_row2_count(),
    nx = box_scores_.get_row3_count();
  double ox = box_origin_.x();
  double oy = box_origin_.y();
  double oz = box_origin_.z();
  double dx = step_delta_.x();
  double dy = step_delta_.y();
  double dz = step_delta_.z();
  unsigned ix = 0, iy = 0, iz = 0;
  double x, y, z;
  for (ix = 0, x = ox; ix<nx; ++ix, x+=dx)
    for (iy = 0, y = oy; iy<ny; ++iy, y+=dy)
      for (iz = 0, z = oz; iz<nz; ++iz, z+=dz)
      {
        double s = box_scores_[iz][iy][ix];
        if (varies)
          s = (s-min_score)*ratio;
        else
          s = 0.5;
        // map s using a more rapid scale, e.g. 0.05
        double den = 1.0 + std::exp(-0.5*s/params_.local_min_thresh_);
        double ss = 2.0/den -1.0;
        vgl_point_3d<double> pt(x, y, z);
        double r = radius*(0.1 + 0.9*ss);
#if 0
        std::cout << " writing(x y z)["<< ix << ' ' << iy << ' ' << iz << "]("
                 << x << ' ' << y << ' ' << z << ") with score( "
                 << ss << "): " << s << '\n';
#endif
        write_vrml_sphere(str, pt, r, 1-ss, ss, 0.2f, 0.0f);
      }
  vgl_point_3d<double> act(trans.x(), trans.y(), trans.z());
  write_vrml_sphere(str, act, radius, 1.0, 0.0, 1.0, 0.0f);
  double min_cost;
  int ix_min = 0, iy_min =0, iz_min = 0;
  bool found = smallest_local_minima(box_scores_, params_.local_min_thresh_,
                                     ix_min, iy_min, iz_min, min_cost);
  if (found) {
    // convert to translation
    double mx = box_origin_.x() + ix_min*step_delta_.x();
    double my = box_origin_.y() + iy_min*step_delta_.y();
    double mz = box_origin_.z() + iz_min*step_delta_.z();
    vgl_point_3d<double> p(mx+radius, my+radius, mz+radius);
    write_vrml_sphere(str, p, radius/5.0, 1.0, 1.0, 1.0, 0.0f);
  }
  str.close();
  return true;
}

bool icam_minimizer::smallest_local_minimum(double nbhd_cost_threshold,
                                            double& min_cost,
                                            vgl_vector_3d<double>& min_trans,
                                            vgl_rotation_3d<double>& min_rot,
                                            int& ix_min, int& iy_min,
                                            int& iz_min)
{
  if (!cam_search_valid_) {
    std::cout << " No box search has been performed\n";
    return false;
  }
  ix_min = 0; iy_min =0; iz_min = 0;
  bool found = smallest_local_minima(box_scores_, nbhd_cost_threshold,
                                     ix_min, iy_min, iz_min, min_cost);
  if (!found)
    return false;
  // convert to translation
  double x = box_origin_.x() + ix_min*step_delta_.x();
  double y = box_origin_.y() + iy_min*step_delta_.y();
  double z = box_origin_.z() + iz_min*step_delta_.z();
  min_trans.set(x, y, z);
  min_rot = box_rotations_[iz_min][iy_min][ix_min];
  return found;
}

vnl_matrix_fixed<double, 3, 3> icam_minimizer::
to_calibration_matrix(unsigned level)
{
  return dt_pyramid_.to_calibration_matrix(level);
}

vnl_matrix_fixed<double, 3, 3> icam_minimizer::
from_calibration_matrix_inv(unsigned level)
{
  vnl_matrix_fixed<double, 3, 3> K_from =
    dt_pyramid_.from_calibration_matrix(level);
  return vnl_inverse(K_from);
}

vil_image_view<double> icam_minimizer::inv_depth(unsigned level)
{
  vil_image_view<double> depth = this->depth(level);
  unsigned ni = depth.ni(), nj = depth.nj();
  vil_image_view<double> inv_depth(ni, nj);
  float mval = vnl_numeric_traits<float>::maxval;
  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i) {
      auto z = static_cast<float>(depth(i,j));
      if (z<1.0e-6f) {
        inv_depth(i,j) = mval;
        continue;
      }
      inv_depth(i,j) = 1.0f/z;
    }
  return inv_depth;
}

void icam_minimizer::print_axis_search_info(unsigned level,
                                            vgl_rotation_3d<double> const& actual,
                                            vgl_rotation_3d<double> const& init,
                                            bool top_level)
{
  std::cout << "Axis search info-< ";
  vpgl_perspective_camera<double> dcam = this->source_cam(level);
  double image_cone_ang, image_solid_ang;
  vsph_camera_bounds::image_solid_angle(dcam, image_cone_ang, image_solid_ang);
  double pixel_cone_ang, pixel_solid_ang;
  vsph_camera_bounds::
    pixel_solid_angle(dcam, pixel_cone_ang,
                      pixel_solid_ang);
  double search_cone_ang = image_cone_ang;
  if (!top_level)
    search_cone_ang = pixel_cone_ang*params_.axis_search_cone_multiplier_;
  double act_ang = vpgl_ray::angle_between_rays(init, actual);
  std::cout << " axis cone search space angle " << search_cone_ang
           << " angle between actual and initial axes " << act_ang << " >\n";
}

void icam_minimizer::print_polar_search_info(unsigned level, vgl_rotation_3d<double> const&  /*actual*/,
                                             vgl_rotation_3d<double> const& init, bool top_level)
{
  std::cout << "Polar search info -< ";
  vpgl_perspective_camera<double> idcam =
    this->source_cam(level);
  double polar_inc = vsph_camera_bounds::rotation_angle_interval(idcam);
  double polar_range = params_.polar_range_multiplier_*polar_inc/2.0;
  // if top level
  if (top_level)
    polar_range = vnl_math::pi;
  double polar_needed = vpgl_ray::rot_about_ray(init, actual_rot_);
  std::cout << "polar range " << polar_range << " polar rotation needed " << polar_needed << " >\n";
}
