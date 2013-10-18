#include "vpgl_rational_adjust_onept.h"
//:
// \file
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>

#include <vcl_limits.h>

//#define TRANS_ONE_DEBUG

static const double vpgl_trans_z_step = 30.0;//meters

#if 0  // replace with weighted version
static double
scatter_var(vcl_vector<vpgl_rational_camera<double> > const& cams,
            vcl_vector<vgl_point_2d<double> > const& image_pts,
            vgl_point_3d<double> const& initial_pt,
            double elevation, double& xm, double& ym)
{
  unsigned int n = cams.size();
  vgl_plane_3d<double> pl(0, 0, 1, -elevation);
  double xsq = 0, ysq = 0;
  xm = 0; ym = 0;
  for (unsigned int i = 0; i<n; ++i)
  {
    vgl_point_3d<double> pb_pt;
    if (!vpgl_backproject::bproj_plane(cams[i],
                                       image_pts[i], pl,
                                       initial_pt, pb_pt))
      return false;
    double x = pb_pt.x(), y = pb_pt.y();
    xm+=x; ym +=y;
    xsq+=x*x; ysq+=y*y;
  }
  xm/=n; ym/=n;
  double xvar = xsq-(n*xm*xm);
  double yvar = ysq-(n*ym*ym);
  xvar/=n; yvar/=n;
  double var = vcl_sqrt(xvar*xvar + yvar*yvar);
  return var;
}
#endif
// do not use weights in calculating scatter, cause we want to minimize the scatter of all observations around a 'weighted' mean
static double
scatter_var(vcl_vector<vpgl_rational_camera<double> > const& cams,
            vcl_vector<float> const& cam_weights,
            vcl_vector<vgl_point_2d<double> > const& image_pts,
            vgl_point_3d<double> const& initial_pt,
            double elevation, double& xm, double& ym)
{
  unsigned int n = cams.size();
  vgl_plane_3d<double> pl(0, 0, 1, -elevation);
  double xsq = 0, ysq = 0;
  xm = 0; ym = 0;
  vcl_vector<vgl_point_3d<double> > pb_pts;
  for (unsigned int i = 0; i<n; ++i)
  {
    vgl_point_3d<double> pb_pt;
    if (!vpgl_backproject::bproj_plane(cams[i],
                                       image_pts[i], pl,
                                       initial_pt, pb_pt))
      return false;
    pb_pts.push_back(pb_pt);
  }
  for (unsigned i = 0; i < n; i++) 
  {
    double x = pb_pts[i].x(), y = pb_pts[i].y();
    xm+=cam_weights[i]*x; ym +=cam_weights[i]*y;
  }
  for (unsigned i = 0; i < n; i++)
  {
    double x = pb_pts[i].x(), y = pb_pts[i].y();
    xsq+=(x-xm)*(x-xm); ysq+=(y-ym)*(y-ym);
  }
  xsq/=n; ysq/=n;
  double var = vcl_sqrt(xsq*xsq + ysq*ysq);
  return var;
}



vpgl_z_search_lsqr::
vpgl_z_search_lsqr(vcl_vector<vpgl_rational_camera<double> > const& cams,
                   vcl_vector<float> const& cam_weights,
                   vcl_vector<vgl_point_2d<double> > const& image_pts,
                   vgl_point_3d<double> const& initial_pt)
  :  vnl_least_squares_function(1, 1,
                                vnl_least_squares_function::no_gradient ),
     initial_pt_(initial_pt),
     cameras_(cams),
     cam_weights_(cam_weights),
     image_pts_(image_pts),
     xm_(0), ym_(0)
{}

void vpgl_z_search_lsqr::f(vnl_vector<double> const& elevation,
                           vnl_vector<double>& variance)
{
  variance[0] = scatter_var(cameras_, cam_weights_, image_pts_,initial_pt_, elevation[0], xm_, ym_);
}

bool vpgl_rational_adjust_onept::
find_intersection_point(vcl_vector<vpgl_rational_camera<double> > const& cams,
                        vcl_vector<float> const& cam_weights,
                        vcl_vector<vgl_point_2d<double> > const& corrs,
                        vgl_point_3d<double>& p_3d)
{
  unsigned int n = cams.size();
  if (!n || n!=corrs.size())
    return false;
  //the average view volume center
  double x0=0, y0=0;
  // Get the lower bound on elevation range from the cameras
  double zmax = vnl_numeric_traits<double>::maxval, zmin = -zmax;
  for (vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin(); cit != cams.end(); ++cit)
  {
    x0+=(*cit).offset(vpgl_rational_camera<double>::X_INDX);
    y0+=(*cit).offset(vpgl_rational_camera<double>::Y_INDX);

    double zoff = (*cit).offset(vpgl_rational_camera<double>::Z_INDX);
    double zscale = (*cit).scale(vpgl_rational_camera<double>::Z_INDX);
    double zplus = zoff+zscale;
    double zminus = zoff-zscale;
    if (zminus>zmin) zmin = zminus;
    if (zplus<zmax) zmax = zplus;
  }
  assert(zmin<=zmax);
  x0/=n; y0/=n;

  double error = vnl_numeric_traits<double>::maxval;
  vgl_point_3d<double> initial_point(x0, y0, zmin);
  double xopt=0, yopt=0, zopt = 0;
  for (double z = zmin; z<=zmax; z+=vpgl_trans_z_step)
  {
    double xm = 0, ym = 0;
    //double var = scatter_var(cams, corrs,initial_point, z, xm, ym);
    double var = scatter_var(cams, cam_weights, corrs,initial_point, z, xm, ym);
    if (var<error)
    {
      error = var;
      xopt = xm;
      yopt = ym;
      zopt = z;
    }
    initial_point.set(xm, ym, z);
#ifdef TRANS_ONE_DEBUG
    vcl_cout << z << '\t' << var << '\n';
#endif
  }
  // at this point the best common intersection point is known.
  // do some sanity checks
  if (zopt == zmin||zopt == zmax)
    return false;
  p_3d.set(xopt, yopt, zopt);
  return true;
}

