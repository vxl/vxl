#include <string>
#include <iostream>
#include <cstdio>
#include "bwm_io_kml_camera.h"
#include "bwm_io_kml_parser.h"
#include <expatpp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
bool bwm_io_kml_camera::read_camera(std::string const& camera_path,
                                    double& right_fov, double& top_fov,
                                    double& altitude, double& heading,
                                    double& tilt, double& roll){
  bwm_io_kml_parser* parser = new bwm_io_kml_parser();
  std::FILE* xmlFile = std::fopen(camera_path.c_str(), "r");
  if (!xmlFile) {
    std::cerr << camera_path.c_str() << " error on opening\n";
    delete parser;
    return false;
  }
  if (!parser->parseFile(xmlFile)) {
    std::cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << '\n';

    delete parser;
    return false;
  }
  right_fov =   parser->right_fov_;
  top_fov =   parser->top_fov_;
  altitude =   parser->altitude_;
  heading =   parser->heading_;
  tilt =   parser->tilt_;
  roll =   parser->roll_;
  return true;
}
