#ifndef bvxm_world_params_parser_h_
#define bvxm_world_params_parser_h_
//:
// \file expatpp parser to parse bpro process parameter files of bvxm worlds in order to create them outside bpro processes

#include <string>
#include <iostream>
#include <sstream>
#include <expatpplib.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// bvxm files
#include <bvxm/bvxm_world_params.h>

//tag macros
#define WORLD_DIR_TAG "input_directory"
#define CORNER_X_TAG "corner_x"
#define CORNER_Y_TAG "corner_y"
#define CORNER_Z_TAG "corner_z"
#define DIM_X_TAG "voxel_dim_x"
#define DIM_Y_TAG "voxel_dim_y"
#define DIM_Z_TAG "voxel_dim_z"
#define VOXEL_LENGTH_TAG "voxel_length"
#define LVCS_TAG "lvcs"
#define MIN_OCP_TAG "min_ocp_prob"
#define MAX_OCP_TAG "max_ocp_prob"
#define MAX_SCALE_TAG "max_scale_prob"

class bvxm_world_param_parser : public expatpp
{
 public:
  bvxm_world_param_parser() = default;
  bvxm_world_param_parser(std::string& filename);
  ~bvxm_world_param_parser(void) override = default;

  bvxm_world_params_sptr create_bvxm_world_params();

  std::string world_directory() const { return world_dir_; } //  "input_directory";
  float corner_x() const { return corner_x_; } //  "corner_x";
  float corner_y() const { return corner_y_; } //  "corner_y";
  float corner_z() const { return corner_z_; } // "corner_z";
  unsigned int voxel_dim_x() const { return voxel_dim_x_; } // "voxel_dim_x";
  unsigned int voxel_dim_y() const { return voxel_dim_y_; } // "voxel_dim_y";
  unsigned int voxel_dim_z() const { return voxel_dim_z_; } // "voxel_dim_z";
  float voxel_length() const { return voxel_length_; } // "voxel_length";
  std::string lvcs_path() const { return lvcs_path_; } // "lvcs";
  float min_ocp_prob() const { return min_ocp_prob_; } // "min_ocp_prob";
  float max_ocp_prob() const { return max_ocp_prob_; } // "max_ocp_prob";
  unsigned int max_scale() const { return max_scale_; } // "max_scale";

 private:
  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* name) override;
  void charData(const XML_Char* s, int len) override;

  std::string world_dir_, lvcs_path_, temp_str_, active_tag_;
  float corner_x_, corner_y_, corner_z_;
  unsigned int voxel_dim_x_, voxel_dim_y_, voxel_dim_z_, max_scale_;
  float voxel_length_;
  float min_ocp_prob_, max_ocp_prob_;
};

//string converter
template <typename T>
void bvxm_world_param_parser_convert(std::string s, T& d)
{
  std::stringstream strm(s);
  strm >> d;
}


#endif // bvxm_world_params_parser_h_
