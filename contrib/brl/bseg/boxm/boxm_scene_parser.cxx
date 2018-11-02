//:
// \file
// \brief Parses the configuration file for bwm tool.
//
#include <sstream>
#include <cstring>
#include <iostream>
#include <cstdio>
#include "boxm_scene_parser.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  std::stringstream strm(t);
  strm >> d;
}

boxm_scene_parser::boxm_scene_parser()
{
  init_params();
}

bool boxm_scene_parser::lvcs(vpgl_lvcs& lvcs) {
  vpgl_lvcs::cs_names cs_name = vpgl_lvcs::str_to_enum(lvcs_cs_name_.data());
  vpgl_lvcs::LenUnits len_unit;
  vpgl_lvcs::AngUnits geo_unit;
   if (std::strcmp(lvcs_XYZ_unit_.data(), "feet")==0) {
     len_unit = vpgl_lvcs::FEET;
   }
   else if (std::strcmp(lvcs_XYZ_unit_.data(), "meters")==0) {
     len_unit = vpgl_lvcs::METERS;
   }
   else {
     std::cout << "LVCS Length Unit " << lvcs_XYZ_unit_ << " is not valid\n";
     return false;
   }

   if (std::strcmp(lvcs_geo_angle_unit_.data(), "radians")==0) {
     geo_unit = vpgl_lvcs::RADIANS;
   }
   else if (std::strcmp(lvcs_geo_angle_unit_.data(), "degrees")==0) {
     geo_unit = vpgl_lvcs::DEG;
   }
   else {
     std::cout << "LVCS Geo Angle Unit " << lvcs_geo_angle_unit_ << " is not valid\n";
     return false;
   }

   lvcs = vpgl_lvcs(lvcs_origin_lat_,lvcs_origin_lon_,lvcs_origin_elev_, cs_name,
                  lvcs_lat_scale_, lvcs_lon_scale_, geo_unit, len_unit,
                  lvcs_local_origin_x_, lvcs_local_origin_y_, lvcs_theta_);
  return true;
}

void boxm_scene_parser::init_params()
{
  lvcs_cs_name_="";
  lvcs_origin_lon_=0;
  lvcs_origin_lat_=0;
  lvcs_origin_elev_=0;
  lvcs_lon_scale_=0;
  lvcs_lat_scale_=0;
  lvcs_XYZ_unit_="";
  lvcs_geo_angle_unit_="";
  lvcs_local_origin_x_=0;
  lvcs_local_origin_y_=0;
  lvcs_theta_=0;

  // world origin
  local_orig_x_=0;
  local_orig_y_=0;
  local_orig_z_=0;

  // block dimensions
  block_dim_x_=0;
  block_dim_y_=0;
  block_dim_z_=0;

  // block numbers
  block_num_x_=0;
  block_num_y_=0;
  block_num_z_=0;

  path_="";
  block_pref_="";
  save_internal_nodes_ = false;
  save_platform_independent_ = true;
  load_all_blocks_=false;
  p_init_=0.01f;
  num_buffers_init_ = 0;
  size_buffer_init_ = 0;
}

