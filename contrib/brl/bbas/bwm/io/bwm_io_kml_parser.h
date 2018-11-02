#ifndef bwm_io_kml_parser_h_
#define bwm_io_kml_parser_h_

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include "bwm_io_structs.h"


#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class bwm_io_kml_parser : public expatpp
{
 public:
  bwm_io_kml_parser(void);
  // parser should not delete the site, it is used afterwards
  ~bwm_io_kml_parser(void) {}

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
  void cdataHandler(std::string name, std::string data);
  void init_params();

  //element parser
  std::string last_tag;

  void trim_string(std::string& s);
};

#endif
