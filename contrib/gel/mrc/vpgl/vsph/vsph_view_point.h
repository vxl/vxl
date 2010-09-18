#ifndef vsph_view_point_h_
#define vsph_view_point_h_

#include <vcl_iostream.h>
#include <vpgl/vpgl_camera.h>
#include "vsph_spherical_coord.h"

template <class T_data>
class vsph_view_point
{
 public:
  vsph_view_point() : cam_(0), metadata_(0) {}
  vsph_view_point(vsph_sph_point_3d const& p) : cam_(0),spher_coord_(p),metadata_(0) {}
  vsph_view_point(vsph_sph_point_3d const& p,vpgl_camera_double_sptr cam) : cam_(cam), spher_coord_(p),metadata_(0) {}
  vsph_view_point(vsph_sph_point_3d const& p,vpgl_camera_double_sptr cam, T_data* metadata) : cam_(cam), spher_coord_(p), metadata_(metadata) {}
  ~vsph_view_point() {}

  void set_metadata(T_data* metadata) { metadata_=metadata; }
  T_data* metadata() { return metadata_; }
  const T_data* metadata() const { return metadata_; }

  void set_view_point(vsph_sph_point_3d p) { spher_coord_=p; }
  vsph_sph_point_3d view_point() const { return spher_coord_; }

  void print(vcl_ostream& os) const { os << " vsph_view_point: camera=" << cam_->type_name() << ",coordinates=" << spher_coord_ << ",data=" << *metadata_ << " " << vcl_endl; }

 private:
  vpgl_camera_double_sptr cam_;      // the camera looking from the view point
  vsph_sph_point_3d spher_coord_;    // spherical coordinates of the view point (radius, theta, phi)
  T_data* metadata_;
};

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, vsph_view_point<T> const& vp)
{
  vp.print(os);
  return os;
}

#endif
