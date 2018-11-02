#ifndef vsph_view_point_h_
#define vsph_view_point_h_

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/io/vpgl_io_perspective_camera.h>
#include "vsph_spherical_coord.h"
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_vector_3d.h>
template <class T_data>
class vsph_view_point
{
 public:
  vsph_view_point() : cam_(nullptr), metadata_(nullptr) {}
  vsph_view_point(vsph_sph_point_3d const& p) : cam_(nullptr),spher_coord_(p),metadata_(nullptr) {}
  vsph_view_point(vsph_sph_point_3d const& p,vpgl_camera_double_sptr cam) : cam_(cam), spher_coord_(p),metadata_(0) {}
  vsph_view_point(vsph_sph_point_3d const& p,vpgl_camera_double_sptr cam, T_data* metadata) : cam_(cam), spher_coord_(p), metadata_(metadata) {}

  void set_metadata(T_data* metadata) { metadata_=metadata; }
  T_data* metadata() { return metadata_; }
  const T_data* metadata() const { return metadata_; }

  void set_camera(vpgl_camera_double_sptr cam) { cam_=cam; }
  vpgl_camera_double_sptr camera() const { return cam_; }

  void set_view_point(vsph_sph_point_3d p) { spher_coord_=p; }
  vsph_sph_point_3d view_point() const { return spher_coord_; }


  void print(std::ostream& os) const { os << " vsph_view_point: camera=" << cam_->type_name() << ",coordinates=" << spher_coord_ << ",data=" << *metadata_ << " " << std::endl; }

  void relative_transf(vpgl_camera_double_sptr const& cam,
                       vgl_rotation_3d<double>& rel_rot,
                       vgl_vector_3d<double>& rel_trans) const;

   void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const { return 1; }

 private:
  vpgl_camera_double_sptr cam_;      // the camera looking from the view point
  vsph_sph_point_3d spher_coord_;    // spherical coordinates of the view point (radius, theta, phi)
  T_data* metadata_;
};

template <class T>
std::ostream& operator<<(std::ostream& os, vsph_view_point<T> const& vp)
{
  vp.print(os);
  return os;
}

template <class T>
void vsph_view_point<T>::b_read(vsl_b_istream& is)
{
  short version;
  vsl_b_read(is, version);
  switch (version) {
    case 1:
      vpgl_perspective_camera<double>* cam=new vpgl_perspective_camera<double>();
      if (cam) {
        vsl_b_read(is, *cam);
        cam_=cam;
        spher_coord_.b_read(is);
        metadata_ = new T();
        vsl_b_read(is, *metadata_);
      } else
        std::cout << "vsph_view_point<T>::b_write -- Camera type:" << cam_->type_name() << " is not supported yet!" << std::endl;
  }
}

template <class T>
void vsph_view_point<T>::b_write(vsl_b_ostream& os)
{
  vsl_b_write(os, version());
  vpgl_perspective_camera<double>* cam=dynamic_cast<vpgl_perspective_camera<double>*>(cam_.as_pointer());
  if (cam) {
    vsl_b_write(os, *cam);
    spher_coord_.b_write(os);
    vsl_b_write(os, *metadata_);
  } else
    std::cout << "vsph_view_point<T>::b_write -- Camera type:" << cam_->type_name() << " is not supported yet!" << std::endl;
}
template <class T>
void vsph_view_point<T>::
relative_transf(vpgl_camera_double_sptr const& cam,
                vgl_rotation_3d<double>& rel_rot,
                vgl_vector_3d<double>& rel_trans) const
{
  vpgl_perspective_camera<double>* c1=dynamic_cast<vpgl_perspective_camera<double>*>(cam.as_pointer());
  vpgl_perspective_camera<double>* c0=dynamic_cast<vpgl_perspective_camera<double>*>(cam_.as_pointer());
  if(!c0||!c1) return;
  vgl_vector_3d<double> t0 = c0->get_translation();
  vgl_vector_3d<double> t1 = c1->get_translation();
  vgl_rotation_3d<double> R0 = c0->get_rotation();
  vgl_rotation_3d<double> R1 = c1->get_rotation();
  rel_rot = R1*(R0.transpose());
  vgl_vector_3d<double> td = rel_rot*t0;
  rel_trans = -td + t1;
}
#endif
