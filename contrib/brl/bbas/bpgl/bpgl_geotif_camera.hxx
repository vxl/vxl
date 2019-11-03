// This is bbas/bpgl/bpgl_geotif_camera.hxx
#ifndef bpgl_geotif_camera_hxx_
#define bpgl_geotif_camera_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "bpgl_geotif_camera.h"
#include <vpgl/vpgl_utm.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
//--------------------------------------
// factory constructors
//
template <class T>
bool bpgl_geotif_camera<T>::construct_from_geotif(vpgl_camera<T> const& general_cam, vil_image_resource_sptr resc, bool elev_org_at_zero, vpgl_lvcs_sptr lvcs_ptr) {
    elev_org_at_zero_ = elev_org_at_zero;
    vpgl_geo_camera* geo_cam = nullptr;
  has_lvcs_ = static_cast<bool>(lvcs_ptr);

  //case I - a camera which projects global geo coordinates - no local CS
  // TO DO - there might exist a UTM camera in the future
  if(general_cam.is_a() == "vpgl_rational_camera"&& !lvcs_ptr){
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    vpgl_geo_camera::init_geo_camera(resc, lvcs_ptr, geo_cam);
    geo_cam_ = std::shared_ptr<vpgl_geo_camera>(geo_cam);
    gcam_has_wgs84_cs_= true;
    return true;
  }
  // Case II - camera is local rational camera and points are in its local CS
  // possibly with a elevation offset required if points have a z=0 reference elevation
  // rather than global z
  if(general_cam.is_a() == "vpgl_local_rational_camera"&&!lvcs_ptr){
   const vpgl_local_rational_camera<T>& lrcam = dynamic_cast<const vpgl_local_rational_camera<T>&>(general_cam);
    vpgl_lvcs_sptr lr_lvcs_ptr = new vpgl_lvcs(lrcam.lvcs());
    const vpgl_rational_camera<T>& rcam = dynamic_cast<const vpgl_rational_camera<T>&>(lrcam);
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(new vpgl_rational_camera<T>(rcam));
    vpgl_geo_camera::init_geo_camera(resc, lr_lvcs_ptr, geo_cam);
    has_lvcs_ = true;
    gcam_has_wgs84_cs_= true;
    geo_cam_ = std::shared_ptr<vpgl_geo_camera>(geo_cam);
    return true;
  }
  // Case III - camera is local and lvcs is specified, input points are in a global CS
  if(lvcs_ptr){
   has_lvcs_ = true;
   gcam_has_wgs84_cs_= false;
   general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
   vpgl_geo_camera::init_geo_camera(resc, lvcs_ptr, geo_cam);
   geo_cam_ = std::shared_ptr<vpgl_geo_camera>(geo_cam);
    return true;
  }
  std::cout << "camera is local but no lvcs specified - fatal" << std::endl;
  return false;
}
template <class T>
bool bpgl_geotif_camera<T>::construct_from_matrix(vpgl_camera<T> const& general_cam, vnl_matrix<double> const& geo_transform_matrix, bool elev_org_at_zero, vpgl_lvcs_sptr lvcs_ptr, int northing , int zone)
  {
   elev_org_at_zero_ = elev_org_at_zero;
  vpgl_geo_camera* geo_cam = nullptr;
  if(general_cam.is_a() == "vpgl_rational_camera"&& !lvcs_ptr){
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    gcam_has_wgs84_cs_= true;
    geo_cam_ = std::shared_ptr<vpgl_geo_camera>(new vpgl_geo_camera(geo_transform_matrix, lvcs_ptr));
    if(northing>=0)
      geo_cam_->set_utm(zone, northing);
    geo_cam_->set_scale_format(true);
    return true;
  }
  // Case II - camera is local rational camera and points are in its local CS
  // possibly with a elevation offset required if points have a z=0 reference elevation
  // rather than global z
  if(general_cam.is_a() == "vpgl_local_rational_camera"&&!lvcs_ptr){
   const vpgl_local_rational_camera<T>& lrcam = dynamic_cast<const vpgl_local_rational_camera<T>&>(general_cam);
    vpgl_lvcs_sptr lr_lvcs_ptr = new vpgl_lvcs(lrcam.lvcs());
    const vpgl_rational_camera<T>& rcam = dynamic_cast<const vpgl_rational_camera<T>&>(lrcam);
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(new vpgl_rational_camera<T>(rcam));
    has_lvcs_ = true;
    gcam_has_wgs84_cs_= true;
    geo_cam_ = std::shared_ptr<vpgl_geo_camera>(new vpgl_geo_camera(geo_transform_matrix, lr_lvcs_ptr));
    if(northing>=0)
      geo_cam_->set_utm(zone, northing);
    geo_cam_->set_scale_format(true);
    return true;
  }
  // Case III - camera is local and lvcs is specified - input points are in a global geographic CS
  if(lvcs_ptr){
    has_lvcs_ = true;
    gcam_has_wgs84_cs_= false;
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    geo_cam_ = std::shared_ptr<vpgl_geo_camera>(new vpgl_geo_camera(geo_transform_matrix, lvcs_ptr));
    if(northing>=0)
      geo_cam_->set_utm(zone, northing);
    geo_cam_->set_scale_format(true);
    return true;
  }
  std::cout << "camera is local but no lvcs specified - fatal" << std::endl;
  return false;
}

