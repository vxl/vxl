#include "sdet_region.h"
//:
// \file
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vdgl/vdgl_digital_region.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>

sdet_region::sdet_region()
{
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(int npts, const float* xp, const float* yp,
                         const unsigned short *pix)
  : vdgl_digital_region(npts, xp, yp, pix)
{
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(vdgl_digital_region const& reg)
  :vdgl_digital_region(reg.Npix(), reg.Xj(), reg.Yj(), reg.Ij())
{
  boundary_valid_ = false;
  region_label_ = 0;
}

bool sdet_region::compute_boundary()
{
  if (boundary_valid_)
    return true;
  //need at least a triangle
  if (this->Npix()<3)
    return false;
  std::vector<vgl_point_2d<double> > region_points;
  for (this->reset(); this->next();)
    region_points.push_back(vgl_point_2d<double>(this->X(), this->Y()));
  vgl_convex_hull_2d<double> ch(region_points);
  vgl_polygon<double> h = ch.hull();
  vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(h);
  if (!poly)
    return false;
  boundary_ = poly;
  boundary_valid_ = true;
  return true;
}

vsol_polygon_2d_sptr sdet_region::boundary()
{
  vsol_polygon_2d_sptr temp;
  if (!boundary_valid_)
    if (!this->compute_boundary())
      return temp;
  return boundary_;
}
