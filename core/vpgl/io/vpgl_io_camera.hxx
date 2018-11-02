#ifndef vpgl_io_camera_hxx_
#define vpgl_io_camera_hxx_

#include <string>
#include <iostream>
#include "vpgl_io_camera.h"
//:
// \file
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/io/vpgl_io_proj_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include <vpgl/io/vpgl_io_affine_camera.h>
#include <vpgl/io/vpgl_io_rational_camera.h>
#include <vpgl/io/vpgl_io_local_rational_camera.h>

#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Binary save camera to stream
template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_camera<T>* const& camera)
{
  if ( camera->type_name() == "vpgl_proj_camera" ){
    // projective camera
    vpgl_proj_camera<T>* procam = static_cast<vpgl_proj_camera<T>*>(camera);
    vsl_b_write(os,procam->type_name());
    vsl_b_write(os,*procam);
  }
  else if ( camera->type_name() == "vpgl_perspective_camera" ) {
    // perspective camera
    vpgl_perspective_camera<T>* percam =
      static_cast<vpgl_perspective_camera<T>*>(camera);
    vsl_b_write(os,percam->type_name());
    vsl_b_write(os,*percam);
  }
  else if ( camera->type_name() == "vpgl_affine_camera" ) {
    // affine camera
    vpgl_affine_camera<T>* affcam =
      static_cast<vpgl_affine_camera<T>*>(camera);
    vsl_b_write(os,affcam->type_name());
    vsl_b_write(os,*affcam);
  }
  else if ( camera->type_name() == "vpgl_rational_camera" ) {
    // rational camera
    vpgl_rational_camera<T>* ratcam =
      static_cast<vpgl_rational_camera<T>*>(camera);
    vsl_b_write(os,ratcam->type_name());
    vsl_b_write(os,*ratcam);
  }
  else if ( camera->type_name() == "vpgl_local_rational_camera" ) {
    // local rational camera
    vpgl_local_rational_camera<T>* lratcam =
      static_cast<vpgl_local_rational_camera<T>*>(camera);
    vsl_b_write(os,lratcam->type_name());
    vsl_b_write(os,*lratcam);
  }
  else {
    std::cerr << "tried to write unknown camera type!\n";
    std::string cam_type("unknown");
    vsl_b_write(os,cam_type);
  }
  return;
}


//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_camera<T>* &camera)
{
  std::string cam_type;
  vsl_b_read(is,cam_type);

  if (cam_type == "vpgl_proj_camera") {
    // projective camera
    vpgl_proj_camera<T>* procam = new vpgl_proj_camera<T>();
    vsl_b_read(is,*procam);
    camera = procam;
  }
  else if (cam_type == "vpgl_perspective_camera") {
    // perspective camera
    vpgl_perspective_camera<T>* percam = new vpgl_perspective_camera<T>();
    vsl_b_read(is,*percam);
    camera = percam;
  }
  else if (cam_type == "vpgl_affine_camera") {
    // rational camera
    vpgl_affine_camera<T>* affcam = new vpgl_affine_camera<T>();
    vsl_b_read(is,*affcam);
    camera = affcam;
  }
  else if (cam_type == "vpgl_rational_camera") {
    // rational camera
    vpgl_rational_camera<T>* ratcam = new vpgl_rational_camera<T>();
    vsl_b_read(is,*ratcam);
    camera = ratcam;
  }
  else if (cam_type == "vpgl_local_rational_camera") {
    // rational camera
    vpgl_local_rational_camera<T>* lratcam=new vpgl_local_rational_camera<T>();
    vsl_b_read(is,*lratcam);
    camera = lratcam;
  }
  else if (cam_type == "unknown") {
    std::cerr << "cannot read camera of unknown type!\n";
  }
  else {
    std::cerr << "error reading vpgl_camera!\n";
  }
  return;
}

#define VPGL_IO_CAMERA_INSTANTIATE(T) \
template void vsl_b_read(vsl_b_istream &, vpgl_camera<T >* &); \
template void vsl_b_write(vsl_b_ostream &, vpgl_camera<T >* const&)

#endif // vpgl_io_camera_hxx_
