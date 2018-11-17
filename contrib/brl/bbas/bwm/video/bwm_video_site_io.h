#ifndef bwm_video_site_io_h_
#define bwm_video_site_io_h_

#include <iostream>
#include <string>
#include <vector>
#include "bwm_video_site_io_defs.h"

#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "bwm_video_corr_sptr.h"

class bwm_video_site_io : public expatpp
{
 public:
  bwm_video_site_io(void);
  ~bwm_video_site_io(void) override = default;
  bool open(std::string const& xml_path);
  void clear();
  void set_name(std::string name) { name_ = name; }
  void set_corrs(std::vector<bwm_video_corr_sptr>const& corrs);
  void set_site_directory(std::string const& directory) { site_dir_=directory; }
  void set_video_path(std::string const& video_path);
  void set_camera_path(std::string const& video_path);
  std::string name() const { return name_; }
  std::string site_directory() const { return site_dir_; }
  std::string video_path() const { return video_path_; }
  std::string camera_path() const { return camera_path_; }
  std::vector<bwm_video_corr_sptr> corrs() { return corrs_; }
  std::vector<std::string> object_types() const { return obj_types_; }
  std::vector<std::string> object_paths() const { return obj_paths_; }
  void set_object_types(std::vector<std::string> object_types) {
    obj_types_=object_types; }
  void set_object_paths(std::vector<std::string> object_paths) {
    obj_paths_=object_paths; }
  void x_write(std::string const& xml_path);

 private:
  bool fail_;
  void startElement(const XML_Char* name, const XML_Char** atts) override;
  void endElement(const XML_Char* name) override;
  void charData(const XML_Char* s, int len) override;

  void handleAtts(const XML_Char** atts);
  void cdataHandler(const std::string& name, std::string data);
  void init_params();
  //Data
  int mDepth;
  std::string cdata;
  std::string last_tag;

  // intermediate variables to keep values during parsing
  std::string name_;
  std::string site_dir_;
  std::string video_path_;
  std::string camera_path_;
  bwm_video_corr_sptr corr_;
  //a set of frame-point pairs
  std::vector<unsigned> corr_ids_;
  std::vector<bwm_video_corr_sptr> corrs_;
  std::vector<std::string> obj_types_;
  std::vector<std::string> obj_paths_;
  std::string object_dir_;
  void trim_string(std::string& s);
};

#endif
