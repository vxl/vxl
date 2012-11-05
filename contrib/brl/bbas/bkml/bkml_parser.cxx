//:
// \file
// \brief Parses the kml configuration file for bwm tool.
// 
//
// \verybatim
//	Modifications
//   2012-09-10 Yi Dong - Modified to parser the polygon and path(LineString) coordinates stored in kml
// \endverbatim
//
#include "bkml_parser.h"

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

bkml_parser::bkml_parser()
{
#if 0
  init_params();
#endif
}

void bkml_parser::init_params()
{
}

void
bkml_parser ::cdataHandler(vcl_string name, vcl_string data)
{
}

void
bkml_parser::handleAtts(const XML_Char** /*atts*/)
{
}


void
bkml_parser::startElement(const char* name, const char** atts)
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
  else if (vcl_strcmp(name, KML_CORD_TAG) == 0 && (cord_tag_ != "") ){
	last_tag = KML_CORD_TAG;
  }
  else if (vcl_strcmp(name, KML_POLYOB_TAG) == 0) {
	cord_tag_ = KML_POLYOB_TAG;
  }
  else if (vcl_strcmp(name, KML_POLYIB_TAG) == 0) {
	cord_tag_ = KML_POLYIB_TAG;
  }
  else if (vcl_strcmp(name, KML_LINE_TAG) == 0) {
	cord_tag_ = KML_LINE_TAG;
  }
}


void
bkml_parser::endElement(const char* name)
{
}

void bkml_parser::charData(const XML_Char* s, int len)
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
  else if (last_tag == KML_CORD_TAG) {
    vcl_stringstream str;
    double x,y,z;
    vcl_string str_s;
    str_s = s;
    size_t cord_end = str_s.find(KML_POLYCORE_END_TAG);
    while(str_s[cord_end] != '\n')
      cord_end--;
    while(str_s[cord_end-1] == ' ')
      cord_end--;
    if(cord_end > len)
      len = (int)cord_end;
    for(int i=0; i<cord_end; ++i) 
      str << s[i];
    while(!str.eof()){
      str >> x; 
      str.ignore();
      str >> y; 
      str.ignore();
      str >> z; 
      if(str == "")
        str.ignore();
      vgl_point_3d<double> vpt(x,y,z);
      if(cord_tag_ == KML_POLYOB_TAG)
        polyouter_.push_back(vpt);
      else if(cord_tag_ == KML_POLYIB_TAG)
        polyinner_.push_back(vpt);
      else if(cord_tag_ == KML_LINE_TAG)
        linecord_.push_back(vpt);
      else{
        vcl_cout << "WARNING: shape tag can not be recognized (not LineString or Polygon)" << vcl_endl;
      }	
    }
    last_tag = "";
  }
}

void bkml_parser::trim_string(vcl_string& s)
{
  int i = (int)s.find_first_not_of(" ");
  int j = (int)s.find_last_not_of(" ");
  vcl_string t = s.substr(i,j-i+1);
  s = t;
}
