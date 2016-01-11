// This is brl/bseg/boxm2/vecf/boxm2_vecf_plot_orbit.h
#ifndef boxm2_vecf_plot_orbit_h
#define boxm2_vecf_plot_orbit_h
//:
// \file
// \brief A tableau to view projections of the orbit model
// \author J.L. Mundy
// \date July 25, 2015
#include <vnl/vnl_math.h>
#include "boxm2_vecf_orbit_params.h"

class boxm2_vecf_plot_orbit
{
 public:
  static void plot_inferior_margin(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max, vcl_vector<vgl_point_3d<double> >& pts);
  static void plot_superior_margin(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max,vcl_vector<vgl_point_3d<double> >& pts);
  static void plot_crease(boxm2_vecf_orbit_params const& opr, bool is_right, double xm_min, double xm_max, vcl_vector<vgl_point_3d<double> >& pts);
  static bool plot_limits(vcl_vector<vgl_point_3d<double> > const& inf_pts, vcl_vector<vgl_point_3d<double> > const& sup_pts, int& imin, int& imax);
 private:
  boxm2_vecf_plot_orbit();
};
#endif // boxm2_vecf_plot_orbit
