#include <cstring>
#include <iostream>
#include <cstdio>
#include <sstream>
#include "bvxm_world_params_parser.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

//Constructor from file
bvxm_world_param_parser::bvxm_world_param_parser(std::string& filename)
{
  std::FILE* xmlFile = std::fopen(filename.c_str(), "r");
  if (!xmlFile) {
    std::cerr << filename << " error on opening\n";
    throw -1;
  }
  if (!this->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(this->XML_GetErrorCode()) << " at line "
             << this->XML_GetCurrentLineNumber() << '\n';
    throw -1;
  }
}

//-----------------------------------------------------------------------------
void bvxm_world_param_parser::startElement(const char* name, const char** atts)
{
  //set active tag for charData
  active_tag_ = std::string(name);

  if (active_tag_ == WORLD_DIR_TAG)
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        world_dir_ = std::string(atts[i+1]);

  if (active_tag_ == LVCS_TAG)
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        lvcs_path_ = std::string(atts[i+1]);

  if (active_tag_ == CORNER_X_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), corner_x_);
  if (active_tag_ == CORNER_Y_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), corner_y_);
  if (active_tag_ == CORNER_Z_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), corner_z_);

  if (active_tag_ == DIM_X_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), voxel_dim_x_);
  if (active_tag_ == DIM_Y_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), voxel_dim_y_);
  if (active_tag_ == DIM_Z_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), voxel_dim_z_);

  if (active_tag_ == VOXEL_LENGTH_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), voxel_length_);

  if (active_tag_ == MIN_OCP_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), min_ocp_prob_);

  if (active_tag_ == MAX_OCP_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), max_ocp_prob_);

  if (active_tag_ == MAX_SCALE_TAG )
    for (int i=0; atts[i]; i+=2)
      if (std::strcmp(atts[i], "value") == 0)
        bvxm_world_param_parser_convert(std::string(atts[i+1]), max_scale_);
}

void bvxm_world_param_parser::endElement(const XML_Char*  /*name*/)
{
}

//Grabs data from points
void bvxm_world_param_parser::charData(const XML_Char*  /*s*/, int  /*len*/)
{
}

bvxm_world_params_sptr bvxm_world_param_parser::create_bvxm_world_params()
{
  std::cout << " creating world params, world dir: " << world_dir_ << std::endl;
  bvxm_world_params_sptr params = new bvxm_world_params();
  vgl_point_3d<float> corner(corner_x_, corner_y_, corner_z_);
  vgl_vector_3d<unsigned int> voxel_dims(voxel_dim_x_, voxel_dim_y_, voxel_dim_z_);

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  if (lvcs_path_ != "") {
    std::ifstream is(lvcs_path_.c_str());
    if (!is)
    {
      std::cerr << " Error opening lvcs file  " << lvcs_path_ << std::endl;
      return bvxm_world_params_sptr();
    }
    lvcs->read(is);
    is.close();
  }

  params->set_params(world_dir_, corner, voxel_dims, voxel_length_, lvcs, min_ocp_prob_, max_ocp_prob_, max_scale_);
  return params;
}
