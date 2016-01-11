#include "volm_osm_parser.h"
//:
// \file
#include <vcl_sstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_cstddef.h>
#include <vcl_cassert.h>

template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream str(t);
  str >> d;
}

void convert_string(const char* t, vcl_string& d)
{
  vcl_stringstream strstream(t);
  d = strstream.str();
}

bool is_line(vcl_vector<unsigned long long> ids)
{
  return *(ids.begin()) != *(ids.end()-1);
}

volm_osm_parser::volm_osm_parser()
{
  init_params();
}

void volm_osm_parser::init_params()
{
}

void volm_osm_parser::startElement(const XML_Char* name, const XML_Char** atts)
{
  // BOUNDARY OF CURRENT OSM FILE
  if (vcl_strcmp(name, OSM_BOUND) == 0) {
    if (atts) {
      double min_lon, min_lat, max_lon, max_lat;
      for (unsigned i = 0; atts[i]; i+=2) {
        if (vcl_strcmp(atts[i], "minlat") == 0)
          convert(atts[i+1], min_lat);
        else if (vcl_strcmp(atts[i], "minlon") == 0)
          convert(atts[i+1], min_lon);
        else if (vcl_strcmp(atts[i], "maxlat") == 0)
          convert(atts[i+1], max_lat);
        else if (vcl_strcmp(atts[i], "maxlon") == 0)
          convert(atts[i+1], max_lon);
      }
      bbox_.set_min_point(vgl_point_2d<double>(min_lon, min_lat));
      bbox_.set_max_point(vgl_point_2d<double>(max_lon, max_lat));
    }
  }
  // NODE
  if (vcl_strcmp(name, OSM_NODE) == 0) {
    if (atts) {
      unsigned long long node_id;
      double lat, lon;
      double alt = 0.0;
      for (unsigned i = 0; atts[i]; i+=2) {
        if (vcl_strcmp(atts[i], "id") == 0)
          convert(atts[i+1], node_id);
        else if (vcl_strcmp(atts[i], "lat") == 0)
          convert(atts[i+1], lat);
        else if (vcl_strcmp(atts[i], "lon") == 0)
          convert(atts[i+1], lon);
        else if (vcl_strcmp(atts[i], "alt") == 0)
          convert(atts[i+1], alt);
      }
      parent_.first = OSM_NODE;
      parent_.second = node_id;
      nodes_.insert(vcl_pair<unsigned long long, vgl_point_2d<double> >(node_id, vgl_point_2d<double>(lon, lat)));
      vcl_pair<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > node_key;
      node_key.first = node_id;
      vcl_vector<vcl_pair<vcl_string, vcl_string> > keys;  keys.clear();
      node_key.second = keys;
      node_keys_.insert(node_key);
      // insert 3d points
      nodes_3d_.insert(vcl_pair<unsigned long long, vgl_point_3d<double> >(node_id, vgl_point_3d<double>(lon, lat, alt)));
    }
  }

  // WAY
  if (vcl_strcmp(name, OSM_WAY) == 0) {
    if (atts) {
      unsigned long long way_id;
      for (unsigned i = 0; atts[i]; i+=2)
        if (vcl_strcmp(atts[i], "id") == 0)
          convert(atts[i+1], way_id);
      parent_.first = OSM_WAY;
      parent_.second = way_id;
      vcl_pair<unsigned long long, vcl_vector<unsigned long long> > way;
      way.first = way_id;
      vcl_vector<unsigned long long> nd_list;  nd_list.clear();
      way.second = nd_list;
      ways_.insert(way);
      vcl_pair<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > way_key;
      way_key.first = way_id;
      vcl_vector<vcl_pair<vcl_string, vcl_string> > keys;  keys.clear();
      way_key.second = keys;
      way_keys_.insert(way_key);
    }
  }

  // RELATION
  if (vcl_strcmp(name, OSM_RELATION) == 0) {
    if (atts) {
      unsigned long long relation_id;
      for (unsigned i = 0; atts[i]; i+=2)
        if (vcl_strcmp(atts[i], "id") == 0)
          convert(atts[i+1], relation_id);
      parent_.first = OSM_RELATION;
      parent_.second = relation_id;
      vcl_pair<unsigned long long, vcl_vector<vcl_pair<vcl_string, unsigned long long> > > relation;
      relation.first = relation_id;
      vcl_vector<vcl_pair<vcl_string, unsigned long long> > ref_list;  ref_list.clear();
      relation.second = ref_list;
      relations_.insert(relation);
      vcl_pair<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > relation_key;
      vcl_vector<vcl_pair<vcl_string, vcl_string> > keys;  keys.clear();
      relation_key.first = relation_id;
      relation_key.second = keys;
      relation_keys_.insert(relation_key);
    }
  }

  // WAY node list
  if (vcl_strcmp(name, OSM_WAY_ND) == 0) {
    if (atts && parent_.first == OSM_WAY) {
      unsigned long long ref_id;
      for (unsigned i = 0; atts[i]; i+=2)
        if (vcl_strcmp(atts[i], "ref") == 0)
          convert(atts[i+1], ref_id);
      ways_[parent_.second].push_back(ref_id);
    }
  }

  // RELATION reference memeber
  if (vcl_strcmp(name, OSM_RELATION_MEM) == 0) {
    if (atts && parent_.first == OSM_RELATION) {
      unsigned mem_id; vcl_string mem_type;
      for (unsigned i = 0; atts[i]; i+=2) {
        if (vcl_strcmp(atts[i], "type") == 0)
          convert(atts[i+1], mem_type);
        else if (vcl_strcmp(atts[i], "ref") == 0)
          convert(atts[i+1], mem_id);
      }
      relations_[parent_.second].push_back(vcl_pair<vcl_string, unsigned long long>(mem_type, mem_id));
    }
  }

  // KEY tag for parent node, way or relation
  if (vcl_strcmp(name, OSM_TAG) == 0) {
    if (atts) {
      vcl_string key, value;
      for (unsigned i = 0; atts[i]; i+=2) {
        if (vcl_strcmp(atts[i], "k") == 0)
          convert_string(atts[i+1], key);
        else if (vcl_strcmp(atts[i], "v") == 0)
          convert_string(atts[i+1], value);
      }
      vcl_pair<vcl_string, vcl_string> pair(key, value);
      if (parent_.first == OSM_NODE)
        node_keys_[parent_.second].push_back(pair);
      else if (parent_.first == OSM_WAY)
        way_keys_[parent_.second].push_back(pair);
      else if (parent_.first == OSM_RELATION && key == "type")
        relation_types_.insert(vcl_pair<unsigned long long, vcl_string>(parent_.second, value));
      else if (parent_.first == OSM_RELATION)
        relation_keys_[parent_.second].push_back(pair);
      else
        vcl_cerr << " WARNNING: unknown tag detected when parsing osm file " << vcl_endl;
    }
  }
}

