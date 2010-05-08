#ifndef boxm2_scene_parser_h_
#define boxm2_scene_parser_h_

#include <boxm2/boxm_scene.h>

#include <expatpplib.h>
#include <vcl_string.h>

#include <vpgl/bgeo/bgeo_lvcs.h>
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
#define P_INIT_TAG "p_init"

class boxm_scene_parser : public expatpp
{
 public:
  boxm_scene_parser();

  ~boxm_scene_parser(void) {}

   bool lvcs(bgeo_lvcs& lvcs);
   vgl_point_3d<double> origin() const { return vgl_point_3d<double>(local_orig_x_,local_orig_y_,local_orig_z_);}
   vgl_vector_3d<double> block_dim() const { return vgl_vector_3d<double>(block_dim_x_,block_dim_y_,block_dim_z_);}
   vgl_vector_3d<unsigned> block_nums() const { return vgl_vector_3d<unsigned>(block_num_x_,block_num_y_,block_num_z_);}
   void paths(vcl_string& scene_path, vcl_string& block_pref) {scene_path=path_; block_pref=block_pref_;}
   vcl_string app_model() const { return app_model_; }
   bool multi_bin() const { return multi_bin_; }
   void levels(unsigned& max, unsigned& init) { max = max_tree_level_; init=init_tree_level_; }
   bool save_internal_nodes() {return save_internal_nodes_;}
   bool save_platform_independent() {return save_platform_independent_;}
   float p_init(){return p_init_;}

 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* /*name*/) {}
  virtual void charData(const XML_Char* /*s*/, int /*len*/) {}

  void init_params();

  // temporary values
  //lvcs
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

  vcl_string path_;
  vcl_string block_pref_;
  vcl_string app_model_;
  bool multi_bin_;
  unsigned max_tree_level_;
  unsigned init_tree_level_;
  bool save_internal_nodes_;
  bool save_platform_independent_;
  float p_init_;
};

#endif
