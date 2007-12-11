#include "vpgl_rational_geo_adjust.h"
//:
// \file

#include <vcl_cmath.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
//#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vpgl/algo/vpgl_ray_intersect.h>
#define ADJUST_DEBUG

vpgl_adjust_lsqr::
vpgl_adjust_lsqr(vpgl_rational_camera<double>  const& rcam,
                 vcl_vector<vgl_point_2d<double> > const& img_pts,
                 vcl_vector<vgl_point_3d<double> > const& geo_pts,
                 unsigned num_unknowns, unsigned num_residuals)
  : vnl_least_squares_function(num_unknowns, num_residuals,
                               vnl_least_squares_function::no_gradient),
    rcam_(rcam), img_pts_(img_pts), geo_pts_(geo_pts)
{
  num_corrs_ = img_pts.size();
}
// The virtual least-squares cost function.
// The unknowns are [xscale, xoff, yscale, yoff, zscale, zoff]
// The error residual vector elements are image errors for each point
void vpgl_adjust_lsqr::f( const vnl_vector<double>& unknowns,
                          vnl_vector<double> & projection_error)
{
  // set the scales and offsets for the rational camera
#if 0
  rcam_.set_scale(vpgl_rational_camera<double>::X_INDX, unknowns[0]);
  rcam_.set_offset(vpgl_rational_camera<double>::X_INDX, unknowns[1]);
  rcam_.set_scale(vpgl_rational_camera<double>::Y_INDX, unknowns[2]);
  rcam_.set_offset(vpgl_rational_camera<double>::Y_INDX, unknowns[3]);
  rcam_.set_scale(vpgl_rational_camera<double>::Z_INDX, unknowns[4]);
  rcam_.set_offset(vpgl_rational_camera<double>::Z_INDX, unknowns[5]);

  vcl_cout << "x(" << unknowns[0] << ' ' << unknowns[1] << ")\n"
           << "y(" << unknowns[2] << ' ' << unknowns[3] << ")\n"
           << "z(" << unknowns[4] << ' ' << unknowns[5] << ")\n";
#endif
  rcam_.set_offset(vpgl_rational_camera<double>::X_INDX, unknowns[0]);
  rcam_.set_offset(vpgl_rational_camera<double>::Y_INDX, unknowns[1]);
  rcam_.set_offset(vpgl_rational_camera<double>::Z_INDX, unknowns[2]);

  // project the geo points using the camera
  unsigned ir = 0;
  for (unsigned i = 0; i<num_corrs_; ++i)
  {
    vgl_point_2d<double> pp = rcam_.project(geo_pts_[i]);
    vgl_point_2d<double> c = img_pts_[i];
    projection_error[ir++] = (pp.x()-c.x())*(pp.x()-c.x());
    projection_error[ir++] = (pp.y()-c.y())*(pp.y()-c.y());

#if 0
    projection_error[ir++] = vcl_fabs(pp.x()-c.x());
    projection_error[ir++] = vcl_fabs(pp.y()-c.y());

    vcl_cout << "perror[" << i << "](" << projection_error[ir-2] << ' '
             << projection_error[ir-1] << ")\n";
#endif
  }
}

//This method works by projecting the image points onto a plane that is
//at the average elevation of the 3-d points. zoff is set to this value.
//xoff and yoff are adjusted to remove the average translation between
//The backprojected image points and their actual 3-d locations.
//
static bool initial_offsets(vpgl_rational_camera<double> const& initial_rcam,
                            vcl_vector<vgl_point_2d<double> > const& img_pts,
                            vcl_vector<vgl_point_3d<double> > const& geo_pts,
                            double& xoff,
                            double& yoff,
                            double& zoff)
{
  unsigned npts = img_pts.size();
  //get the average elevation
  zoff = 0;
  for (unsigned i = 0; i<npts; ++i)
    zoff += geo_pts[i].z();
  zoff /= npts;
  //construct a x-y plane with this elevation
  vgl_plane_3d<double> pl(0, 0, 1, -zoff);

  //an inital point for the backprojection
  double xo = initial_rcam.offset(vpgl_rational_camera<double>::X_INDX);
  double yo = initial_rcam.offset(vpgl_rational_camera<double>::Y_INDX);
  vgl_point_3d<double> initial_pt(xo, yo, zoff);

  double xshift = 0, yshift = 0;
  //backproject the image points onto this plane
  for (unsigned i = 0; i<npts; ++i)
  {
    vgl_point_3d<double> bp_pt;
    if (!vpgl_backproject::bproj_plane(initial_rcam,
                                       img_pts[i], pl,
                                       initial_pt, bp_pt))
      return false;
    xshift += geo_pts[i].x()- bp_pt.x();
    yshift += geo_pts[i].y()- bp_pt.y();
  }
  xoff = xo + xshift/npts;
  yoff = yo + yshift/npts;
  return true;
}