vgl_box_2d<double> volm_osm_parser::parse_bbox(vcl_string const& osm_file)
{
  volm_osm_parser* parser = new volm_osm_parser();
  vcl_FILE* xmlFile = vcl_fopen(osm_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << " can not find osm file to parse: " << osm_file << '\n';
    delete parser;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line " << parser->XML_GetCurrentColumnNumber() << '\n';
    delete parser;
  }
  return parser->bbox_;
}

void volm_osm_parser::parse_points(vcl_vector<vgl_point_2d<double> >& points,
                                   vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                                   vcl_string const& osm_file)
{
  volm_osm_parser* parser = new volm_osm_parser();
  vcl_FILE* xmlFile = vcl_fopen(osm_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << " can not find osm file to parse: " << osm_file << '\n';
    delete parser;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line " << parser->XML_GetCurrentColumnNumber() << '\n';
    delete parser;
  }
  // fetch points that have tags
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > >::iterator mit = parser->node_keys_.begin();
  for (; mit != parser->node_keys_.end(); ++mit) {
    if ( mit->second.empty())
      continue;
    unsigned long long node_id = mit->first;
    //assert(parser->bbox_.contains(parser->nodes_[node_id]) && "the node in osm in outside bounding box");
    points.push_back(parser->nodes_[node_id]);
    keys.push_back(mit->second);
  }
  delete parser;
}

