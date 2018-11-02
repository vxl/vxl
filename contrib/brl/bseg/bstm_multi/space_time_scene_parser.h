#ifndef bstm_multi_space_time_scene_parser_h_
#define bstm_multi_space_time_scene_parser_h_

//: \file space_time_scene_parser.h
//
// \brief space_time_scene_parser is a generic XML parser for 4D
// scenes. It is templated on block type, which specifies how it
// parses <block>...</block> tags in the XML files it reads. Thus, one
// can use this to parse scenes with different implementations for
// their space-time blocks. The Block type must have a Block::metadata
// type that provides a from_xml() method to create a metadata object
// from a given XML <block> tag.
//
// This is closely based on bstm/bstm_scene_parser.h.
//
// \author Raphael Kargon
// \date 31 Jul 2017

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <expatpplib.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_lvcs.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/bstm_multi_block_metadata.h>

template <typename Block> class space_time_scene_parser : public expatpp {

public:
  typedef typename Block::metadata_t block_metadata;

  space_time_scene_parser() : version_(1) {}
  ~space_time_scene_parser(void) override = default;

  // ACCESSORS for parser info
  bool lvcs(vpgl_lvcs &lvcs);
  vgl_point_3d<double> origin() const { return origin_; }
  std::string path() const { return path_; }
  std::string name() const { return name_; }
  std::map<bstm_block_id, block_metadata> blocks() { return blocks_; }
  std::vector<std::string> appearances() const { return appearances_; }
  int version() const { return version_; }

private:
  void startElement(const XML_Char *name, const XML_Char **atts) override;
  void endElement(const XML_Char * /*name*/) override {}
  void charData(const XML_Char * /*s*/, int /*len*/) override {}

  // lvcs temp values
  std::string lvcs_cs_name_;
  double lvcs_origin_lon_;
  double lvcs_origin_lat_;
  double lvcs_origin_elev_;
  double lvcs_lon_scale_;
  double lvcs_lat_scale_;
  std::string lvcs_XYZ_unit_;
  std::string lvcs_geo_angle_unit_;
  double lvcs_local_origin_x_;
  double lvcs_local_origin_y_;
  double lvcs_theta_;
  //: world origin
  vgl_point_3d<double> origin_;

  //: scene directory (path)
  std::string path_;

  //: scene name (string)
  std::string name_;

  //: block list
  std::map<bstm_block_id, block_metadata> blocks_;

  //: list of appearances
  std::vector<std::string> appearances_;

  int version_;
};

#endif // bstm_multi_space_time_scene_parser_h_
