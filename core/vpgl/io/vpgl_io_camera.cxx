#include "vpgl_io_camera.h"
//:
// \file

//: Binary save camera sptr to stream
void vsl_b_write(vsl_b_ostream & os, vpgl_camera_double_sptr const& cam_sptr)
{
  if (!cam_sptr) return;
  vpgl_camera<double>* cam = cam_sptr.ptr();
  vsl_b_write(os, cam);
}

//: Binary save camera sptr to stream
void vsl_b_write(vsl_b_ostream & os, vpgl_camera_float_sptr const& cam_sptr)
{
  if (!cam_sptr) return;
  vpgl_camera<float>* cam = cam_sptr.ptr();
  vsl_b_write(os, cam);
}

//: Binary load camera from stream.
void vsl_b_read(vsl_b_istream & is, vpgl_camera_double_sptr &camera)
{
  vpgl_camera<double>* cam = nullptr;
   vsl_b_read(is, cam);
  camera = cam;
}

//: Binary load camera from stream.
void vsl_b_read(vsl_b_istream & is, vpgl_camera_float_sptr &camera)
{
  vpgl_camera<float>* cam = nullptr;
  vsl_b_read(is, cam);
  camera = cam;
}