void volm_osm_parser::parse_points_3d(vcl_vector<vgl_point_3d<double> >& points,
                                      vcl_string const& osm_file)
{
  volm_osm_parser* parser = new volm_osm_parser();
  vcl_FILE* xmlFile = vcl_fopen(osm_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << " can not find osm file to parse: " << osm_file << '\n';
    delete parser;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line " << parser->XML_GetCurrentColumnNumber() << '\n';
    delete parser;
  }
  // parse all points
  vcl_map<unsigned long long, vgl_point_3d<double> >::iterator mit = parser->nodes_3d_.begin();
  for (; mit != parser->nodes_3d_.end(); ++mit) {
    points.push_back(mit->second);
  }
}

void volm_osm_parser::parse_lines(vcl_vector<vcl_vector<vgl_point_2d<double> > >& lines,
                                  vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                                  vcl_string const& osm_file)
{
  volm_osm_parser* parser = new volm_osm_parser();
  vcl_FILE* xmlFile = vcl_fopen(osm_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << " can not find osm file to parse: " << osm_file << '\n';
    delete parser;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line " << parser->XML_GetCurrentColumnNumber() << '\n';
    delete parser;
  }

  vcl_map<unsigned long long, vgl_point_2d<double> > nodes = parser->nodes_;
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > >::iterator mit = parser->way_keys_.begin();

  // retrieve ways that have tags
  for (; mit != parser->way_keys_.end(); ++mit) {
    if (mit->second.empty())
      continue;
    unsigned long long way_id = mit->first;
    vcl_vector<unsigned long long> node_ids = parser->ways_[way_id];
    if (is_line(node_ids) && node_ids.size() > 1) {
      vcl_vector<vgl_point_2d<double> > line;
      for (vcl_vector<unsigned long long>::iterator vit = node_ids.begin(); vit != node_ids.end(); ++vit) {
//        assert(parser->bbox_.contains(nodes[*vit]) && "the node in osm in outside bounding box");
        if (nodes.find(*vit) == nodes.end())
          continue;
        line.push_back(nodes[*vit]);
      }
      lines.push_back(line);
      keys.push_back(mit->second);
    }
  }
  delete parser;
}

vcl_vector<unsigned long long> get_line_from_way_id(unsigned long long const& way_id,
                                                    vcl_vector<vcl_pair<unsigned long long, vcl_vector<unsigned long long> > >& ways)
{
  vcl_vector<unsigned long long> out;
  out.clear();
  vcl_vector<vcl_pair<unsigned long long, vcl_vector<unsigned long long> > >::iterator vit = ways.begin();
  for (; vit != ways.end(); ++vit) {
    if ( vit->first == way_id )
      return vit->second;
  }
  return out;
}

