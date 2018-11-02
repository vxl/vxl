#include <iostream>
#include <limits>
#include <vsph/vsph_sph_cover_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

double vsph_sph_cover_2d::
inside_area(vsph_sph_box_2d const& bb,
            std::vector<vsph_sph_point_2d> const& region_rays,
            double ray_area) const
{
  double inside_area = 0.0;
  for (const auto & region_ray : region_rays)
    if (bb.contains(region_ray))
      inside_area += ray_area;
  return inside_area;
}

vsph_sph_cover_2d::
vsph_sph_cover_2d(vsph_sph_box_2d const& cover_bb,
                  std::vector<vsph_sph_point_2d> const& region_rays,
                  double ray_area,
                  double min_area_fraction):
  cover_bb_(cover_bb), min_area_fraction_(min_area_fraction)
{
  double total_area = cover_bb.area();
  assert(total_area > 0.0);
  double inside_ar = this->inside_area(cover_bb, region_rays, ray_area);
  double area_fraction = inside_ar/total_area;
  if (area_fraction>=min_area_fraction) {
    return;//no sub-boxes
  }
  cover_.emplace_back(cover_bb, area_fraction);
  double c_area = 1.0, in_area = 0.0;

  std::vector<cover_el> keep;
  bool sub_divide = true;
  while (sub_divide) {
    sub_divide = false;
    std::vector<cover_el> temp;
    for (auto & cit : cover_)
    {
      area_fraction = cit.frac_inside_;
      if (area_fraction == 0.0)
        continue;
      if (area_fraction>min_area_fraction){
        keep.push_back(cit);
        continue;
      }
      std::vector<vsph_sph_box_2d> sub_regions;
      sub_divide = cit.box_.sub_divide(sub_regions);
      for (auto & sub_region : sub_regions) {
        c_area = sub_region.area();
        assert(c_area>0.0);
        in_area = inside_area(sub_region, region_rays, ray_area);
        area_fraction = in_area/c_area;
        temp.emplace_back(sub_region, area_fraction);
      }
    }
    if (sub_divide){
      cover_.clear();
      cover_ = temp;
    }
  }
  //all done, add the kept boxes to the cover
  for (auto & kit : keep)
    cover_.push_back(kit);
  // compute area fraction achieved
  total_area_ = 0.0;
  double total_inside = 0.0;
  for (auto & cit : cover_)
    {
      double a = (cit.box_).area();
      total_area_ += a;
      total_inside += a*(cit.frac_inside_);
    }
  assert(total_area_>0.0);
  actual_area_fraction_ = total_inside/total_area_;
}

void vsph_sph_cover_2d::set(double min_area_fraction, double total_area,
                            double actual_area_fraction,
                            std::vector<cover_el> const& cover){
  min_area_fraction_ = min_area_fraction;
  total_area_ = total_area;
  actual_area_fraction_ =   actual_area_fraction;
  cover_.clear();
  cover_ = cover;
}

vsph_sph_cover_2d vsph_sph_cover_2d::transform(double t_theta,
                                               double t_phi, double scale,
                                               double theta_c,double phi_c,
                                               bool in_radians) const{
  std::vector<cover_el> tcov;
  for (const auto & cit : cover_)
    {
      double fin = cit.frac_inside_;
      vsph_sph_box_2d tbox = (cit.box_).transform(t_theta, t_phi, scale,
                                                   theta_c, phi_c, in_radians);
      tcov.emplace_back(tbox, fin);
    }
  vsph_sph_cover_2d rcov;
  rcov.set(min_area_fraction_, total_area_, actual_area_fraction_, tcov);
  return rcov;
}

bool intersection(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2,
                  vsph_sph_cover_2d& cover_inter){
  const std::vector<cover_el>& cels1 = c1.cover();
  const std::vector<cover_el>& cels2 = c2.cover();
  std::vector<cover_el> intersection_cover;
  unsigned n1 = cels1.size(), n2 = cels2.size();
  if (n1==0 || n2 ==0)
          return false;
  bool any_intersection = false;
  for (unsigned i = 0; i<n1; ++i){
    double fin = cels1[i].frac_inside_;
    const vsph_sph_box_2d& b1 = cels1[i].box_;
    for (unsigned j = 0; j<n1; ++j){
      const vsph_sph_box_2d& b2 = cels2[j].box_;
      double max_fin =cels2[j].frac_inside_;
      if (max_fin<fin)
        max_fin = fin;
      std::vector<vsph_sph_box_2d> boxes;
      if (!intersection(b1, b2, boxes))
        continue;
      for (auto & boxe : boxes)
        intersection_cover.emplace_back(boxe, max_fin);
          any_intersection = true;
    }
  }
  if (!any_intersection) return false;
  //determine the least area fraction of the two covers
  double min_af = c1.min_area_fraction();
  if (min_af > c2.min_area_fraction())
    min_af = c2.min_area_fraction();
  double total_area = 0.0;
  double total_inside = 0.0;
  for (auto & cit : intersection_cover)
    {
      double a = (cit.box_).area();
      total_area += a;
      total_inside += a*(cit.frac_inside_);
    }
  if (total_area == 0.0) return false;
  double actual_area_fraction = total_inside/total_area;

  cover_inter.set(min_af, total_area, actual_area_fraction, intersection_cover);
  return true;
}

double intersection_area(vsph_sph_cover_2d const& c1,
                         vsph_sph_cover_2d const& c2)
{
  const std::vector<cover_el>& cels1 = c1.cover();
  const std::vector<cover_el>& cels2 = c2.cover();
  unsigned n1 = cels1.size(), n2 = cels2.size();
  double area = 0.0;
  for (unsigned i = 0; i<n1; ++i){
    const vsph_sph_box_2d& b1 = cels1[i].box_;
    for (unsigned j = 0; j<n2; ++j){
      const vsph_sph_box_2d& b2 = cels2[j].box_;
      area += intersection_area(b1, b2);
    }
  }
  return area;
}
