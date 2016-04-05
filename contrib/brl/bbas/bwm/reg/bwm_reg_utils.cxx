#include "bwm_reg_utils.h"
#include <vgl/vgl_point_2d.h>
#include <vpgl/algo/vpgl_backproject.h>
#include <vsol/vsol_digital_curve_2d.h>
#include <vsol/vsol_digital_curve_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

// back project a set of edges onto a 3-d plane
bool bwm_reg_utils::
back_project_edges(std::vector<vsol_digital_curve_2d_sptr> const& edges_2d,
                   vpgl_rational_camera<double> const& rcam,
                   vgl_plane_3d<double> const& pl,
                   vgl_point_3d<double> const& initial_guess,
                   std::vector<vsol_digital_curve_3d_sptr>& edges_3d)
{
  edges_3d.clear();
  for (std::vector<vsol_digital_curve_2d_sptr>::const_iterator eit = edges_2d.begin();
       eit != edges_2d.end(); ++eit){
    vsol_digital_curve_3d_sptr dc3d = new vsol_digital_curve_3d;
    unsigned n = (*eit)->size();
    for (unsigned i = 0; i<n; ++i)
    {
      vsol_point_2d_sptr p = (*eit)->point(i);
      vgl_point_2d<double> ip = p->get_p();
      vgl_point_3d<double> p3d;
      if (!vpgl_backproject::bproj_plane(rcam, ip, pl, initial_guess, p3d))
         return false;
      dc3d->add_vertex(new vsol_point_3d(p3d));
    }
    edges_3d.push_back(dc3d);
  }
  return true;
}

//forward project edges onto an image plane
void bwm_reg_utils::
project_edges(std::vector<vsol_digital_curve_3d_sptr> const& edges_3d,
              vpgl_rational_camera<double> const& rcam,
              std::vector<vsol_digital_curve_2d_sptr>& edges_2d)
{
  edges_2d.clear();
  for (std::vector<vsol_digital_curve_3d_sptr>::const_iterator eit = edges_3d.begin();
       eit != edges_3d.end(); ++eit){
    vsol_digital_curve_2d_sptr dc2d = new vsol_digital_curve_2d;
    unsigned n = (*eit)->size();
    for (unsigned i = 0; i<n; ++i)
    {
      vsol_point_3d_sptr p = (*eit)->point(i);
      vgl_point_3d<double> p3d = p->get_p();
      vgl_point_2d<double> ip = rcam.project(p3d);
      vsol_point_2d_sptr ipv = new vsol_point_2d(ip);
      dc2d->add_vertex(ipv);
    }
    edges_2d.push_back(dc2d);
  }
}
