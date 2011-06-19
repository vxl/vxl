#ifndef bwm_video_site_io_h_
#define bwm_video_site_io_h_

#include "bwm_video_site_io_defs.h"

#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include "bwm_video_corr_sptr.h"

class bwm_video_site_io : public expatpp
{
 public:
  bwm_video_site_io(void);
  ~bwm_video_site_io(void) {}
  bool open(vcl_string const& xml_path);
  void clear();
  void set_name(vcl_string name) { name_ = name; }
  void set_corrs(vcl_vector<bwm_video_corr_sptr>const& corrs);
  void set_site_directory(vcl_string const& directory) { site_dir_=directory; }
  void set_video_path(vcl_string const& video_path);
  void set_camera_path(vcl_string const& video_path);
  vcl_string name() const { return name_; }
  vcl_string site_directory() const { return site_dir_; }
  vcl_string video_path() const { return video_path_; }
  vcl_string camera_path() const { return camera_path_; }
  vcl_vector<bwm_video_corr_sptr> corrs() { return corrs_; }
  vcl_vector<vcl_string> object_types() const { return obj_types_; }
  vcl_vector<vcl_string> object_paths() const { return obj_paths_; }
  void set_object_types(vcl_vector<vcl_string> object_types) {
    obj_types_=object_types; }
  void set_object_paths(vcl_vector<vcl_string> object_paths) {
    obj_paths_=object_paths; }
  void x_write(vcl_string const& xml_path);

 private:
  bool fail_;
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);

  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void init_params();
  //Data
  int mDepth;
  vcl_string cdata;
  vcl_string last_tag;

  // intermediate variables to keep values during parsing
  vcl_string name_;
  vcl_string site_dir_;
  vcl_string video_path_;
  vcl_string camera_path_;
  bwm_video_corr_sptr corr_;
  //a set of frame-point pairs
  vcl_vector<unsigned> corr_ids_;
  vcl_vector<bwm_video_corr_sptr> corrs_;
  vcl_vector<vcl_string> obj_types_;
  vcl_vector<vcl_string> obj_paths_;
  vcl_string object_dir_;
  void trim_string(vcl_string& s);
};

#endif