bool compose_polygon_from_relation(vgl_box_2d<double> const& osm_bbox,
                                   vcl_map<unsigned long long, vgl_point_2d<double> >& nodes,
                                   vcl_vector<vcl_pair<unsigned long long, vcl_vector<unsigned long long> > > ways,
                                   vcl_vector<unsigned long long>& way_ids,
                                   vgl_polygon<double>& poly)
{
  vcl_vector<unsigned long long>::iterator vit = way_ids.begin();

  // check whether the ways are enclosed when there is only one way
  if (way_ids.size() == 1) {
    vcl_vector<unsigned long long> curr_line = get_line_from_way_id(way_ids[0], ways);
    if (curr_line[0] != curr_line[curr_line.size()-1])
      return false;
  }

  // loop over all ways to compose polygon sheets
  while ( vit != way_ids.end() ) {
    unsigned long long curr_way_id = *vit;
    // load current way
    vcl_vector<unsigned long long> curr_line = get_line_from_way_id(curr_way_id, ways);
    //vcl_vector<unsigned long long> curr_line = ways[curr_way_id];
    vcl_vector<unsigned long long> sheet;
    if (!is_line(curr_line)) {
      for (unsigned n_idx = 0; n_idx < curr_line.size()-1; n_idx++)
        sheet.push_back(curr_line[n_idx]);
    }
    else if (vit == way_ids.end()-1) {
      ++vit;
      continue;
    }
    else {  // search other ways to compose a enclose sheet
      for (unsigned n_idx = 0; n_idx < curr_line.size(); n_idx++)
        sheet.push_back(curr_line[n_idx]);
      // compose the sheet
      unsigned long long p1 = *(curr_line.begin());
      unsigned long long p2 = *(curr_line.end()-1);
      unsigned long long start = p1;
      unsigned long long end = p2;
      bool finish = (start == end);
      do {
        ++vit;
        unsigned long long next_way_id = *vit;
        // get the end-point from next way
        vcl_vector<unsigned long long> next_line = get_line_from_way_id(next_way_id, ways);
        unsigned long long p3 = *(next_line.begin());
        unsigned long long p4 = *(next_line.end()-1);

        // connect to next line segment
        if (start == p3) {
          if (end == p4) {
            end = p3;
            for (int n_idx = next_line.size()-1; n_idx > 0; n_idx--)
              sheet.push_back(next_line[n_idx]);
          }
          else {
            start = p4;
            for (unsigned n_idx = 1; n_idx < next_line.size(); n_idx++) {
              vcl_vector<unsigned long long>::iterator vit = sheet.begin();
              sheet.insert(vit, next_line[n_idx]);
            }
          }
        }
        else if (start == p4) {
          if (end == p3) {
            end = p4;
            for (unsigned n_idx = 1; n_idx < next_line.size()-1; n_idx++)
              sheet.push_back(next_line[n_idx]);
          }
          else {
            start = p3;
            for (int n_idx = next_line.size()-1; n_idx >= 0; n_idx--) {
              vcl_vector<unsigned long long>::iterator vit = sheet.begin();
              sheet.insert(vit, next_line[n_idx]);
            }
          }
          /*if (end == p3) {
            end = p4;
            for (unsigned n_idx = 1; n_idx < next_line.size()-1; n_idx++)
              sheet.push_back(next_line[n_idx]);
          }
          else {
            end = p3;
            for (int n_idx = next_line.size()-1; n_idx >= 0; n_idx--)
              sheet.push_back(next_line[n_idx]);
          }*/
        }
        else if (end == p3) {
          if (start == p4) {
            end = p4;
            for (unsigned n_idx = 1; n_idx < next_line.size()-1; n_idx++)
              sheet.push_back(next_line[n_idx]);
          }
          else {
            end = p4;
            for (unsigned n_idx = 1; n_idx < next_line.size(); n_idx++)
              sheet.push_back(next_line[n_idx]);
          }
        }
        else if (end == p4) {
          if (start == p3) {
            end = p3;
            for (int n_idx = next_line.size()-2; n_idx > 0; n_idx--)
              sheet.push_back(next_line[n_idx]);
          }
          else {
            end = p3;
            for (int n_idx = next_line.size()-2; n_idx >=0; n_idx--)
              sheet.push_back(next_line[n_idx]);
          }
        }
        finish = (start==end);
      } while( vit != (way_ids.end()-1) && !finish);
      // has to be finished otherwise there is a boundary defined in the relation is not enclosed
      if (!finish)
        return false;
    }
    // current sheet is an enclosed line segments, put it into polygon
    poly.new_sheet();
    for (unsigned n_idx = 0; n_idx < sheet.size(); n_idx++) {
      //assert(osm_bbox.contains(nodes[sheet[n_idx]]) && "the node in osm in outside bounding box");
      if (nodes.find(sheet[n_idx]) == nodes.end())
        return false;
      poly.push_back(nodes[sheet[n_idx]]);
    }
    // update to the next line segment
    ++vit;
  }
  return true;
}

