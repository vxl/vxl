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

#include <expatpplib.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_box_2d.h>
#include <vcl_vector.h>
#include <vcl_utility.h>

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
  ~volm_osm_parser(void) {}

  //: parse meaningful location points from open street map (point with defined keys), return loc points are in x = lon and y = lat
  static void parse_points(vcl_vector<vgl_point_2d<double> >& points,
                           vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                           vcl_string const& osm_file);

  //: parse bounding box of the give osm file
  static vgl_box_2d<double> parse_bbox(vcl_string const& osm_file);

  //: parse 3d location points from osm
  static void parse_points_3d(vcl_vector<vgl_point_3d<double> >& points,
                              vcl_string const& osm_file);

  //: parse polylines from open street map (line can be ways, return lines with points having x = lon and y = lat
  static void parse_lines(vcl_vector<vcl_vector<vgl_point_2d<double> > >& lines,
                          vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                          vcl_string const& osm_file);

  //: parse polygons from open street map (polygon can be from ways and relation elements)
  static void parse_polygons(vcl_vector<vgl_polygon<double> >& polys,
                             vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                             vcl_string const& osm_file);

  //: node list from osm  (key is the node id and the point is arranged as x = lon, y = lat)
  vcl_map<unsigned long long, vgl_point_2d<double> > nodes_;
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > node_keys_;
  vcl_map<unsigned long long, vgl_point_3d<double> > nodes_3d_;

  //: way list from osm
  vcl_map<unsigned long long, vcl_vector<unsigned long long> > ways_;
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > way_keys_;

  //: relation list from osm
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, unsigned long long> > > relations_;
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > relation_keys_;
  vcl_map<unsigned long long, vcl_string> relation_types_;

  //: boundary of current open street map
  vgl_box_2d<double> bbox_;

private:

  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name) {}
  virtual void charData(const XML_Char* s, int len) {}

  void init_params();

  //: element parser
  vcl_pair<vcl_string, unsigned long long> parent_;

};

#endif // volm_osm_parser_h_