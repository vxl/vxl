#ifndef bstm_multi_space_time_scene_parser_hxx_
#define bstm_multi_space_time_scene_parser_hxx_

//:
// \file
// \brief Parses the configuration file for bwm tool.
//

#include "space_time_scene_parser.h"

#include <vcl_compiler.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_utility.h>

#include <bstm/bstm_scene_parser.h>

// --------------
// --- PARSER ---
// --------------
template <typename T> void convert(const char *t, T &d) {
  vcl_stringstream strm(t);
  strm >> d;
}

template <typename Block>
bool space_time_scene_parser<Block>::lvcs(vpgl_lvcs &lvcs) {
  vpgl_lvcs::cs_names cs_name = vpgl_lvcs::str_to_enum(lvcs_cs_name_.data());
  vpgl_lvcs::LenUnits len_unit;
  vpgl_lvcs::AngUnits geo_unit;
  if (vcl_strcmp(lvcs_XYZ_unit_.data(), "feet") == 0) {
    len_unit = vpgl_lvcs::FEET;
  } else if (vcl_strcmp(lvcs_XYZ_unit_.data(), "meters") == 0) {
    len_unit = vpgl_lvcs::METERS;
  } else {
    vcl_cout << "LVCS Length Unit " << lvcs_XYZ_unit_ << " is not valid\n";
    return false;
  }

  if (vcl_strcmp(lvcs_geo_angle_unit_.data(), "radians") == 0) {
    geo_unit = vpgl_lvcs::RADIANS;
  } else if (vcl_strcmp(lvcs_geo_angle_unit_.data(), "degrees") == 0) {
    geo_unit = vpgl_lvcs::DEG;
  } else {
    vcl_cout << "LVCS Geo Angle Unit " << lvcs_geo_angle_unit_
             << " is not valid\n";
    return false;
  }

  lvcs = vpgl_lvcs(lvcs_origin_lat_,
                   lvcs_origin_lon_,
                   lvcs_origin_elev_,
                   cs_name,
                   lvcs_lat_scale_,
                   lvcs_lon_scale_,
                   geo_unit,
                   len_unit,
                   lvcs_local_origin_x_,
                   lvcs_local_origin_y_,
                   lvcs_theta_);
  return true;
}

//-----------------------------------------------------------------------------
//: Start Element needs to parse the following tags
// - scene level metadata
//   * LVCS_TAG "lvcs"
//   * LOCAL_ORIGIN_TAG "local_origin"
//   * SCENE_PATHS_TAG "scene_paths"
// - block level metadata
//   * BLOCK_TAG "block"
//   * BLOCK_ID_TAG "block_id"
//   * BLOCK_ORIGIN_TAG "block_origin"
//   * SUB_BLOCK_DIMENSIONS_TAG "sub_block_dimensions"
//   * TREE_INIT_LEVEL_TAG "tree_init_level"
//   * TREE_MAX_LEVEL_TAG "tree_max_level"
//   * P_INIT_TAG "p_init"
//   * MAX_MB_TAG "max_mb"
template <typename Block>
void space_time_scene_parser<Block>::startElement(const char *name,
                                                  const XML_Char **atts) {
#if 0
  vcl_cout<< "element=" << name << '\n'
          << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
#endif

  // LVCS tag
  if (vcl_strcmp(name, LVCS_TAG) == 0) {
    for (int i = 0; atts[i]; i += 2) {
      if (vcl_strcmp(atts[i], "cs_name") == 0)
        convert(atts[i + 1], lvcs_cs_name_);
      else if (vcl_strcmp(atts[i], "origin_lon") == 0)
        convert(atts[i + 1], lvcs_origin_lon_);
      else if (vcl_strcmp(atts[i], "origin_lat") == 0)
        convert(atts[i + 1], lvcs_origin_lat_);
      else if (vcl_strcmp(atts[i], "origin_elev") == 0)
        convert(atts[i + 1], lvcs_origin_elev_);
      else if (vcl_strcmp(atts[i], "lon_scale") == 0)
        convert(atts[i + 1], lvcs_lon_scale_);
      else if (vcl_strcmp(atts[i], "lat_scale") == 0)
        convert(atts[i + 1], lvcs_lat_scale_);
      else if (vcl_strcmp(atts[i], "local_XYZ_unit") == 0)
        convert(atts[i + 1], lvcs_XYZ_unit_);
      else if (vcl_strcmp(atts[i], "geo_angle_unit") == 0)
        convert(atts[i + 1], lvcs_geo_angle_unit_);
      else if (vcl_strcmp(atts[i], "local_origin_x") == 0)
        convert(atts[i + 1], lvcs_local_origin_x_);
      else if (vcl_strcmp(atts[i], "local_origin_y") == 0)
        convert(atts[i + 1], lvcs_local_origin_y_);
      else if (vcl_strcmp(atts[i], "theta_") == 0)
        convert(atts[i + 1], lvcs_theta_);
    }
  }

  // Local Origin Tag
  else if (vcl_strcmp(name, LOCAL_ORIGIN_TAG) == 0) {
    double x, y, z;
    for (int i = 0; atts[i]; i += 2) {
      if (vcl_strcmp(atts[i], "x") == 0)
        convert(atts[i + 1], x);
      else if (vcl_strcmp(atts[i], "y") == 0)
        convert(atts[i + 1], y);
      else if (vcl_strcmp(atts[i], "z") == 0)
        convert(atts[i + 1], z);
    }
    origin_ = vgl_point_3d<double>(x, y, z);
  }

  // SCENE PATHS TAG
  else if (vcl_strcmp(name, SCENE_PATHS_TAG) == 0) {
    for (int i = 0; atts[i]; i += 2) {
      if (vcl_strcmp(atts[i], "path") == 0)
        convert(atts[i + 1], path_);
    }
  }

  //----------- APPEARANCE TAG -------------------------------------------------
  else if (vcl_strcmp(name, APM_TAG) == 0) {
    for (int i = 0; atts[i]; i += 2) {
      vcl_string buff;
      if (vcl_strcmp(atts[i], "apm") == 0) {
        convert(atts[i + 1], buff);
        appearances_.push_back(buff);
      }
    }
  } else if (vcl_strcmp(name, VERSION_TAG) == 0) {
    for (int i = 0; atts[i]; i += 2) {
      if (vcl_strcmp(atts[i], "number") == 0)
        convert(atts[i + 1], version_);
    }
  }
  //---------- BLOCK TAG -------------------------------------------------------
  else if (vcl_strcmp(name, BLOCK_TAG) == 0) {
    block_metadata metadata = block_metadata::from_xml(atts);
    metadata.version_ = version_;
    blocks_[metadata.id_] = metadata;
  }
}

#endif // bstm_multi_space_time_scene_parser_hxx_
