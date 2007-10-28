#include <vcl_cmath.h> 
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_rigid_body.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#include <vpgl/algo/vpgl_adjust_rational_trans_onept.h>

//#define TRANS_ONE_DEBUG

static const double vpgl_trans_z_step = 30.0;//meters
static double 
scatter_var(vcl_vector<vpgl_rational_camera<double> > const& cams,
            vcl_vector<vgl_point_2d<double> > const& image_pts,
            vgl_point_3d<double> const& initial_pt,
            double elevation, double& xm, double& ym)
{
  unsigned n = cams.size();
  vgl_plane_3d<double> pl(0, 0, 1, -elevation);
  double xsq = 0, ysq = 0;
  xm = 0; ym = 0;
  for(unsigned i = 0; i<n; ++i)
    {
      vgl_point_3d<double> pb_pt;
      if(!vpgl_backproject::bproj_plane(cams[i],
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
vpgl_z_search_lsqr::
vpgl_z_search_lsqr(vcl_vector<vpgl_rational_camera<double> > const& cams,
                   vcl_vector<vgl_point_2d<double> > const& image_pts,
                   vgl_point_3d<double> const& initial_pt)
  :  vnl_least_squares_function(1, 1,
                                vnl_least_squares_function::no_gradient ),
     cameras_(cams),
     image_pts_(image_pts),
     initial_pt_(initial_pt),
     xm_(0), ym_(0)
{}

void vpgl_z_search_lsqr::f(vnl_vector<double> const& elevation,
                           vnl_vector<double>& variance)
{

  variance[0] = scatter_var(cameras_, image_pts_,initial_pt_, elevation[0], xm_, ym_);
}
  

static bool 
find_intersection_point(vcl_vector<vpgl_rational_camera<double> > const& cams,
                        vcl_vector<vgl_point_2d<double> > const& corrs,
                        vgl_point_3d<double>& p_3d)
{
  unsigned n = cams.size();
  if(!n || n!=corrs.size())
    return false;
  //the average view volume center
  double x0=0, y0=0;
  // Get the lower bound on elevation range from the cameras
  double zmax = vnl_numeric_traits<double>::maxval, zmin = -zmax;
  for(vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin(); cit != cams.end(); ++cit)
    {
      x0+=(*cit).offset(vpgl_rational_camera<double>::X_INDX);
      y0+=(*cit).offset(vpgl_rational_camera<double>::Y_INDX);

      double zoff = (*cit).offset(vpgl_rational_camera<double>::Z_INDX);
      double zscale = (*cit).scale(vpgl_rational_camera<double>::Z_INDX);
      double zplus = zoff+zscale;
      double zminus = zoff-zscale;
      if(zminus>zmin) zmin = zminus;
      if(zplus<zmax) zmax = zplus;
    }
  assert(zmin<=zmax);
  x0/=n; y0/=n;
   
  double error = vnl_numeric_traits<double>::maxval;
  vgl_point_3d<double> initial_point(x0, y0, zmin);
  double xopt=0, yopt=0, zopt = 0;
  for(double z = zmin; z<=zmax; z+=vpgl_trans_z_step) 
    {
      double xm = 0, ym = 0;
      double var = scatter_var(cams, corrs,initial_point, z, xm, ym);
      if(var<error) 
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
  //at this point the best common intersection point is known.
  // do some sanitity checks
  if(zopt == zmin||zopt == zmax)
    return false;
  p_3d.set(xopt, yopt, zopt);
  return true;
}
static bool
refine_intersection_pt(vcl_vector<vpgl_rational_camera<double> > const& cams,
                       vcl_vector<vgl_point_2d<double> > const& image_pts,
                       vgl_point_3d<double> const& initial_pt,
                       vgl_point_3d<double>& final_pt)
{
  vpgl_z_search_lsqr zsf(cams, image_pts, initial_pt);
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
bool vpgl_adjust_rational_trans_onept::
adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
       vcl_vector<vgl_point_2d<double> > const& corrs,
       vcl_vector<vgl_vector_2d<double> >& cam_translations,
       vgl_point_3d<double>& final)
{
  cam_translations.clear();
  vgl_point_3d<double> intersection;
  if(!find_intersection_point(cams, corrs,intersection))
    return false;

  if(!refine_intersection_pt(cams, corrs,intersection, final))
    return false;
  vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin();
  vcl_vector<vgl_point_2d<double> >::const_iterator rit = corrs.begin(); 
  for(; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit)
    {
      vgl_point_2d<double> uvp = (*cit).project(final);
      vgl_point_2d<double> uv = *rit;
      vgl_vector_2d<double> t(uv.x()-uvp.x(), uv.y()-uvp.y());
      cam_translations.push_back(t);
    }
  return true;
}
