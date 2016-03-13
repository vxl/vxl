// This is brl/bbas/volm/volm_osm_object_point.h
#ifndef volm_osm_object_point_h_
#define volm_osm_object_point_h_
//:
// \file
// \brief  A class to represent a point location obtained from open street map
//
// \author Yi Dong
// \date August 06, 2013
//
// \berbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "volm_category_io.h"
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>

class volm_osm_object_point;
typedef vbl_smart_ptr<volm_osm_object_point> volm_osm_object_point_sptr;


class volm_osm_object_point : public vbl_ref_count
{
public:
  //: default constructor
  volm_osm_object_point() : prop_(0, "invalid", 0, 0.0, vil_rgb<vxl_byte>(0,0,0)), loc_(vgl_point_2d<double>(0.0,0.0)) {}

  ////: constructors
  volm_osm_object_point(volm_land_layer prop, vgl_point_2d<double> loc) : prop_(prop), loc_(loc) {}
  volm_osm_object_point(volm_land_layer prop, vgl_point_3d<double> loc) : prop_(prop), loc_(vgl_point_2d<double>(loc.x(), loc.y())) {}
  volm_osm_object_point(volm_land_layer prop, double lon, double lat) : prop_(prop), loc_(vgl_point_2d<double>(lon, lat)) {}

  //: accessors
  vgl_point_2d<double> loc() const { return loc_; }
  volm_land_layer prop() const { return prop_; }

  //: screen print
  void print();

  // ===========  binary I/O ================

  //: version
  unsigned version() const {return 1;}

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);


private:
  //: location propterties
  volm_land_layer prop_;

  //: coords of the point location in format of wgs84, x -- lon, y -- lat
  vgl_point_2d<double> loc_;
};

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_point* rptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_point*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_point_sptr& sptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_point_sptr& sptr);

#endif // volm_osm_object_point_h_
