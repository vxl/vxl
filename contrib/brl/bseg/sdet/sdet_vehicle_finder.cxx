//:
// \file
#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <bsol/bsol_algs.h>
#include <vcl_cmath.h> // for sqrt()
#include <sdet/sdet_region.h>
#include <brip/brip_roi.h>
#include <brip/brip_para_cvrg.h>
#include <sdet/sdet_watershed_region_proc_params.h>
#include <sdet/sdet_watershed_region_proc.h>
#include <sdet/sdet_vehicle_finder.h>

sdet_vehicle_finder::sdet_vehicle_finder(sdet_vehicle_finder_params& vfp)
  : sdet_vehicle_finder_params(vfp)
{
}

sdet_vehicle_finder::~sdet_vehicle_finder()
{
}

void sdet_vehicle_finder::set_pick(const int x, const int y)
{
  pick_.x()=x; pick_.y()=y;
  this->construct_search_box();
}

void sdet_vehicle_finder::construct_search_box()
{
  search_box_ = new vsol_box_2d();
  search_box_->add_point(pick_.x()-search_radius_, pick_.y()-search_radius_);
  search_box_->add_point(pick_.x()+search_radius_, pick_.y()-search_radius_);
  search_box_->add_point(pick_.x()+search_radius_, pick_.y()+search_radius_);
  search_box_->add_point(pick_.x()-search_radius_, pick_.y()+search_radius_);
}


bool sdet_vehicle_finder::
region_box_union(vcl_vector<sdet_region_sptr> const& regions,
                 vsol_box_2d_sptr& box_union)
{
  int n = regions.size();
  if (!n)
    return false;
  box_union = regions[0]->get_bounding_box();
  for (int i = 1; i<n; i++)
  {
    vsol_box_2d_sptr bi = regions[i]->get_bounding_box();
    if (!bsol_algs::box_union(box_union, bi, box_union))
      return false;
  }
  return true;
}

bool sdet_vehicle_finder::
n_regions_closest_to_pick(vcl_vector<sdet_region_sptr> const& regions,
                          const int n,
                          vcl_vector<sdet_region_sptr> & n_regions)
{
  n_regions.clear();
  for (int i = 0; i<n; i++)
  {
    float dmin = -1.0f;
    sdet_region_sptr reg;
    for (vcl_vector<sdet_region_sptr>::const_iterator rit = regions.begin();
         rit != regions.end(); rit++)
    {
      bool found = false;
      for (vcl_vector<sdet_region_sptr>::iterator nit = n_regions.begin();
          nit != n_regions.end()&&!found; nit++)
        if (*nit == *rit)
          found = true;
      if (found)
        continue;
      float x0 = (*rit)->Xo(), y0 = (*rit)->Yo();
      int xp = pick_.x(), yp = pick_.y();
      float d = (float)vcl_sqrt((x0-xp)*(x0-xp) + (y0-yp)*(y0-yp));
      double area = (*rit)->area();
      float max_d = distance_scale_*vcl_sqrt(area);
#ifdef DEBUG
      vcl_cout << "d = " << d << '\n'
               << "max_d = " << max_d << '\n';
#endif
      if (d>max_d)
        continue;
      if (dmin<0 || d<dmin)
      {
        dmin = d;
        reg = *rit;
      }
    }
    if (!reg)
      return false;
    //first distance
    n_regions.push_back(reg);
  }
  return true;
}

bool sdet_vehicle_finder::detect_shadow_regions()
{
   if (!image_)
    return false;
  shadow_regions_.clear();
  sdet_watershed_region_proc wrp(wrpp_);
  wrp.set_image(image_, search_box_);
  wrp.extract_regions();
  wrp.merge_priority_ = sdet_watershed_region_proc::DARK;
  wrp.merge_regions();
  if (verbose_)
  {
    wrp.print_region_info();
    vcl_cout << "shadow regions\n";
  }
  //get the regions that are potential shadows
  vcl_vector<sdet_region_sptr>& regions = wrp.regions();
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
    sdet_region_sptr r = *rit;
    if (r->Io()<shadow_thresh_&&r->Npix()>=wrp.min_area_)
    {
      if (verbose_)
        vcl_cout << "Sreg(Np:" << r->Npix() << " Io:"
                 << r->Io() << " Xo:" << r->Xo()
                 << " Yo:" << r->Yo() << ")\n" << vcl_flush;
      shadow_regions_.push_back(r);
    }
  }
