//:
// \file
// \brief Parses the kmluration file for bwm tool.
//
#include "bwm_io_kml_parser.h"

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>

// --------------
// --- PARSER ---
// --------------
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);
  strm >> d;
}

bwm_io_kml_parser::bwm_io_kml_parser()
{
#if 0
  init_params();
#endif
}

void bwm_io_kml_parser::init_params()
{
}

void
bwm_io_kml_parser ::cdataHandler(vcl_string name, vcl_string data)
{
}

void
bwm_io_kml_parser::handleAtts(const XML_Char** /*atts*/)
{
}


void
bwm_io_kml_parser::startElement(const char* name, const char** atts)
{
  if (vcl_strcmp(name, KML_LON_TAG) == 0) {
    last_tag = KML_LON_TAG;
  }
  else if (vcl_strcmp(name, KML_LAT_TAG) == 0) {
    last_tag = KML_LAT_TAG;
  }
  else if (vcl_strcmp(name, KML_ALT_TAG) == 0) {
    last_tag = KML_ALT_TAG;
  }
  else if (vcl_strcmp(name, KML_HEAD_TAG) == 0 ) {
    last_tag = KML_HEAD_TAG;
  }
  else if (vcl_strcmp(name, KML_TILT_TAG) == 0 ) {
    last_tag = KML_TILT_TAG;
      }
  else if (vcl_strcmp(name, KML_ROLL_TAG) == 0 ) {
    last_tag = KML_ROLL_TAG;
  }
  else if (vcl_strcmp(name, KML_RFOV_TAG) == 0) {
    last_tag = KML_RFOV_TAG;
      }
  else if (vcl_strcmp(name, KML_TFOV_TAG) == 0) {
    last_tag = KML_TFOV_TAG;
  }
  else if (vcl_strcmp(name, KML_NEAR_TAG) == 0) {
    last_tag = KML_NEAR_TAG;
  }
}


void
bwm_io_kml_parser::endElement(const char* name)
{
}

void bwm_io_kml_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
    return;  // called with whitespace between elements
  if (last_tag == KML_LON_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> longitude_;
    last_tag = "";
  }
  else if (last_tag == KML_LAT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> latitude_;
    last_tag = "";
  }
  else if (last_tag == KML_ALT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> altitude_;
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> heading_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> tilt_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> roll_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> right_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> top_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_NEAR_TAG) {
    vcl_stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> near_;
    last_tag = "";
  }
}

void bwm_io_kml_parser::trim_string(vcl_string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}
