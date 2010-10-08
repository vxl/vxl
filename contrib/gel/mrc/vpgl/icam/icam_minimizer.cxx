// This is gel/mrc/vpgl/icam/icam_minimizer.cxx
//:
// \file

#include "icam_minimizer.h"

#include <vil/vil_convert.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_vector_fixed.h>
#include <vil/vil_math.h>
#include <vcl_cstdlib.h>
#include <vul/vul_timer.h>
#include <icam/icam_depth_trans_pyramid.h>
#include <icam/icam_sample.h>
#include <vpgl/algo/vpgl_camera_bounds.h>

//: Constructor
icam_minimizer::icam_minimizer( const vil_image_view<float>& source_img,
                                const vil_image_view<float>& dest_img,
                                const icam_depth_transform& dt,
                                unsigned min_level_size)
  : end_error_(0.0), min_level_size_(min_level_size)
{
  unsigned n_levels = 
    icam_depth_trans_pyramid::required_levels(dest_img.ni(), dest_img.nj(),
                                              min_level_size);

  vil_image_view_base_sptr source_sptr = 
    new vil_image_view<float>(source_img);
  source_pyramid_ = vil_pyramid_image_view<float>(source_sptr,n_levels);
  vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(dest_img);
  dest_pyramid_=vil_pyramid_image_view<float>(dest_sptr,n_levels);
  dt_pyramid_ = icam_depth_trans_pyramid(const_cast<icam_depth_transform&>(dt),n_levels);
}

//: Constructor
icam_minimizer::icam_minimizer( const vil_image_view<float>& dest_img,
                                const icam_depth_transform& dt,
                                unsigned min_level_size)
  : end_error_(0.0), min_level_size_(min_level_size)
{
  unsigned n_levels = 
    icam_depth_trans_pyramid::required_levels(dest_img.ni(), dest_img.nj(),
                                              min_level_size);

  vil_image_view_base_sptr dest_sptr = new vil_image_view<float>(dest_img);
  dest_pyramid_=vil_pyramid_image_view<float>(dest_sptr,n_levels);
  dt_pyramid_ = icam_depth_trans_pyramid(const_cast<icam_depth_transform&>(dt),n_levels);
}

void icam_minimizer::set_source_img(const vil_image_view<float>& source_img)
{
  vil_image_view_base_sptr source_sptr = new vil_image_view<float>(source_img);
  source_pyramid_ = vil_pyramid_image_view<float>(source_sptr,dest_pyramid_.nlevels());
}