void
boxm_scene_parser::startElement(const char* name, const char** atts)
{
#if 0
  std::cout<< "element=" << name << std::endl;
#endif
  if (std::strcmp(name, LVCS_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
        if (std::strcmp(atts[i], "cs_name") == 0)
        convert(atts[i+1], lvcs_cs_name_);
      else if (std::strcmp(atts[i], "origin_lon") == 0)
        convert(atts[i+1], lvcs_origin_lon_);
      else if (std::strcmp(atts[i], "origin_lat") == 0)
        convert(atts[i+1], lvcs_origin_lat_);
      else if (std::strcmp(atts[i], "origin_elev") == 0)
        convert(atts[i+1], lvcs_origin_elev_);
      else if (std::strcmp(atts[i], "lon_scale") == 0)
        convert(atts[i+1], lvcs_lon_scale_);
      else if (std::strcmp(atts[i], "lat_scale") == 0)
        convert(atts[i+1], lvcs_lat_scale_);
      else if (std::strcmp(atts[i], "local_XYZ_unit") == 0)
        convert(atts[i+1], lvcs_XYZ_unit_);
      else if (std::strcmp(atts[i], "geo_angle_unit") == 0)
        convert(atts[i+1], lvcs_geo_angle_unit_);
      else if (std::strcmp(atts[i], "local_origin_x") == 0)
        convert(atts[i+1], lvcs_local_origin_x_);
      else if (std::strcmp(atts[i], "local_origin_y") == 0)
        convert(atts[i+1], lvcs_local_origin_y_);
      else if (std::strcmp(atts[i], "theta_") == 0)
        convert(atts[i+1], lvcs_theta_);
    }
  }
  else if (std::strcmp(name,LOCAL_ORIGIN_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
        if (std::strcmp(atts[i], "x") == 0)
        convert(atts[i+1], local_orig_x_);
      else if (std::strcmp(atts[i], "y") == 0)
        convert(atts[i+1], local_orig_y_);
      else if (std::strcmp(atts[i], "z") == 0)
        convert(atts[i+1], local_orig_z_);
    }
  }
  else if (std::strcmp(name,BLOCK_DIMENSIONS_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
        if (std::strcmp(atts[i], "x") == 0)
        convert(atts[i+1], block_dim_x_);
      else if (std::strcmp(atts[i], "y") == 0)
        convert(atts[i+1], block_dim_y_);
      else if (std::strcmp(atts[i], "z") == 0)
        convert(atts[i+1], block_dim_z_);
    }
  }
  else if (std::strcmp(name,BLOCK_NUM_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "x_dimension") == 0)
        convert(atts[i+1], block_num_x_);
      else if (std::strcmp(atts[i], "y_dimension") == 0)
        convert(atts[i+1], block_num_y_);
      else if (std::strcmp(atts[i], "z_dimension") == 0)
        convert(atts[i+1], block_num_z_);
    }
  }
  else if (std::strcmp(name,SCENE_PATHS_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
      std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "path") == 0)
        convert(atts[i+1], path_);
      else if (std::strcmp(atts[i], "block_prefix") == 0)
        convert(atts[i+1], block_pref_);
    }
  }
  else if (std::strcmp(name,APP_MODEL_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "type") == 0)
        convert(atts[i+1], app_model_);
    }
  }
  else if (std::strcmp(name,MULTI_BIN_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "value") == 0)
        convert(atts[i+1], multi_bin_);
    }
  }
  else if (std::strcmp(name,SAVE_INTERNAL_NODES_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
      std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "value") == 0)
        convert(atts[i+1], save_internal_nodes_);
    }
  }
  else if (std::strcmp(name,SAVE_PLATFORM_INDEPENDENT_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "value") == 0)
        convert(atts[i+1], save_platform_independent_);
    }
    //std::cout << "parser: save_platform_independent_ = " << save_platform_independent_ << std::endl;
  }
  else if (std::strcmp(name,LOAD_ALL_BLOCKS_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "value") == 0)
        convert(atts[i+1], load_all_blocks_);
    }
    //std::cout << "parser: load_all_blocks_ = " << load_all_blocks_ << std::endl;
  }

  else if (std::strcmp(name, "octree_level")==0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "max") == 0)
        convert(atts[i+1], max_tree_level_);
      else if (std::strcmp(atts[i], "init") == 0)
        convert(atts[i+1], init_tree_level_);
    }
  }
  else if (std::strcmp(name, "p_init")==0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
    if (std::strcmp(atts[i], "val") == 0)
        convert(atts[i+1], p_init_);
    }
  }

  else if (std::strcmp(name, "tree_init") == 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "num_buffers")==0)
        convert(atts[i+1], num_buffers_init_);
      if (std::strcmp(atts[i], "buff_size")==0)
        convert(atts[i+1], size_buffer_init_);
    }
  }

  else if (std::strcmp(name, "max_mb") == 0) {
    for (int i=0; atts[i]; i+=2) {
#if 0
        std::cout << "  Attr=" << atts[i] << "->" << atts[i+1] << std::endl;
#endif
      if (std::strcmp(atts[i], "mb") == 0)
        convert(atts[i+1], max_mb_);
    }
  }

}
