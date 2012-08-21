#ifndef bkml_parser_h_
#define bkml_parser_h_

// Tags for KML
#define PHOTO_OVL_TAG "PhotoOverlay"
#define KML_CAMERA_TAG "Camera"
#define KML_LON_TAG "longitude"
#define KML_LAT_TAG "latitude"
#define KML_ALT_TAG "altitude"
#define KML_HEAD_TAG "heading"
#define KML_TILT_TAG "tilt"
#define KML_ROLL_TAG "roll"
#define KML_RFOV_TAG "rightFov"
#define KML_TFOV_TAG "topFov"
#define KML_NEAR_TAG "near"


#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>


class bkml_parser : public expatpp
{
 public:
  bkml_parser(void);
  // parser should not delete the site, it is used afterwards
  ~bkml_parser(void) {}

  // results of parse
  double longitude_;
  double latitude_;
  double altitude_;
  double heading_;
  double tilt_;
  double roll_;
  double right_fov_;
  double top_fov_;
  double near_;

 private:

  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
  virtual void charData(const XML_Char* s, int len);
  void handleAtts(const XML_Char** atts);
  void cdataHandler(vcl_string name, vcl_string data);
  void init_params();

  //element parser
  vcl_string last_tag;

  void trim_string(vcl_string& s);
};

#endif
