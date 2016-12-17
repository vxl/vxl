#include "sdet_region.h"
//:
// \file
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vdgl/vdgl_digital_region.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>
#include <vnl/vnl_float_2.h>
sdet_region::sdet_region()
{
  boundary_ = VXL_NULLPTR;
  obox_valid_ = false;
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(int npts, const float* xp, const float* yp,
                         const unsigned short *pix)
  : vdgl_digital_region(npts, xp, yp, pix)
{
  obox_valid_ = false;
  boundary_ = VXL_NULLPTR;
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(vdgl_digital_region const& reg)
  :vdgl_digital_region(reg.Npix(), reg.Xj(), reg.Yj(), reg.Ij())
{
  obox_valid_ = false;
  boundary_ = VXL_NULLPTR;
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
bool sdet_region::compute_obox(){
  vnl_float_2 major_dir;
  if (this->Npix() < 4) return false;
  if(!this->PrincipalOrientation(major_dir))
    return false;
  if (!this->scatter_matrix_valid_)
    return false;
  vgl_point_2d<float> center(this->Xo(), this->Yo());
  vgl_vector_2d<float> vmaj(major_dir[0], major_dir[1]), vmin(-major_dir[1], major_dir[0]);
  vgl_point_2d<float> majp1(center-vmaj), majp2(center+vmaj);
  vmin /= this->AspectRatio();
  vgl_point_2d<float> minp1(center-vmin), minp2(center+vmin);
  vgl_line_segment_2d<float> majs(majp1, majp2), mins(minp1, minp2);
  obox_.set(majs, mins);
  obox_valid_ = true;
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
vgl_oriented_box_2d<float> sdet_region::obox(){
  vgl_oriented_box_2d<float> temp;
  if (!obox_valid_)
    if (!this->compute_obox())
      return temp;
  return obox_;
}
sdet_region_sptr merge(sdet_region_sptr const& r1,sdet_region_sptr const& r2,
                       unsigned merged_label){
  vdgl_digital_region* r1_ptr = dynamic_cast<vdgl_digital_region*>(r1.ptr());
  vdgl_digital_region* r2_ptr = dynamic_cast<vdgl_digital_region*>(r2.ptr());
  sdet_region_sptr ret = new sdet_region();
  vdgl_digital_region* r12_ptr = dynamic_cast<vdgl_digital_region*>(ret.ptr());
  merge(r1_ptr, r2_ptr, r12_ptr);
  const std::set<unsigned>& nbrs1 = r1->nbrs();
  const std::set<unsigned>& nbrs2 = r2->nbrs();
  unsigned lab1 = r1->label(), lab2 = r2->label();
  // add set union of neigbors to the merged region
  for(std::set<unsigned>::const_iterator nit = nbrs1.begin();
      nit != nbrs1.end(); ++nit){
    if(*nit != lab1 && *nit != lab2)
      ret->add_neighbor(*nit);
  }
  for(std::set<unsigned>::const_iterator nit = nbrs2.begin();
      nit != nbrs2.end(); ++nit){
    if(*nit != lab1 && *nit != lab2)
      ret->add_neighbor(*nit);
  }
  ret->set_label(merged_label);
  // note that other regions may still have neighbor pointers to r1 or r2
  // these pointers must be externally removed after the merge
  return ret;
}
