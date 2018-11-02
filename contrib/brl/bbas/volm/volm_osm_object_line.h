// This is brl/bbas/volm/volm_osm_object_line.h
#ifndef volm_osm_object_line_h_
#define volm_osm_object_line_h_
//:
// \file
// \brief  A class to represent a line object obtained from open street map
//
// \author Yi Dong
// \date August 06, 2013
//
// \berbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <utility>
#include <vector>
#include "volm_category_io.h"
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class volm_osm_object_line;
typedef vbl_smart_ptr<volm_osm_object_line> volm_osm_object_line_sptr;

class volm_osm_object_line : public vbl_ref_count
{
public:
  //: default constructor
  volm_osm_object_line() : prop_(0, "invalid", 0, 0.0, vil_rgb<vxl_byte>(0,0,0)) { line_.clear(); }

  //: constructor
  volm_osm_object_line(volm_land_layer prop, std::vector<vgl_point_2d<double> > line) : prop_(prop), line_(std::move(line)) {}

  //: accessors
  std::vector<vgl_point_2d<double> >& line() { return line_; }
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
  std::vector<vgl_point_2d<double> > line_;
};

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_line* rptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_line*& rptr);

void vsl_b_write(vsl_b_ostream& os, const volm_osm_object_line_sptr& sptr);

void vsl_b_read(vsl_b_istream& is, volm_osm_object_line_sptr& sptr);

#endif // volm_osm_object_line_h_