template <class T>
void bpgl_geotif_camera<T>::project(const T x, const T y, const T z, T& u, T& v) const{

  // case I - The camera and the input points have the same CS
  if(!geo_cam_||!has_lvcs_){
    general_cam_->project(x, y, z, u, v);
    return;
  }
  // case II - the camera is global WGS 84,
  // and input points are in a local CS
  if (geo_cam_ && has_lvcs_&& gcam_has_wgs84_cs_) {
    double gx, gy, gz;
    T zadj = z;
    if(!elev_org_at_zero_){
      double elev_org = geo_cam_->lvcs_elev_origin();
      zadj = static_cast<T>(zadj - elev_org);
    }
    geo_cam_->local_to_global(x, y, zadj, gx, gy, gz);
    general_cam_->project(T(gx), T(gy), T(gz), u, v);
    return;
  }
  // case III - the camera is not global
  // and input points are in a global CS
  if (geo_cam_ && has_lvcs_) {
    double lx, ly, lz;
    if(!geo_cam_->global_to_local(x, y, z, lx, ly, lz)){
      u = T(-1); v = T(-1);
      return;
    }
    general_cam_->project(T(lx), T(ly), T(lz), u, v);
    return;
  }
}

template <class T>
void bpgl_geotif_camera<T>::project_gtif_to_image(const T tifu, const T tifv, const T tifz, T& u, T& v) const {
  if (geo_cam_ && has_lvcs_ && gcam_has_wgs84_cs_ && !geo_cam_->is_utm()) {
    double lat = 0.0, lon = 0.0;
    geo_cam_->img_to_global(tifu, tifv, lon, lat);
    T zadj = tifz;
    if(elev_org_at_zero_){
      double elev_org = geo_cam_->lvcs_elev_origin();
      zadj = static_cast<T>(zadj + elev_org);
    }
    general_cam_->project(T(lon), T(lat), zadj, u, v);
    return;
  }
  // geotiff is in UTM so conversion is required
  if (geo_cam_ && has_lvcs_ && gcam_has_wgs84_cs_ && geo_cam_->is_utm()) {
      double east = 0.0, north = 0.0;
      geo_cam_->img_to_global_utm(tifu, tifv, east, north);
      vpgl_utm utm;
      int northing = geo_cam_->utm_northing();
      bool south_flag = northing > 0;
      T zadj = tifz;
      if (elev_org_at_zero_) {
          double elev_org = geo_cam_->lvcs_elev_origin();
          zadj = static_cast<T>(zadj + elev_org);
      }
      double lat = 0.0, lon = 0.0, elev = 0.0;
      int zone = geo_cam_->utm_zone();
      utm.transform(zone, east, north, zadj, lat, lon, elev, south_flag);
      general_cam_->project(T(lon), T(lat), T(elev), u, v);
      return;
  }
  std::cout << "improper camera or geographic CS to project DSM points" << std::endl;
}

//: write to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, const bpgl_geotif_camera<T >& c )
{
  return s;
}
//: read from stream
template <class T>
std::istream&  operator>>(std::istream& s, const bpgl_geotif_camera<T >& c )
{
  return s;
}

// Code for easy instantiation.
#undef BPGL_GEOTIF_CAMERA_INSTANTIATE
#define BPGL_GEOTIF_CAMERA_INSTANTIATE(T) \
template class bpgl_geotif_camera<T >; \
template std::ostream& operator<<(std::ostream&, const bpgl_geotif_camera<T >&); \
template std::istream& operator>>(std::istream&, const bpgl_geotif_camera<T >&)


#endif // bpgl_geotif_camera_hxx_
