// This is brl/bbas/volm/volm_osm_objects.h
#ifndef volm_osm_objects_h_
#define volm_osm_objects_h_
//:
// \file
// \brief  A class to contain all objects defined in an oepn street map file
//
// \author Yi Dong
// \date August 06, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <utility>
#include <vector>
#include "volm_category_io.h"
#include "volm_osm_parser.h"
#include "volm_osm_object_point.h"
#include "volm_osm_object_line.h"
#include "volm_osm_object_polygon.h"
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class volm_osm_objects
{
public:
  //: default constructor
  volm_osm_objects() = default;

  //: create volm_osm_objects from open street map file
  volm_osm_objects(std::string const& osm_file, std::string const& osm_to_volm_file);

  //: create volm_osm_objects from binary file
  volm_osm_objects(std::string const& bin_file_name);

  //: create volm_osm_objects from multiple volm_locs, lines and regions
  volm_osm_objects(std::vector<volm_osm_object_point_sptr>  loc_pts,
                   std::vector<volm_osm_object_line_sptr>  loc_lines,
                   std::vector<volm_osm_object_polygon_sptr>  loc_polys)
    : loc_pts_(std::move(loc_pts)), loc_lines_(std::move(loc_lines)), loc_polys_(std::move(loc_polys)) {}

  //: write all volm_osm_object into binary file
  bool write_osm_objects(std::string const& bin_file);

  //: asscessors
  std::vector<volm_osm_object_point_sptr>&     loc_pts() { return loc_pts_;   }
  std::vector<volm_osm_object_line_sptr>&    loc_lines() { return loc_lines_; }
  std::vector<volm_osm_object_polygon_sptr>& loc_polys() { return loc_polys_; }

  //: number of location points
  unsigned num_locs()    const { return (unsigned)loc_pts_.size();   }
  unsigned num_roads()   const { return (unsigned)loc_lines_.size(); }
  unsigned num_regions() const { return (unsigned)loc_polys_.size(); }

  //: write location points to kml
  bool write_pts_to_kml(std::string const& kml_file);

  //: write lines to kml
  bool write_lines_to_kml(std::string const& kml_file);

  //: write regions to kml
  bool write_polys_to_kml(std::string const& kml_file);


  // ===========  binary I/O ================

  //: version
  short version() const { return 1; }

  //: binary IO write
  void b_write(vsl_b_ostream& os);

  //: binary IO read
  void b_read(vsl_b_istream& is);

private:
  std::vector<volm_osm_object_point_sptr>     loc_pts_;
  std::vector<volm_osm_object_line_sptr>    loc_lines_;
  std::vector<volm_osm_object_polygon_sptr> loc_polys_;

};


class volm_osm_object_ids;
typedef vbl_smart_ptr<volm_osm_object_ids> volm_osm_object_ids_sptr;

class volm_osm_object_ids : public vbl_ref_count
{
public:

  //: default constructor
  volm_osm_object_ids() {pt_ids_.clear(); line_ids_.clear(); region_ids_.clear(); }

  //: constuctor
  volm_osm_object_ids(std::vector<unsigned>  pt_ids, std::vector<unsigned>  line_ids, std::vector<unsigned>  region_ids)
    : pt_ids_(std::move(pt_ids)), line_ids_(std::move(line_ids)), region_ids_(std::move(region_ids)) {}

  //: construct by reading from a binary file
  volm_osm_object_ids(std::string const& bin_file);

  //: accessors
  std::vector<unsigned>&     pt_ids() { return pt_ids_;     }
  std::vector<unsigned>&   line_ids() { return line_ids_;   }
  std::vector<unsigned>& region_ids() { return region_ids_; }

  unsigned num_pts()     { return (unsigned)pt_ids_.size(); }
  unsigned num_lines()   { return (unsigned)line_ids_.size(); }
  unsigned num_regions() { return (unsigned)region_ids_.size(); }

  //: add a location point
  void add_pt(unsigned const& pt_id);
  void add_line(unsigned const& line_id);
  void add_region(unsigned const& region_id);

  bool is_empty()
  {
    return (pt_ids_.empty() && line_ids_.empty() && region_ids_.empty());
  }

  //: binary io
  bool write_osm_ids(std::string const& bin_file);

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 1; }

private:
  std::vector<unsigned> pt_ids_;
  std::vector<unsigned> line_ids_;
  std::vector<unsigned> region_ids_;
};

#endif // volm_osm_objects_h_
