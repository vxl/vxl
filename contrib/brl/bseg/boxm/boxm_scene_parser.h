#ifndef boxm_scene_parser_h_
#define boxm_scene_parser_h_

#include <iostream>
#include <string>
#include <boxm/boxm_scene.h>

#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#define LVCS_TAG "lvcs"
#define LOCAL_ORIGIN_TAG "local_origin"
#define BLOCK_DIMENSIONS_TAG "block_dimensions"
#define BLOCK_NUM_TAG "blocks"
#define SCENE_PATHS_TAG "scene_paths"
#define APP_MODEL_TAG "appearence_model"
#define MULTI_BIN_TAG "multi_bin"
#define SAVE_INTERNAL_NODES_TAG "save_internal_nodes"
#define SAVE_PLATFORM_INDEPENDENT_TAG "save_platform_independent"
#define LOAD_ALL_BLOCKS_TAG "load_all_blocks"
#define OCTREE_LEVELS_TAG "octree_level"
#define P_INIT_TAG "p_init"
#define TREE_INIT_TAG "tree_init"
#define MAX_MB_TAG "max_mb"

class boxm_scene_parser : public expatpp
{
 public:
  boxm_scene_parser();

  ~boxm_scene_parser(void) override = default;

   bool lvcs(vpgl_lvcs& lvcs);
   vgl_point_3d<double> origin() const { return {local_orig_x_,local_orig_y_,local_orig_z_}; }
   vgl_vector_3d<double> block_dim() const { return {block_dim_x_,block_dim_y_,block_dim_z_}; }
   vgl_vector_3d<unsigned> block_nums() const { return {block_num_x_,block_num_y_,block_num_z_}; }
   void paths(std::string& scene_path, std::string& block_pref) { scene_path=path_; block_pref=block_pref_; }
   std::string app_model() const { return app_model_; }
   bool multi_bin() const { return multi_bin_; }
   void levels(unsigned& max, unsigned& init) { max = max_tree_level_; init=init_tree_level_; }
   bool save_internal_nodes() const { return save_internal_nodes_; }
   bool save_platform_independent() const { return save_platform_independent_; }
   bool load_all_blocks() const { return load_all_blocks_; }
   void tree_buffer_shape(int &num_buffers, int &buff_size) { num_buffers = num_buffers_init_; buff_size = size_buffer_init_; }
   int max_mb() const { return max_mb_; }

   float p_init() const { return p_init_; }

 private:
  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* /*name*/) override {}
  void charData(const XML_Char* /*s*/, int /*len*/) override {}

  void init_params();

  // temporary values
  //lvcs
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

  // world origin
  double local_orig_x_;
  double local_orig_y_;
  double local_orig_z_;

  // block dimensions
  double block_dim_x_;
  double block_dim_y_;
  double block_dim_z_;

  // block numbers
  unsigned block_num_x_;
  unsigned block_num_y_;
  unsigned block_num_z_;

  std::string path_;
  std::string block_pref_;
  std::string app_model_;
  bool multi_bin_;
  unsigned max_tree_level_;
  unsigned init_tree_level_;
  bool save_internal_nodes_;
  bool save_platform_independent_;
  bool load_all_blocks_;
  float p_init_;
  int num_buffers_init_, size_buffer_init_;
  int max_mb_;
};

#endif
