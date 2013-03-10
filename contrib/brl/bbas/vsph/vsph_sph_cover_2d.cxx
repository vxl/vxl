#include <vsph/vsph_sph_cover_2d.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>
double vsph_sph_cover_2d::
inside_area(vsph_sph_box_2d const& bb, 
	    vcl_vector<vsph_sph_point_2d> const& region_rays,
	    double ray_area) const{
  double inside_area = 0.0;
  for(vcl_vector<vsph_sph_point_2d>::const_iterator rit = region_rays.begin();
      rit != region_rays.end(); ++rit)
    if(bb.contains(*rit))
      inside_area += ray_area;
  return inside_area;
}
vsph_sph_cover_2d::
vsph_sph_cover_2d(vsph_sph_box_2d const& cover_bb,
		  vcl_vector<vsph_sph_point_2d> const& region_rays,
		  double ray_area,
		  double min_area_fraction): 
  min_area_fraction_(min_area_fraction)
{
  double total_area = cover_bb.area(); 
  assert(total_area > 0.0);
  double inside_ar = this->inside_area(cover_bb, region_rays, ray_area);
  double area_fraction = inside_ar/total_area;
  if(area_fraction>=min_area_fraction){
    return;//no sub-boxes
  }
  // initial sub-division 
  vcl_vector<vsph_sph_box_2d> sub_regions; 
  double c_area = 1.0, in_area = 0.0, area_frac = 0.0;
  cover_bb.sub_divide(sub_regions); 
  for(vcl_vector<vsph_sph_box_2d>::iterator rit = sub_regions.begin();
      rit != sub_regions.end(); ++rit){
       c_area = (*rit).area();
      assert(c_area>0.0);
      in_area = inside_area(*rit, region_rays, ray_area);
      area_frac = in_area/c_area;
      cover_.push_back(cover_el(*rit, area_frac));
  }
  double out_frac = 1.0-min_area_fraction;
  bool sub_divide = true;
  while(sub_divide){
    sub_divide = false;
    vcl_vector<cover_el> temp;
    vcl_vector<vcl_vector<cover_el>::iterator> to_erase;
    for(vcl_vector<cover_el>::iterator cit = cover_.begin();
	cit != cover_.end(); ++cit){
      
      if((*cit).frac_inside_>min_area_fraction)
	continue;
     to_erase.push_back(cit);
      if((*cit).frac_inside_<out_frac)
	continue;
      sub_regions.clear();
      (*cit).box_.sub_divide(sub_regions);
      sub_divide = true;
      for(vcl_vector<vsph_sph_box_2d>::iterator bit = sub_regions.begin();
	  bit != sub_regions.end(); ++bit){
	c_area = (*bit).area();
	assert(c_area>0.0);
	in_area = inside_area(*bit, region_rays, ray_area);
	area_frac = in_area/c_area;
	temp.push_back(cover_el(*bit, area_frac));
      }
    }
    unsigned ne = to_erase.size();
    for(unsigned i = 0; i<ne; ++i)
      cover_.erase(to_erase[i]);
    for(vcl_vector<cover_el>::iterator eit = temp.begin();
      eit != temp.end(); ++eit)
    cover_.push_back(*eit);
  }
}