#if 0
  //find the two shadow regions closest to the pick
  vcl_vector<sdet_region_sptr> close_shadows;
  if (!this->n_regions_closest_to_pick(shadow_regions_, 2, close_shadows))
     return false;
  //compute the union of their bounding boxes
  if (!this->region_box_union(close_shadows, shadow_box_))
    return false;
#endif // 0
  int n_nearby = 1;
  vcl_vector<sdet_region_sptr> closest_shadow_regions;
  if (!this->n_regions_closest_to_pick(shadow_regions_, n_nearby,
                                       closest_shadow_regions))
    return false;
  vcl_vector<vsol_polygon_2d_sptr> shadow_boundaries;
  for (int i = 0; i<n_nearby; i++)
  {
    sdet_region_sptr cs = closest_shadow_regions[i];
    if (verbose_)
      vcl_cout << "Closest Shadow Region(Np:" << cs->Npix() << " Io:"
               << cs->Io() << " Xo:" << cs->Xo()
               << " Yo:" << cs->Yo() << ")\n" << vcl_flush;
    shadow_boundaries.push_back(cs->boundary());
  }
  if (!bsol_algs::hull_of_poly_set(shadow_boundaries, shadow_hull_))
    return false;
  return true;
}

bool sdet_vehicle_finder::detect_para_regions()
{
  if (!image_)
    return false;
  para_regions_.clear();
  //compute parallel coverage on whole image (for now)
  brip_para_cvrg pc(pcp_);
  pc.do_coverage(image_);
  vil1_memory_image_of<float> pimage = pc.get_float_detection_image();
  sdet_watershed_region_proc wrp(wrpp_);
  wrp.set_image(pimage, search_box_);
  wrp.extract_regions();
  wrp.merge_priority_ = sdet_watershed_region_proc::LIGHT;
  wrp.merge_regions();
  if (verbose_)
  {
    vcl_cout << "\n\nparallel coverage regions\n";
    wrp.print_region_info();
  }
  vcl_vector<sdet_region_sptr>& regions = wrp.regions();

  //get the regions that are potential para coverage regions
  for (vcl_vector<sdet_region_sptr>::iterator rit = regions.begin();
       rit != regions.end(); rit++)
  {
      sdet_region_sptr r = *rit;
      if (r->Io()>para_thresh_&&r->Npix()>=wrp.min_area_)
      {
        if (verbose_)
          vcl_cout << "Preg(Np:" << r->Npix() << " Io:"
                   << r->Io() << " Xo:" << r->Xo()
                   << " Yo:" << r->Yo() << ")\n" << vcl_flush;
        para_regions_.push_back(r);
      }
  }

  int n_nearby = 2;
  vcl_vector<sdet_region_sptr> closest_para_regions;
  this->n_regions_closest_to_pick(para_regions_, n_nearby,
                                  closest_para_regions);
  if (closest_para_regions.size()<1)
    return false;
  vcl_vector<vsol_polygon_2d_sptr> para_boundaries;
  for (unsigned int i = 0; i<closest_para_regions.size(); ++i)
  {
    sdet_region_sptr cs = closest_para_regions[i];
    if (verbose_)
      vcl_cout << "Closest Para Region(Np:" << cs->Npix() << " Io:"
               << cs->Io() << " Xo:" << cs->Xo()
               << " Yo:" << cs->Yo() << ")\n" << vcl_flush;
    para_boundaries.push_back(cs->boundary());
  }
  if (!bsol_algs::hull_of_poly_set(para_boundaries, para_hull_))
    return false;

  return true;
}

//: compute the bounding polygon for the para region and shadow region.
// Corresponds to the convex hull
bool sdet_vehicle_finder::compute_track_boundary()
{
  if (!shadow_hull_||!para_hull_)
    return false;
  vcl_vector<vsol_polygon_2d_sptr> polys;
  polys.push_back(shadow_hull_);  polys.push_back(para_hull_);
  if (!bsol_algs::hull_of_poly_set(polys, vehicle_track_poly_))
    return false;
  return true;
}
