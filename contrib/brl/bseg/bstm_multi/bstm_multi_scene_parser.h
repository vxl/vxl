#ifndef bstm_scene_parser_h_
#define bstm_scene_parser_h_

#include <expatpplib.h>

#include <vcl_compiler.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/vpgl_lvcs.h>

#include <bstm/basic/bstm_block_id.h>

class bstm_multi_scene_parser : public expatpp {
public:
  // ACCESSORS for parser info
  bool lvcs(vpgl_lvcs &lvcs);
  vgl_point_3d<double> origin() const { return origin_; }
  vcl_string path() const { return path_; }
  vcl_string name() const { return name_; }
  vcl_map<bstm_block_id, bstm_multi_block_metadata> blocks() { return blocks_; }
  vcl_vector<vcl_string> appearances() const { return appearances_; }
  int version() const { return version_; }

private:
  virtual void startElement(const XML_Char *name, const XML_Char **atts);
  virtual void endElement(const XML_Char * /*name*/) {}
  virtual void charData(const XML_Char * /*s*/, int /*len*/){}

  ;

  void init_params();

  // lvcs temp values
  vcl_string lvcs_cs_name_;
  double lvcs_origin_lon_;
  double lvcs_origin_lat_;
  double lvcs_origin_elev_;
  double lvcs_lon_scale_;
  double lvcs_lat_scale_;
  vcl_string lvcs_XYZ_unit_;
  vcl_string lvcs_geo_angle_unit_;
  double lvcs_local_origin_x_;
  double lvcs_local_origin_y_;
  double lvcs_theta_;
  // world origin
  vgl_point_3d<double> origin_;

  // scene directory (path)
  vcl_string path_;

  // scene name (string)
  vcl_string name_;

  // block list
  vcl_map<bstm_block_id, bstm_multi_block_metadata> blocks_;

  // list of appearances
  vcl_vector<vcl_string> appearances_;

  int version_;
};

#endif // bstm_scene_parser_h_