bool vpgl_rational_adjust_onept::
refine_intersection_pt(vcl_vector<vpgl_rational_camera<double> > const& cams,
                       vcl_vector<float> const& cam_weights,
                       vcl_vector<vgl_point_2d<double> > const& image_pts,
                       vgl_point_3d<double> const& initial_pt,
                       vgl_point_3d<double>& final_pt)
{
  vpgl_z_search_lsqr zsf(cams, cam_weights, image_pts, initial_pt);
  vnl_levenberg_marquardt levmarq(zsf);
#ifdef TRANS_ONE_DEBUG
  levmarq.set_verbose(true);
#endif
  // Set the x-tolerance.  When the length of the steps taken in X (variables)
  // are no longer than this, the minimization terminates.
  levmarq.set_x_tolerance(1e-10);

  // Set the epsilon-function.  This is the step length for FD Jacobian.
  levmarq.set_epsilon_function(1);

  // Set the f-tolerance.  When the successive RMS errors are less than this,
  // minimization terminates.
  levmarq.set_f_tolerance(1e-15);

  // Set the maximum number of iterations
  levmarq.set_max_function_evals(10000);

  vnl_vector<double> elevation(1);
  elevation[0]=initial_pt.z();

  // Minimize the error and get the best intersection point
  levmarq.minimize(elevation);
#ifdef TRANS_ONE_DEBUG
  levmarq.diagnose_outcome();
#endif
  final_pt.set(zsf.xm(), zsf.ym(), elevation[0]);
  return true;
}

bool vpgl_rational_adjust_onept::
adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
       vcl_vector<vgl_point_2d<double> > const& corrs,
       vcl_vector<vgl_vector_2d<double> >& cam_translations,
       vgl_point_3d<double>& final)
{
  cam_translations.clear();
  vgl_point_3d<double> intersection;
  vcl_vector<float> cam_weights(cams.size(), 1.0f/cams.size());
  if (!find_intersection_point(cams, cam_weights, corrs,intersection))
    return false;

  if (!refine_intersection_pt(cams, cam_weights, corrs,intersection, final))
    return false;
  vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin();
  vcl_vector<vgl_point_2d<double> >::const_iterator rit = corrs.begin();
  for (; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit)
  {
    vgl_point_2d<double> uvp = (*cit).project(final);
    vgl_point_2d<double> uv = *rit;
    vgl_vector_2d<double> t(uv.x()-uvp.x(), uv.y()-uvp.y());
    cam_translations.push_back(t);
  }
  return true;
}

// pass a weight for each camera, the weights should add up to 1.0
bool vpgl_rational_adjust_onept::
  adjust_with_weights(vcl_vector<vpgl_rational_camera<double> > const& cams, 
                      vcl_vector<float> weights,
                      vcl_vector<vgl_point_2d<double> > const& corrs,
                      vcl_vector<vgl_vector_2d<double> >& cam_translations,
                      vgl_point_3d<double>& final)
{
  cam_translations.clear();
  vgl_point_3d<double> intersection;
  if (!find_intersection_point(cams, weights, corrs,intersection))
    return false;
  if (!refine_intersection_pt(cams, weights, corrs,intersection, final))
    return false;
  vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin();
  vcl_vector<vgl_point_2d<double> >::const_iterator rit = corrs.begin();
  vcl_vector<float>::const_iterator wit = weights.begin();
  for (; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit, ++wit)
  {
    // if weight is 1, it's a special case, do not change it at all (the projection still yields a tiny translation so just ignore that)
    if ((*wit) == 1.0f) {
      vgl_vector_2d<double> t(0.0, 0.0);
      cam_translations.push_back(t);
      continue;
    }
    vgl_point_2d<double> uvp = (*cit).project(final);
    vgl_point_2d<double> uv = *rit;
    vgl_vector_2d<double> t(uv.x()-uvp.x(), uv.y()-uvp.y());
    cam_translations.push_back(t);
  }
  return true;
}