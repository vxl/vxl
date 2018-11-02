// This is brl/bbas/volm/volm_osm_parser.h
#ifndef volm_osm_parser_h_
#define volm_osm_parser_h_
//:
// \file
// \brief A parser to read the open street map data from xml file
// The format of open street map file (osm) follows general xml data format, which are well defined in http://wiki.openstreetmap.org/wiki/Elements
// Units are in lat, lon
// \author Yi Dong
// \date July 22, 2013
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
#include <expatpplib.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>

// Tags for OSM
#define OSM_NODE "node"
#define OSM_WAY "way"
#define OSM_RELATION "relation"
#define OSM_TAG "tag"
#define OSM_WAY_ND "nd"
#define OSM_RELATION_MEM "member"
#define OSM_BOUND "bounds"

class volm_osm_parser : public expatpp
{
public:
  //: default constructor
  volm_osm_parser(void);
  //: destructor
  ~volm_osm_parser(void) override = default;

  //: parse meaningful location points from open street map (point with defined keys), return loc points are in x = lon and y = lat
  static void parse_points(std::vector<vgl_point_2d<double> >& points,
                           std::vector<std::vector<std::pair<std::string, std::string> > >& keys,
                           std::string const& osm_file);

  //: parse bounding box of the give osm file
  static vgl_box_2d<double> parse_bbox(std::string const& osm_file);

  //: parse 3d location points from osm
  static void parse_points_3d(std::vector<vgl_point_3d<double> >& points,
                              std::string const& osm_file);

  //: parse polylines from open street map (line can be ways, return lines with points having x = lon and y = lat
  static void parse_lines(std::vector<std::vector<vgl_point_2d<double> > >& lines,
                          std::vector<std::vector<std::pair<std::string, std::string> > >& keys,
                          std::string const& osm_file);

  //: parse polygons from open street map (polygon can be from ways and relation elements)
  static void parse_polygons(std::vector<vgl_polygon<double> >& polys,
                             std::vector<std::vector<std::pair<std::string, std::string> > >& keys,
                             std::string const& osm_file);

  //: node list from osm  (key is the node id and the point is arranged as x = lon, y = lat)
  std::map<unsigned long long, vgl_point_2d<double> > nodes_;
  std::map<unsigned long long, std::vector<std::pair<std::string, std::string> > > node_keys_;
  std::map<unsigned long long, vgl_point_3d<double> > nodes_3d_;

  //: way list from osm
  std::map<unsigned long long, std::vector<unsigned long long> > ways_;
  std::map<unsigned long long, std::vector<std::pair<std::string, std::string> > > way_keys_;

  //: relation list from osm
  std::map<unsigned long long, std::vector<std::pair<std::string, unsigned long long> > > relations_;
  std::map<unsigned long long, std::vector<std::pair<std::string, std::string> > > relation_keys_;
  std::map<unsigned long long, std::string> relation_types_;

  //: boundary of current open street map
  vgl_box_2d<double> bbox_;

private:

  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* name) override {}
  void charData(const XML_Char* s, int len) override {}

  void init_params();

  //: element parser
  std::pair<std::string, unsigned long long> parent_;

};

#endif // volm_osm_parser_h_
