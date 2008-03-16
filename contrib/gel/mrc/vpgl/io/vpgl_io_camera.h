#ifndef vpgl_io_camera_h_
#define vpgl_io_camera_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include "../vpgl_camera.h"


//: Binary save camera to stream
template<class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_camera<T>* const& camera);


//: Binary load camera from stream.
template<class T>
void vsl_b_read(vsl_b_istream & is, vpgl_camera<T>* &camera);


#endif

