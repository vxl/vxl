#ifndef vpgl_io_perspective_camera_h_
#define vpgl_io_perspective_camera_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vpgl/vpgl_perspective_camera.h>

//: Binary save camera to stream
template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_perspective_camera<T> const& camera);

//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_perspective_camera<T> &camera);

//: Print human readable summary of object to a stream
template <class T>
void vsl_print_summary(std::ostream& os,const vpgl_perspective_camera<T> & c);
#endif
