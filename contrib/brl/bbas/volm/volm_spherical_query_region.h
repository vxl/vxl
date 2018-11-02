//This is brl/bbas/volm/volm_spherical_query_region.h
#ifndef volm_spherical_query_region_h_
#define volm_spherical_query_region_h_
//:
// \file
// \brief A class to store an axis-aligned spherical query region
// Units are in meters
//
// \author J.L. Mundy
// \date January 21, 2012
// \verbatim
//  Modifications
// None
// \endverbatim

#include <vector>
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/depth_map/depth_map_region.h>
#include <vsph/vsph_sph_box_2d.h>
#include <volm/volm_spherical_container_sptr.h>
class volm_spherical_query_region
{
 public:
  //: default constructor
  volm_spherical_query_region(): min_depth_(0), max_depth_(0), order_(0), nlcd_id_(0) {}


  void set_from_depth_map_region(vpgl_perspective_camera<double> const& cam,
                                 depth_map_region_sptr const& dm_region,
                                 volm_spherical_container_sptr const& sph_vol);
  bool inside(vsph_sph_point_2d const& sp) const{
    return box_.contains(sp);}

  //:accessors
  const vsph_sph_box_2d& bbox_ref() {return box_;}
  vsph_sph_box_2d& bbox() {return box_;}
  unsigned char orientation() const {return orientation_;}
  unsigned char min_depth() const {return min_depth_;}
  unsigned char max_depth() const {return max_depth_;}
  unsigned char order() const {return order_;}
  unsigned char nlcd_id() const {return nlcd_id_;}

  void print(std::ostream& os) const;

 private:
  //: box in spherical coordinates
  vsph_sph_box_2d box_;
  // region orientation code (see depth_map_region)
  unsigned char orientation_;
  // voxel depth indices from spherical volume container
  unsigned char min_depth_;
  unsigned char max_depth_;
  // region order
  unsigned char order_;
  // nlcd code
  unsigned char nlcd_id_;
};

#endif // volm_spherical_query_region_h_
