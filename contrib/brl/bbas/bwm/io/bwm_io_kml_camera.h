#ifndef bwm_io_kml_camera_h_
#define bwm_io_kml_camera_h_
#include <vcl_string.h>
class bwm_io_kml_camera
{
 public:

  static bool read_camera(vcl_string const& camera_path,
                          double& right_fov, double& top_fov,
                          double& altitude, double& heading,
                          double& tilt, double& roll);


 private:
  bwm_io_kml_camera(void);
  ~bwm_io_kml_camera(void);
};

#endif