//: A function adjust the rational camera 3-d scales and offsets
// Returns true if successful, else false
bool vpgl_rational_geo_adjust::
adjust(vpgl_rational_camera<double> const& initial_rcam,
       vcl_vector<vgl_point_2d<double> > img_pts,
       vcl_vector<vgl_point_3d<double> > geo_pts,
       vpgl_rational_camera<double> & adj_rcam)
{
  //Get initial offsets by backprojection
  double xoff=0, yoff=0, zoff=0;
  if (!initial_offsets(initial_rcam, img_pts, geo_pts, xoff, yoff, zoff))
    return false;
  unsigned num_corrs = img_pts.size();
  unsigned num_unknowns = 3;
  unsigned num_residuals = num_corrs*2;
  // Initialize the least squares function
  vpgl_adjust_lsqr lsf(initial_rcam, img_pts, geo_pts,
                       num_unknowns, num_residuals);

  // Create the Levenberg Marquardt minimizer
  vnl_levenberg_marquardt levmarq(lsf);
#ifdef ADJUST_DEBUG
  levmarq.set_verbose(true);
  levmarq.set_trace(true);
#endif
#if 0
  // Set the x-tolerance.  When the length of the steps taken in X (variables)
  // are no longer than this, the minimization terminates.
  levmarq.set_x_tolerance(1e-5);

  // Set the epsilon-function.  This is the step length for FD Jacobian.
  levmarq.set_epsilon_function(1.0);

  // Set the f-tolerance.  When the successive RMS errors are less than this,
  // minimization terminates.
  levmarq.set_f_tolerance(1e-10);
#endif
  // Set the maximum number of iterations
  levmarq.set_max_function_evals(10000);

  // Create an initial values of parameters with which to start
  vnl_vector<double> unknowns(num_unknowns);

#if 0
  unknowns[0] = initial_rcam.scale(vpgl_rational_camera<double>::X_INDX);
  unknowns[1] = xoff;
  unknowns[2] = initial_rcam.scale(vpgl_rational_camera<double>::Y_INDX);
  unknowns[3] = yoff;
  unknowns[4] = initial_rcam.scale(vpgl_rational_camera<double>::Z_INDX);
  unknowns[5] = zoff;
#endif
  unknowns[0] = xoff;
  unknowns[1] = yoff;
  unknowns[2] = zoff;

  // Minimize the error and get best correspondence vertices and translations
  levmarq.minimize(unknowns);

  // Summarize the results
  levmarq.diagnose_outcome();
#ifdef ADJUST_DEBUG
  vcl_cout << "Min error of " << levmarq.get_end_error() << " at the following local minima : " << '\n';
#endif
  // set up the camera for return
  adj_rcam = initial_rcam;
  // set the scales and offsets for the rational camera

  adj_rcam.set_offset(vpgl_rational_camera<double>::X_INDX, unknowns[0]);
  adj_rcam.set_offset(vpgl_rational_camera<double>::Y_INDX, unknowns[1]);
  adj_rcam.set_offset(vpgl_rational_camera<double>::Z_INDX, unknowns[2]);

#ifdef ADJUST_DEBUG
  for (unsigned i = 0; i<num_corrs; ++i)
  {
    vgl_point_2d<double> pp = adj_rcam.project(geo_pts[i]);
    vgl_point_2d<double> c = img_pts[i];
    double d = vgl_distance<double>(c, pp);
    vcl_cout << "p[" << i << "]->(" << pp.x() << ' ' << pp.y() << ")\n"
             << "c(" << c.x() << ' ' << c.y() << "): " << d << '\n';
  }
#endif
  // Return success
  return true;
}
