#ifndef sdet_vehicle_finder_h_
#define sdet_vehicle_finder_h_
//---------------------------------------------------------------------
//:
// \file
// \brief a processor for finding vehicles
//
// \author
//  J.L. Mundy - July 11, 2004
//
// \verbatim
// Modifications
//  none
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vil1/vil1_image.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <brip/brip_roi_sptr.h>
#include <sdet/sdet_region_sptr.h>
#include <sdet/sdet_vehicle_finder_params.h>

class sdet_vehicle_finder : public sdet_vehicle_finder_params
{
 public:
  //Constructors/destructor
  sdet_vehicle_finder(sdet_vehicle_finder_params& wrpp);

  ~sdet_vehicle_finder();
  //Accessors

  void set_image(vil1_image& image){image_ = image;}

  void set_pick(const int x, const int y);

  vsol_box_2d_sptr search_box(){return search_box_;}
  void clear();
  vcl_vector<sdet_region_sptr>& shadow_regions(){return shadow_regions_;}

  vsol_polygon_2d_sptr shadow_hull(){return shadow_hull_;}
  vsol_polygon_2d_sptr para_hull(){return para_hull_;}
  vsol_polygon_2d_sptr vehicle_track_poly(){return vehicle_track_poly_;}
  //Utility Methods
  bool detect_shadow_regions();
  bool detect_para_regions();
  bool compute_track_boundary();
  //Debug methods

 protected:
  sdet_vehicle_finder();//do not use
  //protected methods
  void construct_search_box();
  bool n_regions_closest_to_pick(vcl_vector<sdet_region_sptr> const& regions,
                                 const int n,
                                 vcl_vector<sdet_region_sptr> & n_regions);

  bool region_box_union(vcl_vector<sdet_region_sptr> const& regions,
                        vsol_box_2d_sptr& box_union);

  //members
  vil1_image image_;  //input image
  vgl_point_2d<int> pick_; //user's pick position
  vsol_box_2d_sptr search_box_;
  vsol_polygon_2d_sptr shadow_hull_;
  vsol_polygon_2d_sptr para_hull_;
  vsol_polygon_2d_sptr vehicle_track_poly_;
  bool shadows_valid_;      //process state flag
  vil1_image overlay_image_; // for display
  vcl_vector<sdet_region_sptr> shadow_regions_; //shadow regions near pick
  vcl_vector<sdet_region_sptr> para_regions_; //para regions near pick
};

#endif // sdet_vehicle_finder_h_
