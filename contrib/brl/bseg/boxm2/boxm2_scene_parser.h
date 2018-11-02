#ifndef boxm2_scene_parser_h_
#define boxm2_scene_parser_h_

#include <string>
#include <iostream>
#include <map>
#include <expatpplib.h>

#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_block_metadata.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//scene level metadata
#define VERSION_TAG "version"
#define LVCS_TAG "lvcs"
#define LOCAL_ORIGIN_TAG "local_origin"
#define SCENE_PATHS_TAG "scene_paths"
#define APM_TAG "appearance"

//block level metadata
#define BLOCK_TAG "block"
#define BLOCK_ID_TAG "block_id"
#define BLOCK_ORIGIN_TAG "block_origin"
#define SUB_BLOCK_NUMS_TAG "sub_block_nums"
#define SUB_BLOCK_DIMENSIONS_TAG "sub_block_dimensions"
#define TREE_INIT_LEVEL_TAG "tree_init_level"
#define TREE_MAX_LEVEL_TAG "tree_max_level"
#define P_INIT_TAG "p_init"
#define MAX_MB_TAG "max_mb"


class boxm2_scene_parser : public expatpp
{
 public:
  boxm2_scene_parser();
  ~boxm2_scene_parser(void) override = default;

   // ACCESSORS for parser info
   bool lvcs(vpgl_lvcs& lvcs);
   vgl_point_3d<double> origin() const { return origin_; }
   std::string path() const { return path_; }
   bool is_model_local_to_scene_path() const { return is_model_local_to_scene_path_; }
   std::string name() const { return name_; }
   std::map<boxm2_block_id, boxm2_block_metadata> blocks() { return blocks_; }
   std::vector<std::string> appearances() const { return appearances_; }
   int num_illumination_bins() const { return num_illum_bins_; }
   int version() const { return version_; }
 private:
  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* /*name*/) override {}
  void charData(const XML_Char* /*s*/, int /*len*/) override {}

  void init_params();

  //lvcs temp values
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
  int num_illum_bins_;
  // world origin
  vgl_point_3d<double> origin_;

  // scene directory (path)
  std::string path_;

  // are model files relative to scene directory (path_)
  bool is_model_local_to_scene_path_;

  // scene name (string)
  std::string name_;

  // block list
  std::map<boxm2_block_id, boxm2_block_metadata> blocks_;

  // list of appearances
  std::vector<std::string> appearances_;

  int version_;
};

#endif
