// This is brl/bseg/bmrf/bmrf_epipole.cxx
//:
// \file

#include "bmrf_epipole.h"
#include <vcl_cmath.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_distance.h>


//: Constructor
bmrf_epipole::bmrf_epipole()
 : epi_(0.0,0.0)
{
}

//: Constructor
bmrf_epipole::bmrf_epipole(double u, double v)
 : epi_(u,v)
{
}

//: Constructor
bmrf_epipole::bmrf_epipole(const vgl_point_2d<double>& epipole)
 : epi_(epipole)
{
}

//: Conversion to epipolar coordinates
vgl_point_2d<double>
bmrf_epipole::to_epi_coords(const vgl_point_2d<double>& u_v) const
{
  // the line from the epipole to the image point
  vgl_line_segment_2d<double> epi_line(epi_, u_v);

  return vgl_point_2d<double>(vgl_distance(epi_, u_v), epi_line.slope_radians());
}


//: Conversion to epipolar coordinates
void
bmrf_epipole::to_epi_coords(double u, double v, double &s, double &a) const
{
  vgl_point_2d<double> s_a = this->to_epi_coords(vgl_point_2d<double>(u,v));
  s = s_a.x();
  a = s_a.y();
}


//: Conversion to image coordinates
vgl_point_2d<double>
bmrf_epipole::to_img_coords(const vgl_point_2d<double>& s_a) const
{
  double u = s_a.x()*vcl_cos(s_a.y());
  double v = s_a.x()*vcl_sin(s_a.y());
  return epi_ + vgl_vector_2d<double>(u,v);
}


//: Conversion to image coordinates
void
bmrf_epipole::to_img_coords(double s, double a, double &u, double &v) const
{
  vgl_point_2d<double> u_v = this->to_epi_coords(vgl_point_2d<double>(s,a));
  u = u_v.x();
  v = u_v.y();
}


