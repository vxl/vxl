#ifndef bwm_io_config_parser_h_
#define bwm_io_config_parser_h_

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "bwm_io_structs.h"
#include <bwm/io/bwm_site_sptr.h>

#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>

class bwm_io_config_parser : public expatpp
{
 public:
  bwm_io_config_parser(void);
  // parser should not delete the site, it is used afterwards
  ~bwm_io_config_parser(void) {}

  bwm_site_sptr site() { return site_; }
#if 0
  std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > correspondences() const { return corresp_; }
  std::string corresp_mode() const {return corr_mode_; }
  std::string corresp_type() const {return corr_type_; }
  std::vector<vsol_point_3d> corresp_world_pts() const {return corresp_world_pts_; }
#endif

 private:
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(std::string name, std::string data);
  void init_params();

  //Data
  int mDepth;
  std::string cdata;
  std::string last_tag;

  bwm_site_sptr site_;

  // intermediate variables to keep values during parsing
  std::string name_;
  bool status_;
  std::string image_path_;
  std::string camera_path_;
  std::string camera_type_;
  std::string proj2d_type_;
  std::string coin3d_name_;
  std::string object_path_;
  std::string object_type_;
  double lat_, lon_, elev_;

  // correspondence related parameters
  //std::string corr_mode_;
  //std::string corr_type_;
  std::string corr_cam_tab_;
  double X_, Y_, Z_;
#if 0
  std::vector<std::vector<std::pair<std::string, vsol_point_2d> > > corresp_;
  std::vector<vsol_point_3d> corresp_world_pts_;
#endif
  std::vector<std::pair<std::string, vsol_point_2d> > corresp_elm_;

  void trim_string(std::string& s);
};

#endif
