#include "vpgl_io_camera.h"
#include "vpgl_io_rational_camera.h"
#include "vpgl_io_proj_camera.h"
#include "../vpgl_camera.h"
#include "../vpgl_proj_camera.h"
#include "../vpgl_rational_camera.h"
#include "../vpgl_local_rational_camera.h"

#include <vsl/vsl_binary_io.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


//: Binary save camera to stream
template <class T>
void vsl_b_write(vsl_b_ostream & os, vpgl_camera<T>* const& camera)
{
  if ( vpgl_proj_camera<T> *procam = dynamic_cast<vpgl_proj_camera<T>*>(camera) ){
    // projective camera
    vcl_string cam_type("vpgl_proj_camera");
    vsl_b_write(os,cam_type);
    vsl_b_write(os,*procam);

  }else if ( vpgl_rational_camera<T> *ratcam = dynamic_cast<vpgl_rational_camera<T>*>(camera) ) {
    // rational camera
    vcl_string cam_type("vpgl_rational_camera");
    vsl_b_write(os,cam_type);
    vsl_b_write(os,*ratcam);
  }else if ( vpgl_local_rational_camera<T> *lratcam = dynamic_cast<vpgl_local_rational_camera<T>*>(camera) ) {
    // local rational camera
    vcl_string cam_type("vpgl_local_rational_camera");
    vsl_b_write(os,cam_type);
    vsl_b_write(os,*lratcam);
  }else {
    vcl_cerr << "tried to write unknown camera type!" << vcl_endl;
    vcl_string cam_type("unknown");
    vsl_b_write(os,cam_type);
  }
  return;
}


//: Binary load camera from stream.
template <class T>
void vsl_b_read(vsl_b_istream & is, vpgl_camera<T>* &camera)
{
  vcl_string cam_type;
  vsl_b_read(is,cam_type);

  if (cam_type == "vpgl_proj_camera") {
    // projective camera
    vpgl_proj_camera<T>* procam = new vpgl_proj_camera<T>();
    vsl_b_read(is,*procam);
    camera = procam;
  } else if (cam_type == "vpgl_rational_camera") {
    // rational camera
    vpgl_rational_camera<T>* ratcam = new vpgl_rational_camera<T>();
    vsl_b_read(is,*ratcam);
    camera = ratcam;
  } else if (cam_type == "vpgl_local_rational_camera") {
    // rational camera
    vpgl_local_rational_camera<T>* lratcam=new vpgl_local_rational_camera<T>();
    vsl_b_read(is,*lratcam);
    camera = lratcam;
  }else if (cam_type == "unknown") {
    vcl_cerr << "cannot read camera of unknown type!" << vcl_endl;
  }
  else {
    vcl_cerr << "error reading vpgl_camera!" << vcl_endl;
  }
  return;
}

#define VPGL_IO_CAMERA_INSTANTIATE(T) \
template void vsl_b_read(vsl_b_istream &, vpgl_camera<T>* &); \
template void vsl_b_write(vsl_b_ostream &, vpgl_camera<T>* const&);

