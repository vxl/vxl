//:
// \file
// \brief Parses the configuration file for bwm tool.
//
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "bwm_io_config_parser.h"
#include "bwm_site.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
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

bwm_io_config_parser::bwm_io_config_parser()
{
  init_params();
  site_ = new bwm_site();
}

void bwm_io_config_parser::init_params()
{
  name_ = "";
  image_path_ = "";
  camera_path_ = "";
  camera_type_ = "";
}

void
bwm_io_config_parser ::cdataHandler(std::string name, std::string data)
{
  // clean up the empty chars before and after the file paths
  trim_string(data);
  if (name.compare(IMAGE_PATH_TAG) == 0 ) {
    image_path_.assign(data);
  }
  else if (name.compare(CAMERA_PATH_TAG) == 0 ) {
    camera_path_.assign(data);
  }
  else if (name.compare(CORR_CAMERA_TAG) == 0 ) {
    corr_cam_tab_.assign(data);
  }
  else if (name.compare(OBJECT_TAG) == 0 ) {
    object_path_.assign(data);
  }
  else if (name.compare(SITE_HOME_TAG) == 0) {
    site_->path_.assign(data);
  }
  else if (name.compare(PYRAMID_EXE_TAG) == 0) {
    site_->pyr_exe_path_.assign(data);
  }
   cdata = "";
}

void
bwm_io_config_parser::handleAtts(const XML_Char** /*atts*/)
{
}

void
bwm_io_config_parser::startElement(const char* name, const char** atts)
{
  if (std::strcmp(name, SITE_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "name") == 0)
        convert(atts[i+1], site_->name_);
    }
  }
  else if ((std::strcmp(name,IMAGE_TABLEAU_TAG)== 0) ||
    (std::strcmp(name, CAMERA_TABLEAU_TAG) == 0) ||
    (std::strcmp(name,COIN3D_TABLEAU_TAG) == 0) ||
    (std::strcmp(name,PROJ2D_TABLEAU_TAG) == 0)) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "name") == 0)
        convert(atts[i+1], name_);
      else if (std::strcmp(atts[i], "status") == 0) {
        std::string status;
        convert(atts[i+1], status);
        if (status.compare("active") == 0)
          status_ = true;
        else
          status_ = false;
      }
    }
  }
  else if (std::strcmp(name,CAMERA_PATH_TAG)== 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "type") == 0)
        convert(atts[i+1], camera_type_);
    }
  }
  else if (std::strcmp(name, CORRESPONDENCES_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "mode") == 0)
        convert(atts[i+1], site_->corr_mode_);
      else if (std::strcmp(atts[i], "type") == 0)
        convert(atts[i+1], site_->corr_type_);
    }
  }
  else if (std::strcmp(name, CORRESP_PT_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "X") == 0)
        convert(atts[i+1], X_);
      else if (std::strcmp(atts[i], "Y") == 0)
        convert(atts[i+1], Y_);
    }
  }
  else if (std::strcmp(name, CORRESP_WORLD_PT_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "X") == 0)
        convert(atts[i+1], X_);
      else if (std::strcmp(atts[i], "Y") == 0)
        convert(atts[i+1], Y_);
      else if (std::strcmp(atts[i], "Z") == 0)
        convert(atts[i+1], Z_);
    }
  }
  else if (std::strcmp(name, OBJECT_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "type") == 0)
        convert(atts[i+1], object_type_);
    }
  }
  else if (std::strcmp(name, LVCS_TAG) == 0) {
    for (int i=0; atts[i]; i+=2) {
      if (std::strcmp(atts[i], "lat") == 0)
        convert(atts[i+1], lat_);
      else if (std::strcmp(atts[i], "lon") == 0)
        convert(atts[i+1], lon_);
      else if (std::strcmp(atts[i], "elev") == 0)
        convert(atts[i+1], elev_);
    }
  }
}

void
bwm_io_config_parser::endElement(const char* name)
{
  // first check if the last element has some cdata
  if (cdata.size() > 0) {
    cdataHandler(name, cdata);
    cdata= "";
  }

  if (std::strcmp(name, IMAGE_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_img* img = new bwm_io_tab_config_img(name_, status_, image_path_);
    site_->tableaus_.push_back(img);
    init_params();
  }
  else if (std::strcmp(name, CAMERA_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_cam* cam = new bwm_io_tab_config_cam(name_, status_, image_path_, camera_path_, camera_type_);
    site_->tableaus_.push_back(cam);
    init_params();
  }
  else if (std::strcmp(name, COIN3D_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_coin3d* coin3d = new bwm_io_tab_config_coin3d(name_, status_, camera_path_, camera_type_);
    site_->tableaus_.push_back(coin3d);
    init_params();
  }
  else if (std::strcmp(name, PROJ2D_TABLEAU_TAG) == 0) {
    bwm_io_tab_config_proj2d* proj2d = new bwm_io_tab_config_proj2d(name_, status_, proj2d_type_, camera_path_, camera_type_, coin3d_name_);
    site_->tableaus_.push_back(proj2d);
  }
  else if (std::strcmp(name, CORRESP_ELM_TAG) == 0) {
    corresp_elm_.push_back(std::pair<std::string, vsol_point_2d> (corr_cam_tab_, vsol_point_2d(X_,Y_)));
    corr_cam_tab_ = "";
  }
  else if (std::strcmp(name, CORRESP_WORLD_PT_TAG) == 0) {
    site_->corresp_world_pts_.push_back(vsol_point_3d(X_, Y_, Z_));
  }
  else if (std::strcmp(name, CORRESP_TAG) == 0) {
    site_->corresp_.push_back(corresp_elm_);
    corresp_elm_.clear();
  }
  else if (std::strcmp(name, OBJECT_TAG) == 0) {
    std::pair<std::string, std::string> object(object_path_, object_type_);
    site_->objects_.push_back(object);
  }
  else if (std::strcmp(name, LVCS_TAG) == 0) {
    site_->lvcs_ = new vsol_point_3d(lat_, lon_, elev_);
  }
}

void bwm_io_config_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements

  std::putchar('(');
  std::fwrite(s, len, 1, stdout);
  puts(")");
  cdata.append(s, len);
}

void bwm_io_config_parser::trim_string(std::string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  std::string t = s.substr(i,j-i+1);
  s = t;
}
