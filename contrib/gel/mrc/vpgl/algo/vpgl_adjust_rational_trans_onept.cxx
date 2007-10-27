#include <vcl_cmath.h> 
#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_rigid_body.h>
#include <vnl/vnl_numeric_traits.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#include <vpgl/algo/vpgl_adjust_rational_trans_onept.h>
#define TRANS_ONE_DEBUG

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
  for(double z = zmin; z<=zmax; z+=0.5) //increments of 1/2 meter
    {
      vgl_plane_3d<double> pl(0, 0, 1, -z);
      double xsq = 0, ysq = 0;
      double xm = 0, ym = 0;
      for(unsigned i = 0; i<n; ++i)
      {
        vgl_point_3d<double> pb_pt;
        if(!vpgl_backproject::bproj_plane(cams[i],
                                          corrs[i], pl,
                                          initial_point, pb_pt))
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
      if(var<error) 
        {
          error = var;
          xopt = xm;
          yopt = ym;
          zopt = z;
        }
      initial_point.set(xm, ym, z);
    }
  //at this point the best common intersection point is known.
  // do some sanitity checks
  if(zopt == zmin||zopt == zmax)
    return false;
  p_3d.set(xopt, yopt, zopt);
  return true;
}
  
bool vpgl_adjust_rational_trans_onept::
adjust(vcl_vector<vpgl_rational_camera<double> > const& cams,
       vcl_vector<vgl_point_2d<double> > const& corrs,
       vcl_vector<vgl_vector_2d<double> >& cam_translations,
       vgl_point_3d<double>& intersection)
{
  cam_translations.clear();
  if(!find_intersection_point(cams, corrs,intersection))
    return false;
  //project the intersection point into each image
 vcl_vector<vpgl_rational_camera<double> >::const_iterator cit = cams.begin();
 vcl_vector<vgl_point_2d<double> >::const_iterator rit = corrs.begin(); 
  for(; cit!=cams.end() && rit!=corrs.end(); ++cit, ++rit)
    {
      vgl_point_2d<double> uvp = (*cit).project(intersection);
      vgl_point_2d<double> uv = *rit;
      vgl_vector_2d<double> t(uv.x()-uvp.x(), uv.y()-uvp.y());
      cam_translations.push_back(t);
    }
  return true;
}