icam_cost_func icam_minimizer::cost_fn(unsigned level)
{
      vil_image_view<float>& source = source_pyramid_(level);
      vil_image_view<float>& dest = dest_pyramid_(level);
      vil_image_view<float> source_sm(source.ni(), source.nj());
      vil_image_view<float> dest_sm(dest.ni(), dest.nj());

      vil_gauss_filter_5tap(source,source_sm,vil_gauss_filter_5tap_params(2));
      vil_gauss_filter_5tap(dest,dest_sm,vil_gauss_filter_5tap_params(2));
      return icam_cost_func(source_sm, dest_sm,dt_pyramid_.depth_trans(level));
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
  int n_levels = source_pyramid_.nlevels();
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
      icam_cost_func cost(source_sm, dest_sm, 
                          dt_pyramid_.depth_trans(L));
#endif
      // no masks
      vnl_levenberg_marquardt minimizer(cost_fn(L));
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
bool icam_minimizer::
exhaustive_rotation_search(vgl_vector_3d<double> const& trans,
                                  unsigned level,
                                  double min_allowed_overlap,
                                  vgl_rotation_3d<double>& min_rot,
                                  double& min_cost,
                                  double& min_overlap_fraction)
{
  vpgl_perspective_camera<double> dcam = this->dest_cam(level);
  double pixel_cone_ang, pixel_solid_ang;
  vpgl_camera_bounds::pixel_solid_angle(dcam, pixel_cone_ang, pixel_solid_ang);
  double image_cone_ang, image_solid_ang;
  vpgl_camera_bounds::image_solid_angle(dcam, image_cone_ang, image_solid_ang);
  unsigned npts = static_cast<unsigned>(image_solid_ang/pixel_solid_ang);
  double polar_inc = vpgl_camera_bounds::rotation_angle_interval(dcam);
  unsigned nangle_steps = static_cast<unsigned>(2.0*vnl_math::pi/polar_inc );
  vcl_cout << "Searching over " 
	       << static_cast<unsigned>(npts*nangle_steps) << " rotations\n" 
           << vcl_flush;
  double polar_range = vnl_math::pi;
  vnl_vector_fixed<double,3> min_rod;
  icam_cost_func cost = this->cost_fn(level);
  vul_timer tim;
  principal_ray_scan prs(image_cone_ang, npts); 
  unsigned n_succ = 0;
  min_overlap_fraction = 0.0;
  min_cost = vnl_numeric_traits<double>::maxval;
  for(prs.reset(); prs.next();){
    for(double ang = -polar_range; ang<=polar_range; ang+=polar_inc)
      {
        vgl_rotation_3d<double> rot = prs.rot(ang);
        vnl_vector_fixed<double, 3> rod = rot.as_rodrigues();
        double c = cost.error(rod, trans,min_allowed_overlap);
        if(c==vnl_numeric_traits<double>::maxval)
          continue;
        if(c<min_cost){
        min_cost = c;
        min_rod = rod;
        min_overlap_fraction = cost.frac_samples();
        n_succ++;
        }
      }
  }
  vcl_cout << "scan took " << tim.real()/1000.0 << " seconds\n" << vcl_flush;

  if(n_succ==0) return false;
  min_rot = vgl_rotation_3d<double>(min_rod);
  return true;
}
static vnl_vector_fixed<double, 3> min_rotg(0.0333366 -0.0459954 -0.127258);
static vgl_vector_3d<double> min_trang(0.320743,0.0423136,-0.0199299);
bool icam_minimizer::
exhaustive_camera_search(vgl_box_3d<double> const& trans_box,
                         vgl_vector_3d<double> const& trans_steps,
                         unsigned level,
                         double min_allowed_overlap,
                         vgl_vector_3d<double>& min_trans,
                         vgl_rotation_3d<double>& min_rot,
                         double& min_cost,
                         double& min_overlap_fraction
                         )
{
  double min_trans_cost = vnl_numeric_traits<double>::maxval;
  double min_trans_overlap;
  vnl_vector_fixed<double, 3> min_trans_rod;
  vgl_vector_3d<double> t, min_trans_trans;
  for(double x=trans_box.min_x(); x<=trans_box.max_x(); x+=trans_steps.x())
    for(double y=trans_box.min_y(); y<=trans_box.max_y(); y+=trans_steps.y())
      for(double z=trans_box.min_z(); z<=trans_box.max_z(); z+=trans_steps.z())
        {
          t.set(x, y, z);
          vgl_rotation_3d<double> rot;
          double overlap, cost;
          if(!exhaustive_rotation_search(t, level, min_allowed_overlap,
                                         rot, cost, overlap) )
            return false;
          vnl_vector_fixed<double, 3> rotr = rot.as_rodrigues();
          if(cost<min_trans_cost){
            min_trans_trans = t;
            min_trans_cost = cost;
            min_trans_rod = rotr;
            min_trans_overlap = overlap;
          }
          double t_dist = (t-min_trang).length();
          double r_dist = (rotr - min_rotg).magnitude();
          vcl_cout << "t(" << x << ' ' << y << ' ' << z << ")["
                   << t_dist << ' ' << r_dist << "] = " << cost << '\n';
        }
  //probably can't happen but just in case ...
  if(min_trans_overlap<min_allowed_overlap) return false;
  
  min_trans = min_trans_trans;
  min_rot = vgl_rotation_3d<double>(min_trans_rod);
  min_cost = min_trans_cost;
  min_overlap_fraction = min_trans_overlap;
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
  

vcl_vector<double> icam_minimizer::error(vgl_rotation_3d<double>& rot,
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

vcl_vector<vil_image_view<float> > icam_minimizer::views(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans, 
                                           unsigned level,
                                           unsigned param_index, double pmin,
                                           double pmax, double pinc)
{
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  vnl_vector<double> params = dt_pyramid_.params();
  vcl_vector<vil_image_view<float> > res;
  vil_image_view<float>& source = source_pyramid_(level);
  unsigned n_samples;
  for(double p = pmin; p<=pmax; p+=pinc)
    {
      params[param_index] = p;
      dt_pyramid_.set_params(params);
      icam_depth_transform dt = dt_pyramid_.depth_trans(level, true);
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
  icam_depth_transform dt = dt_pyramid_.depth_trans(level, true);
  vil_image_view<float>& source = source_pyramid_(level);
  vil_image_view<float> trans_view, mask;
  unsigned n_samples;
  icam_sample::resample(source.ni(), source.nj(), source,
                        dt, trans_view, mask, n_samples);
  return trans_view;
}

vil_image_view<float> icam_minimizer::mask(vgl_rotation_3d<double>& rot,
                                           vgl_vector_3d<double>& trans, 
                                           unsigned level){
  dt_pyramid_.set_rotation(rot);
  dt_pyramid_.set_translation(trans);
  icam_depth_transform dt = dt_pyramid_.depth_trans(level, true);
  vil_image_view<float>& source = source_pyramid_(level);
  vil_image_view<float> trans_view, mask;
  unsigned n_samples;
  icam_sample::resample(source.ni(), source.nj(), source,
                        dt, trans_view, mask, n_samples);
  return mask;
}

vpgl_perspective_camera<double> icam_minimizer::dest_cam(unsigned level)
{
  vgl_rotation_3d<double>& rot = dt_pyramid_.rotation();
  vgl_vector_3d<double>& trans = dt_pyramid_.translation();
  vnl_matrix_fixed<double, 3, 3> Km = dt_pyramid_.calibration_matrix(level);
  vpgl_calibration_matrix<double> K(Km);
  vpgl_perspective_camera<double> cam(K, rot, trans);
  return cam;
}
