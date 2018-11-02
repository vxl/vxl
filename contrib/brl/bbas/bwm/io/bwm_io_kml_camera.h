#ifndef bwm_io_kml_camera_h_
#define bwm_io_kml_camera_h_
#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class bwm_io_kml_camera
{
 public:

  static bool read_camera(std::string const& camera_path,
                          double& right_fov, double& top_fov,
                          double& altitude, double& heading,
                          double& tilt, double& roll);


 private:
  bwm_io_kml_camera(void);
  ~bwm_io_kml_camera(void);
};

#endif
