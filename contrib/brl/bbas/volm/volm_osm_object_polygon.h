// This is brl/bbas/volm/volm_osm_object_polygon.h
#ifndef volm_osm_object_polygon_h_
#define volm_osm_object_polygon_h_
//:
// \file
// \brief A class to represent a region obtained from open street map
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
#include <vgl/vgl_polygon.h>

class volm_osm_object_polygon;
typedef vbl_smart_ptr<volm_osm_object_polygon> volm_osm_object_polygon_sptr;

class volm_osm_object_polygon : public vbl_ref_count
{
public:
  //: default constructor
  volm_osm_object_polygon() : prop_(0, "invalid", 0, 0.0, vil_rgb<vxl_byte>(0,0,0)) {}

  //: constructor
  volm_osm_object_polygon(volm_land_layer prop, vgl_polygon<double> poly) : prop_(prop), poly_(poly) {}

  //: accessors
  vgl_polygon<double> poly() const { return poly_; }
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
  vgl_polygon<double> poly_;
};

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_polygon* rptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_polygon*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_polygon_sptr& sptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_polygon_sptr& sptr);

#endif // volm_osm_object_polygon_h_
