#ifndef bwm_io_kml_parser_h_
#define bwm_io_kml_parser_h_

#include "bwm_io_structs.h"


#ifdef WIN32
 #define _LIB
#endif
#include <expatpp.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_utility.h>


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
  void cdataHandler(vcl_string name, vcl_string data);
  void init_params();

  //element parser
  vcl_string last_tag;

  void trim_string(vcl_string& s);
};

#endif
