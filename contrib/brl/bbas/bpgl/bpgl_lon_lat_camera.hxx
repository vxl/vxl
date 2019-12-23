// This is bbas/bpgl/bpgl_lon_lat_camera.hxx
#ifndef bpgl_lon_lat_camera_hxx_
#define bpgl_lon_lat_camera_hxx_
#include "bpgl_lon_lat_camera.h"
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vgl/algo/vgl_compute_cremona_2d.h>
template <class T>
bool bpgl_lon_lat_camera<T>::init(std::string const& lon_path, std::string const& lat_path) {
  lon_ = vil_load(lon_path.c_str());
  ni_  = lon_.ni(); nj_ = lon_.nj();
  if(ni_==0 || nj_ == 0){
    std::cerr << "couldn't load valid longitude image from " << lon_path << std::endl;
    return false;
  }
  lat_ = vil_load(lat_path.c_str());
  size_t nila = lat_.ni(), njla = lat_.nj();
  if(nila != ni_ || njla != nj_){
    std::cerr << "invalid latitude image loaded from " << lat_path << std::endl;
    return false;
  } 
  std::vector<vgl_point_2d<T> > verts;
  T lon_00 = lon_(0,0), lat_00 = lat_(0,0);
  T lon_01 = lon_(0,(nj_-1)), lat_01 = lat_(0,(nj_-1));
  T lon_11 = lon_((ni_-1),(nj_-1)), lat_11 = lat_((ni_-1),(nj_-1));
  T lon_10 = lon_((ni_-1), 0), lat_10 = lat_((ni_-1),0);
  verts.emplace_back(lon_00, lat_00);  verts.emplace_back(lon_01, lat_01);
  verts.emplace_back(lon_11, lat_11);  verts.emplace_back(lon_10, lat_10);
  geo_boundary_.push_back(verts);

  for(size_t i = 0; i<verts.size(); ++i)
      geo_bb_.add(verts[i]);

  return true;
}
template <class T>
bool bpgl_lon_lat_camera<T>::fit_geo_to_uv_map(){
  size_t inc = 1;
  std::vector<vgl_homg_point_2d<double> > geo_pts;
  std::vector<vgl_homg_point_2d<double> > img_pts;
  for(size_t j = 0; j<nj_; j+=inc)
    for(size_t i = 0; i<ni_; i+=inc){
      double di = static_cast<double>(i), dj = static_cast<double>(j);
      geo_pts.emplace_back(lon_(i,j), lat_(i,j));
      img_pts.emplace_back(di, dj);
    }
  vgl_compute_cremona_2d<double, 7> cc;
  bool good = cc.compute_linear(geo_pts, img_pts, vgl_compute_cremona_2d<double, 7>::UNITY_DENOMINATOR);
  if(!good)
    return false;
  err_.set_size(ni_, nj_);
  std::cout << "Solution error deg = 1 unity denominator " << cc.linear_error() << std::endl;
  vgl_cremona_trans_2d<double, 7> ct = cc.linear_trans();
  for (size_t j = 0; j < nj_; j += 1)
      for (size_t i = 0; i < ni_; i += 1) {
          vgl_point_2d<double> p(i, j);
          double lon = lon_(i, j), lat = lat_(i, j);
          vgl_point_2d<double> pg(lon, lat);
          vgl_point_2d<double> pi = ct(pg);
          float err = (p - pi).length();
          err_(i, j) = err;
      }
  ct_ = cc.linear_trans();
  return true;
}

template <class T>
bool bpgl_lon_lat_camera<T>::save_error_image(std::string const& path){
  return vil_save(err_, path.c_str());
}
// x is lon y is lat for now z doesn't matter
template <class T>
void bpgl_lon_lat_camera<T>::project(const T x, const T y, const T z, T& u, T& v) const{
  double dx = static_cast<double>(x), dy = static_cast<double>(y), dz = static_cast<double>(z), du = 0.0, dv = 0.0;
  ct_.project(dx, dy, du, dv);
  u = static_cast<T>(du); v = static_cast<T>(dv);
}
// Code for easy instantiation.
#undef BPGL_LON_LAT_CAMERA_INSTANTIATE
#define BPGL_LON_LAT_CAMERA_INSTANTIATE(T) \
template class bpgl_lon_lat_camera<T >
#endif
