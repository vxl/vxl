#include <limits>
#include "sdet_region.h"
//:
// \file
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_area.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vdgl/vdgl_digital_region.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>
#include <vnl/vnl_float_2.h>
sdet_region::sdet_region()
{
  boundary_ = nullptr;
  bbox_valid_ = false;
  obox_valid_ = false;
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(int npts, const float* xp, const float* yp,
                         const unsigned short *pix)
  : vdgl_digital_region(npts, xp, yp, pix)
{
  bbox_valid_ = false;
  obox_valid_ = false;
  boundary_ = nullptr;
  boundary_valid_ = false;
  region_label_ = 0;
}

sdet_region::sdet_region(vdgl_digital_region const& reg)
  :vdgl_digital_region(reg.Npix(), reg.Xj(), reg.Yj(), reg.Ij())
{
  bbox_valid_ = false;
  obox_valid_ = false;
  boundary_ = nullptr;
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
    region_points.emplace_back(this->X(), this->Y());
  vgl_convex_hull_2d<double> ch(region_points);
  vgl_polygon<double> h = ch.hull();
  vsol_polygon_2d_sptr poly = bsol_algs::poly_from_vgl(h);
  if (!poly)
    return false;
  boundary_ = poly;
  boundary_valid_ = true;
  return true;
}
bool sdet_region::compute_bbox(){
  if(Npix()==0)
    return false;
  for (this->reset(); this->next();)
    bbox_.add(vgl_point_2d<float>(this->X(), this->Y()));
  //expand by a margin of 0.5 to account for pixel granularity
  vgl_point_2d<float> minp = bbox_.min_point(), maxp = bbox_.max_point();
  bbox_.set_min_x(minp.x()-0.5f); bbox_.set_min_y(minp.y()-0.5f);
  bbox_.set_max_x(maxp.x()+0.5f); bbox_.set_max_y(maxp.y()+0.5f);
  bbox_valid_ = true;
  return true;
}
float sdet_region::int_over_union(vgl_box_2d<float> bb){
  if(!bbox_valid_)
    this->compute_bbox();
  if(!bbox_valid_)
    return 0.0f;
  vgl_box_2d<float> bint = vgl_intersection<float>(bbox_, bb);
  if(bint.is_empty())
    return 0.0f;
  float aint = vgl_area(bint);
  float aunion = vgl_area(bb) +  vgl_area(bbox_) - aint;
  return aint/aunion;
}
float sdet_region::int_over_min_area(vgl_box_2d<float> bb){
  if(!bbox_valid_)
    this->compute_bbox();
  if(!bbox_valid_)
    return 0.0f;
  vgl_box_2d<float> bint = vgl_intersection<float>(bbox_, bb);
  if(bint.is_empty())
    return 0.0f;
  float aint = vgl_area(bint);
  float a0 = vgl_area(bbox_), a1 = vgl_area(bb);
  float min_area = a0;
  if(min_area>a1)
    min_area = a1;
  return aint/min_area;
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
  obox_=vgl_oriented_box_2d<float>(majs, mins);
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
vgl_box_2d<float> sdet_region::bbox(){
  vgl_box_2d<float> temp;
  if (!bbox_valid_)
    if (!this->compute_bbox())
      return temp;
  return bbox_;
}
void sdet_region::increment_neighbors(unsigned delta){
  std::set<unsigned> new_nbrs;
  for(const auto & nbr : nbrs_)
    new_nbrs.insert(nbr+delta);
  nbrs_ = new_nbrs;
}
sdet_region_sptr merge(sdet_region_sptr const& r1,sdet_region_sptr const& r2,
                       unsigned merged_label){
  auto* r1_ptr = dynamic_cast<vdgl_digital_region*>(r1.ptr());
  auto* r2_ptr = dynamic_cast<vdgl_digital_region*>(r2.ptr());
  sdet_region_sptr ret = new sdet_region();
  auto* r12_ptr = dynamic_cast<vdgl_digital_region*>(ret.ptr());
  merge(r1_ptr, r2_ptr, r12_ptr);
  r12_ptr->ComputeIntensityStdev();
  const std::set<unsigned>& nbrs1 = r1->nbrs();
  const std::set<unsigned>& nbrs2 = r2->nbrs();
  unsigned lab1 = r1->label(), lab2 = r2->label();
  // add set union of neigbors to the merged region
  for(const auto & nit : nbrs1){
    if(nit != lab1 && nit != lab2)
      ret->add_neighbor(nit);
  }
  for(const auto & nit : nbrs2){
    if(nit != lab1 && nit != lab2)
      ret->add_neighbor(nit);
  }
  ret->set_label(merged_label);
  // note that other regions may still have neighbor pointers to r1 or r2
  // these pointers must be externally removed after the merge
  return ret;
}
// not the same as the similarity definition of Uijlings et al (see sdet_selective_search.h)
// here size similarity is not dependent on image area
//
float similarity(sdet_region_sptr const& r1, bsta_histogram<float> const& h1,
                 sdet_region_sptr const& r2, bsta_histogram<float> const& h2){
   float sim_intensity = hist_intersect(h1, h2);//min probability
  auto n1 = static_cast<float>(r1->Npix()), n2 = static_cast<float>(r2->Npix());
  if(n1 == 0.0f || n2 == 0.0f)
    return std::numeric_limits<float>::max();
  float sim_size = n1/n2;
  if(sim_size>1.0f)
    sim_size = 1.0f/sim_size;
  return sim_intensity + sim_size;
}
//
// the similarity definition of Uijlings et al (see sdet_selective_search.h)
//
float similarity(sdet_region_sptr const& r1, bsta_histogram<float> const& h1,
                 sdet_region_sptr const& r2, bsta_histogram<float> const& h2,
                 float image_area){
  float sim_intensity = hist_intersect(h1, h2);//min probability
  auto n1 = static_cast<float>(r1->Npix()), n2 = static_cast<float>(r2->Npix());
  if(n1 == 0.0f || n2 == 0.0f)
    return std::numeric_limits<float>::max();
  float sim_size = 1.0f - ((n1 + n2)/image_area);
  return sim_intensity + sim_size;
}
