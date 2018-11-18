#ifndef vpgl_io_camera_h_
#define vpgl_io_camera_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vpgl/vpgl_camera_float_sptr.h>

//: Binary save camera to stream
template<class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_camera<T>* const& camera);

//: Binary load camera from stream.
template<class T>
void vsl_b_read(vsl_b_istream & is, vpgl_camera<T>* &camera);

//: Binary save camera sptr to stream
void vsl_b_write(vsl_b_ostream & os, vpgl_camera_double_sptr const& cam_sptr);
void vsl_b_write(vsl_b_ostream & os, vpgl_camera_float_sptr const& cam_sptr);

//: Binary load camera from stream.
void vsl_b_read(vsl_b_istream & is, vpgl_camera_double_sptr &camera);
void vsl_b_read(vsl_b_istream & is, vpgl_camera_float_sptr &camera);

#endif
