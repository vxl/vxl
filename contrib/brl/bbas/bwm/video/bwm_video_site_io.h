#ifndef bwm_video_site_io_h_
#define bwm_video_site_io_h_

#include "bwm_video_site_io_defs.h"

#include <expatpp/expatpplib.h>
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
  void set_name(vcl_string name){name_ = name;}
  void set_corrs(vcl_vector<bwm_video_corr_sptr>const& corrs);
  void set_video_path(vcl_string const& video_path);
  void set_camera_path(vcl_string const& video_path);
  vcl_string name(){return name_;}
  vcl_string video_path(){return video_path_;}
  vcl_string camera_path(){return camera_path_;}
  vcl_vector<bwm_video_corr_sptr> corrs(){return corrs_;}

  void x_write(vcl_string const& xml_path);

 private:
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
  vcl_string video_path_;
  vcl_string camera_path_;
  bwm_video_corr_sptr corr_;
  //a set of frame-point pairs
  vcl_vector<bwm_video_corr_sptr> corrs_;

  void trim_string(vcl_string& s);
};

#endif
