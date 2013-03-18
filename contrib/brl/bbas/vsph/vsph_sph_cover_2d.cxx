#include <vsph/vsph_sph_cover_2d.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>

double vsph_sph_cover_2d::
inside_area(vsph_sph_box_2d const& bb,
            vcl_vector<vsph_sph_point_2d> const& region_rays,
            double ray_area) const
{
  double inside_area = 0.0;
  for (vcl_vector<vsph_sph_point_2d>::const_iterator rit = region_rays.begin();
       rit != region_rays.end(); ++rit)
    if (bb.contains(*rit))
      inside_area += ray_area;
  return inside_area;
}

vsph_sph_cover_2d::
vsph_sph_cover_2d(vsph_sph_box_2d const& cover_bb,
                  vcl_vector<vsph_sph_point_2d> const& region_rays,
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
  cover_.push_back(cover_el(cover_bb, area_fraction));
  double c_area = 1.0, in_area = 0.0;

  vcl_vector<cover_el> keep;
  bool sub_divide = true;
  while (sub_divide) {
    sub_divide = false;
    vcl_vector<cover_el> temp;
    for (vcl_vector<cover_el>::iterator cit = cover_.begin();
         cit != cover_.end(); ++cit)
    {
      area_fraction = (*cit).frac_inside_;
      if (area_fraction == 0.0)
        continue;
      if (area_fraction>min_area_fraction){
        keep.push_back(*cit);
        continue;
      }
      vcl_vector<vsph_sph_box_2d> sub_regions;
      sub_divide = (*cit).box_.sub_divide(sub_regions);
      for (vcl_vector<vsph_sph_box_2d>::iterator bit = sub_regions.begin();
           bit != sub_regions.end(); ++bit) {
        c_area = (*bit).area();
        assert(c_area>0.0);
        in_area = inside_area(*bit, region_rays, ray_area);
        area_fraction = in_area/c_area;
        temp.push_back(cover_el(*bit, area_fraction));
      }
    }
    if (sub_divide){
      cover_.clear();
      cover_ = temp;
    }
  }
  //all done, add the kept boxes to the cover
  for (vcl_vector<cover_el>::iterator kit = keep.begin();
       kit != keep.end(); ++kit)
    cover_.push_back(*kit);
  // compute area fraction achieved
  total_area_ = 0.0;
  double total_inside = 0.0;
  for (vcl_vector<cover_el>::iterator cit = cover_.begin();
       cit != cover_.end(); ++cit)
    {
      double a = ((*cit).box_).area();
      total_area_ += a;
      total_inside += a*((*cit).frac_inside_);
    }
  assert(total_area_>0.0);
  actual_area_fraction_ = total_inside/total_area_;
}

void vsph_sph_cover_2d::set(double min_area_fraction, double total_area,
                            double actual_area_fraction,
                            vcl_vector<cover_el> const& cover){
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
  vcl_vector<cover_el> tcov;
  for (vcl_vector<cover_el>::const_iterator cit = cover_.begin();
       cit != cover_.end(); ++cit)
    {
      double fin = cit->frac_inside_;
      vsph_sph_box_2d tbox = (cit->box_).transform(t_theta, t_phi, scale,
                                                   theta_c, phi_c, in_radians);
      tcov.push_back(cover_el(tbox, fin));
    }
  vsph_sph_cover_2d rcov;
  rcov.set(min_area_fraction_, total_area_, actual_area_fraction_, tcov);
  return rcov;
}

bool intersection(vsph_sph_cover_2d const& c1, vsph_sph_cover_2d const& c2,
                  vsph_sph_cover_2d& cover_inter){
  const vcl_vector<cover_el>& cels1 = c1.cover();
  const vcl_vector<cover_el>& cels2 = c2.cover();
  vcl_vector<cover_el> intersection_cover;
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
      vcl_vector<vsph_sph_box_2d> boxes;
      if (!intersection(b1, b2, boxes))
        continue;
      for (vcl_vector<vsph_sph_box_2d>::iterator bit = boxes.begin();
           bit != boxes.end(); ++bit)
        intersection_cover.push_back(cover_el(*bit, max_fin));
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
  for (vcl_vector<cover_el>::iterator cit = intersection_cover.begin();
       cit != intersection_cover.end(); ++cit)
    {
      double a = ((*cit).box_).area();
      total_area += a;
      total_inside += a*((*cit).frac_inside_);
    }
  if (total_area == 0.0) return false;
  double actual_area_fraction = total_inside/total_area;

  cover_inter.set(min_af, total_area, actual_area_fraction, intersection_cover);
  return true;
}

double intersection_area(vsph_sph_cover_2d const& c1,
                         vsph_sph_cover_2d const& c2)
{
  const vcl_vector<cover_el>& cels1 = c1.cover();
  const vcl_vector<cover_el>& cels2 = c2.cover();
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