void volm_osm_parser::parse_polygons(vcl_vector<vgl_polygon<double> >& polys,
                                     vcl_vector<vcl_vector<vcl_pair<vcl_string, vcl_string> > >& keys,
                                     vcl_string const& osm_file)
{
  volm_osm_parser* parser = new volm_osm_parser();
  vcl_FILE* xmlFile = vcl_fopen(osm_file.c_str(), "r");
  if (!xmlFile) {
    vcl_cerr << " can not find osm file to parse: " << osm_file << '\n';
    delete parser;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line " << parser->XML_GetCurrentColumnNumber() << '\n';
    delete parser;
  }

  vcl_map<unsigned long long, vgl_point_2d<double> > nodes = parser->nodes_;
  //vcl_map<unsigned long long, vcl_vector<unsigned long long> > ways = parser->ways_;
  vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > > way_keys = parser->way_keys_;
  vcl_map<unsigned long long, vcl_string > relation_type = parser->relation_types_;

  // retrieve polygons from ways that have tags
  for (vcl_map<unsigned long long, vcl_vector<vcl_pair<vcl_string, vcl_string> > >::iterator mit = way_keys.begin();
       mit != way_keys.end(); ++mit)
  {
    if (mit->second.empty())
      continue;
    unsigned long long way_id = mit->first;
    vcl_vector<unsigned long long> node_ids = parser->ways_[way_id];
    // note that the way can either be a line or a enclosed polygon sheet
    if (!is_line(node_ids) && node_ids.size() > 2) {
      vgl_polygon<double> poly;
      poly.new_sheet();
      bool outside = false;
      for (unsigned n_idx = 0; (n_idx < node_ids.size()-1 && !outside); n_idx++) {
        if (nodes.find(node_ids[n_idx]) == nodes.end())
          outside = true;
        else poly.push_back(nodes[node_ids[n_idx]]);
      }
      if (!outside) {
        polys.push_back(poly);
        keys.push_back(mit->second);
      }
    }
  }

  // retrieve polygons from relation which have defined types (only multipolygon and boundary are considered)
  for (vcl_map<unsigned long long, vcl_string >::iterator mit = relation_type.begin(); mit != relation_type.end(); ++mit)
  {
    if (mit->second != "boundary" && mit->second != "multipolygon")
      continue;
    unsigned rel_id = mit->first;
    vcl_vector<unsigned long long> way_ids;
    vcl_vector<vcl_pair<vcl_string, unsigned long long> > rel_mem = parser->relations_[rel_id];
    // ignore the points member in relation
    for (unsigned m_idx = 0; m_idx < (unsigned)rel_mem.size(); m_idx++)
      if (rel_mem[m_idx].first == "way")
        way_ids.push_back(rel_mem[m_idx].second);
    // obtain the ways that belong to this relation (note the way may not exist in current osm and if one way misses, the relation is ignored)
    //vcl_map<unsigned long long, vcl_vector<unsigned long long> > ways;
    vcl_vector<vcl_pair<unsigned long long, vcl_vector<unsigned long long> > > ways;
    bool way_missing = false;
    for (unsigned w_idx = 0; (w_idx < (unsigned)way_ids.size() && !way_missing); w_idx++)
      way_missing = parser->ways_.find(way_ids[w_idx]) == parser->ways_.end();
    if (way_missing)
      continue;
    for (unsigned w_idx = 0; w_idx < (unsigned)way_ids.size(); w_idx++) {
      ways.push_back(vcl_pair<unsigned long long, vcl_vector<unsigned long long> >(way_ids[w_idx], parser->ways_[way_ids[w_idx]])) ;
    }
    /*for (unsigned w_idx = 0; w_idx < (unsigned)way_ids.size(); w_idx++)
      ways.insert(vcl_pair<unsigned long long, vcl_vector<unsigned long long> >(way_ids[w_idx], parser->ways_[way_ids[w_idx]]));*/
    vgl_polygon<double> poly;
    bool good_relation = compose_polygon_from_relation(parser->bbox_, nodes, ways, way_ids, poly);
    /*assert( good_relation && "creating polygon from relation failed");*/
    if (good_relation) {
      polys.push_back(poly);
      keys.push_back(parser->relation_keys_[rel_id]);
    }
  }

#if 0
  for (vcl_map<unsigned long long, vcl_vector<unsigned long long> >::iterator mit = ways.begin();
       mit != ways.end(); ++mit)
  {
    unsigned long long way_id;
    vcl_vector<unsigned long long> node_ids = mit->second;
    // check whether the way is a polyline (way can either be a polyline or a enclosed polygon sheet)
    if (!is_line(node_ids)) {
      vgl_polygon<double> poly;
      poly.new_sheet();
      for (unsigned n_idx = 0; n_idx < (unsigned)node_ids.size()-1; n_idx++)
        poly.push_back(nodes[node_ids[n_idx]]);
    }
  }
#endif
  // retrieve polygons from relations

}
