// This is bbas/bpgl/bpgl_lon_lat_camera.h
#ifndef bpgl_lon_lat_camera_h_
#define bpgl_lon_lat_camera_h_
//:
// \file
// \brief Loads an lon_lat image with geographic pixel locations
// \author Joseph Mundy
// \date November 22, 2019
//

#include <iostream>
#include <string>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_2d.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_view.h>
#include <vgl/algo/vgl_cremona_trans_2d.h>
#include <vpgl/vpgl_camera.h>
#include <vgl/vgl_polygon.h>
template <class T>
class bpgl_lon_lat_camera :  public vpgl_camera<T>
{
 public:
 bpgl_lon_lat_camera():ni_(-1), nj_(-1){}
 bool init(std::string const& lon_path, std::string const& lat_path);
 virtual ~bpgl_lon_lat_camera() = default;
 bool fit_geo_to_uv_map();
 bool save_error_image(std::string const& path);
 //: camera interface
 virtual std::string type_name() const { return "bpgl_lon_lat_camera"; }
 virtual void project(const T x, const T y, const T z, T& u, T& v) const;
 virtual bpgl_lon_lat_camera<T>* clone(void) const {return new bpgl_lon_lat_camera(*this);}
 vgl_box_2d<T> geo_bb(){return geo_bb_;}
 vgl_polygon<T> geo_boundary(){return geo_boundary_;}
 protected:
  vgl_cremona_trans_2d<double,7> ct_;
 size_t ni_;
 size_t nj_;
 vil_image_view<float> lon_;
 vil_image_view<float> lat_;
 vgl_box_2d<T> geo_bb_;
 vgl_polygon<T> geo_boundary_;
 vil_image_view<float> err_;
};
#endif // bpgl_lon_lat_camera_h_
#define BPGL_LON_LAT_CAMERA_INSTANTIATE(T) extern "please include vgl/bpgl_lon_lat_camera.txx first"
