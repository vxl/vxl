//:
// \file
// \brief Parses the kmluration file for bwm tool.
//
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstring>
#include "bwm_io_kml_parser.h"

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
bwm_io_kml_parser ::cdataHandler(std::string name, std::string data)
{
}

void
bwm_io_kml_parser::handleAtts(const XML_Char** /*atts*/)
{
}


void
bwm_io_kml_parser::startElement(const char* name, const char** atts)
{
  if (std::strcmp(name, KML_LON_TAG) == 0) {
    last_tag = KML_LON_TAG;
  }
  else if (std::strcmp(name, KML_LAT_TAG) == 0) {
    last_tag = KML_LAT_TAG;
  }
  else if (std::strcmp(name, KML_ALT_TAG) == 0) {
    last_tag = KML_ALT_TAG;
  }
  else if (std::strcmp(name, KML_HEAD_TAG) == 0 ) {
    last_tag = KML_HEAD_TAG;
  }
  else if (std::strcmp(name, KML_TILT_TAG) == 0 ) {
    last_tag = KML_TILT_TAG;
      }
  else if (std::strcmp(name, KML_ROLL_TAG) == 0 ) {
    last_tag = KML_ROLL_TAG;
  }
  else if (std::strcmp(name, KML_RFOV_TAG) == 0) {
    last_tag = KML_RFOV_TAG;
      }
  else if (std::strcmp(name, KML_TFOV_TAG) == 0) {
    last_tag = KML_TFOV_TAG;
  }
  else if (std::strcmp(name, KML_NEAR_TAG) == 0) {
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
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> longitude_;
    last_tag = "";
  }
  else if (last_tag == KML_LAT_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> latitude_;
    last_tag = "";
  }
  else if (last_tag == KML_ALT_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> altitude_;
    last_tag = "";
  }
  else if (last_tag == KML_HEAD_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> heading_;
    last_tag = "";
  }
  else if (last_tag == KML_TILT_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> tilt_;
    last_tag = "";
  }
  else if (last_tag == KML_ROLL_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> roll_;
    last_tag = "";
  }
  else if (last_tag == KML_RFOV_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> right_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_TFOV_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> top_fov_;
    last_tag = "";
  }
  else if (last_tag == KML_NEAR_TAG) {
    std::stringstream str;
    for (int i =0; i<len; ++i)
      str << s[i];
    str >> near_;
    last_tag = "";
  }
}

void bwm_io_kml_parser::trim_string(std::string& s)
{
  int i = s.find_first_not_of(" ");
  int j = s.find_last_not_of(" ");
  std::string t = s.substr(i,j-i+1);
  s = t;
}
