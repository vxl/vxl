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
#include <vil/file_formats/vil_geotiff_header.h>
#include <vil/file_formats/vil_tiff.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
bool bpgl_geotif_camera<T>::construct_matrix(T sx, T sy, T sz, std::vector<std::vector<T> > tiepoints){
  matrix_.set_size(4, 4);
  T fv = T(0);
  matrix_.fill(fv);
  // use tiepoints and scale values to create a transformation matrix
  // for now use the first tiepoint if there are more than one
  bool valid_matrix = (tiepoints.size() > 0) && (tiepoints[0].size() == 6);
  if (!valid_matrix) {
      std::cerr << "GEOTIFF matrix not valid" << std::endl;
      return false;
 }
  T I = tiepoints[0][0];
  T J = tiepoints[0][1];
  T K = tiepoints[0][2];
  T X = tiepoints[0][3];
  T Y = tiepoints[0][4];
  T Z = tiepoints[0][5];

  // Define a transformation matrix as follows:
  //
  //      |-                         -|
  //      |   Sx    0.0   0.0   Tx    |
  //      |                           |      Tx = X - I*Sx
  //      |   0.0  -Sy    0.0   Ty    |      Ty = Y + J*Sy
  //      |                           |      Tz = Z - K*Sz
  //      |   0.0   0.0   Sz    Tz    |
  //      |                           |
  //      |   0.0   0.0   0.0   1.0   |
  //      |-                         -|

  T Tx = X - I*sx;
  T Ty = Y + J*sy;
  T Tz = Z - K*sz;

  matrix_[0][0] = sx;
  matrix_[1][1] = -1*sy;
  matrix_[2][2] = sz;
  matrix_[3][3] = 1.0;
  matrix_[0][3] = Tx;
  matrix_[1][3] = Ty;
  matrix_[2][3] = Tz;
  return true;
}
                                             
template <class T>
bool bpgl_geotif_camera<T>::init_from_geotif(vil_image_resource_sptr const& resc){
  // check if the image is GEOTIFF
  auto* geotiff_tiff = dynamic_cast<vil_tiff_image*> (resc.ptr());
  if (!geotiff_tiff) {
    std::cerr << "Can't cast resource to TIFF" << std::endl;
    return false;
  }

  // check if the tiff file is geotiff
  if (!geotiff_tiff->is_GEOTIFF()) {
    std::cerr << "TIFF image is not GEOTIFF!\n";
    return false;
  }
  // retrieve header
  vil_geotiff_header* gtif = geotiff_tiff->get_geotiff_header();
  if (!gtif) {
    std::cerr << "no geotiff header! "<< std::endl;
    return false;
  }

  vil_geotiff_header::GTIF_HEMISPH h;

  std::vector < std::vector<double> > dtiepoints;
  gtif->gtif_tiepoints(dtiepoints);

  // create a transformation matrix
  // if there is a transformation matrix in GEOTIFF, use that
  double* matrix_dvalues;
  double dsx = 1.0, dsy = 1.0, dsz = 1.0;
  scale_defined_ = false;
  if (gtif->gtif_trans_matrix(matrix_dvalues)) { 
    std::cout << "Transfer matrix is given, using that...." << std::endl;
    T vals[16];
    for (size_t i = 0; i < 16; ++i)
        vals[i] = static_cast<T>(matrix_dvalues[i]);
    matrix_.copy_in(vals);
  }else if (gtif->gtif_pixelscale(dsx, dsy, dsz)) {
    scale_defined_ = true;
    T sx = static_cast<T>(dsx);
    T sy = static_cast<T>(dsy);
    T sz = static_cast<T>(dsz);
    if (dtiepoints.size() == 0) {
        std::cerr << "null tiepoint array - fatal" << std::endl;
        return false;
    }
    std::vector<std::vector<T> > tiepoints(dtiepoints.size(), std::vector<T>(dtiepoints[0].size()));
    for (size_t r = 0; r < dtiepoints.size(); ++r)
        for (size_t c = 0; c < dtiepoints[r].size(); ++c)
            tiepoints[r][c] = static_cast<T>(dtiepoints[r][c]);
    if(!this->construct_matrix(sx, sy, sz, tiepoints))
        return false;
  }
  else {
    std::cout << "Transform matrix cannot be formed..\n";
    return false;
  }
  
  // check if the model type is geographic and also the units
  if (gtif->GCS_WGS84_MET_DEG()) {
    is_utm_ = false;
    return true;
  }

  // otherwise check if it is projected to UTM and figure out the zone
  if (gtif->PCS_WGS84_UTM_zone(utm_zone_, h) || gtif->PCS_NAD83_UTM_zone(utm_zone_, h))
  {
      hemisphere_flag_ = static_cast<int>(h);
    is_utm_ = true;
    return true;
  }
  std::cout << "Only PCS_WGS84_UTM , PCS_NAD83_UTM, and GCS_WGS_84 with linear units in meters, "
            << "angular units in degrees are supported" << std::endl;
  return false;
}
//--------------------------------------
// factory constructors
//
template <class T>
bool bpgl_geotif_camera<T>::construct_from_geotif(vpgl_camera<T> const& general_cam, vil_image_resource_sptr resc, bool elev_org_at_zero, vpgl_lvcs_sptr lvcs_ptr) {
  elev_org_at_zero_ = elev_org_at_zero;
  has_lvcs_ = static_cast<bool>(lvcs_ptr);

  //case I - a camera which projects global geo coordinates - no local CS
  // TO DO - there might exist a UTM camera in the future
  if(general_cam.is_a() == "vpgl_rational_camera"&& !lvcs_ptr){
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    gcam_has_wgs84_cs_= true;
    project_local_points_ = false;
    this->init_from_geotif(resc);
    return true;
  }
  // Case II - camera is local rational camera and points are in its local CS
  // possibly with a elevation offset required if points have a z=0 reference elevation
  // rather than global z
  if(general_cam.is_a() == "vpgl_local_rational_camera"&&!lvcs_ptr){
   const vpgl_local_rational_camera<T>& lrcam = dynamic_cast<const vpgl_local_rational_camera<T>&>(general_cam);
   gcam_has_wgs84_cs_= true;
   lvcs_ptr_ = new vpgl_lvcs(lrcam.lvcs());
   has_lvcs_ = true;
   project_local_points_ = true;
   const vpgl_rational_camera<T>& rcam = dynamic_cast<const vpgl_rational_camera<T>&>(lrcam);
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(new vpgl_rational_camera<T>(rcam));
    this->init_from_geotif(resc);
    return true;
  }
  // Case III - camera is local and lvcs is specified, input points are in the lvcs global CS
  if(lvcs_ptr){
   has_lvcs_ = true;
   lvcs_ptr_ = lvcs_ptr;
   gcam_has_wgs84_cs_= false;
   project_local_points_ = false;
   general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
   this->init_from_geotif(resc);
   return true;
  }

  std::cout << "camera is local but no lvcs specified - fatal" << std::endl;
  return false;
}
template <class T>
bool bpgl_geotif_camera<T>::construct_from_matrix(vpgl_camera<T> const& general_cam, vnl_matrix<T> const& geo_transform_matrix, bool elev_org_at_zero, vpgl_lvcs_sptr lvcs_ptr, int hemisphere_flag , int zone)
  {
   scale_defined_ = true;
   elev_org_at_zero_ = elev_org_at_zero;
   lvcs_ptr_ = lvcs_ptr;
  if(general_cam.is_a() == "vpgl_rational_camera"&& !lvcs_ptr){
    project_local_points_ = false;
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    gcam_has_wgs84_cs_= true;
    matrix_ = geo_transform_matrix;
    if(hemisphere_flag>=0){
      is_utm_ = true;
      utm_zone_ = zone;
      hemisphere_flag_ = hemisphere_flag;
    }
    return true;
  }
  // Case II - camera is local rational camera and points are in its local CS
  // possibly with a elevation offset required if points have a z=0 reference elevation
  // rather than global z
  if(general_cam.is_a() == "vpgl_local_rational_camera"&&!lvcs_ptr){
   const vpgl_local_rational_camera<T>& lrcam = dynamic_cast<const vpgl_local_rational_camera<T>&>(general_cam);
    vpgl_lvcs_sptr lr_lvcs_ptr = new vpgl_lvcs(lrcam.lvcs());
    const vpgl_rational_camera<T>& rcam = dynamic_cast<const vpgl_rational_camera<T>&>(lrcam);
    project_local_points_ = true;
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(new vpgl_rational_camera<T>(rcam));
    has_lvcs_ = true;
    gcam_has_wgs84_cs_= true;
    lvcs_ptr_ = lr_lvcs_ptr;
    matrix_ = geo_transform_matrix;
    if(hemisphere_flag >=0){
    is_utm_ = true;
      utm_zone_ = zone;
      hemisphere_flag_ = hemisphere_flag;
    }
    return true;
  }
  // Case III - camera is local and lvcs is specified - input points are in a global geographic CS
  if(lvcs_ptr){
    lvcs_ptr_ = lvcs_ptr;
    has_lvcs_ = true;
    gcam_has_wgs84_cs_= false;
    general_cam_ = std::shared_ptr<vpgl_camera<T> >(general_cam.clone());
    matrix_ = geo_transform_matrix;
    if(hemisphere_flag >=0){
    is_utm_ = true;
      utm_zone_ = zone;
      hemisphere_flag_ = hemisphere_flag;
    }
    project_local_points_ = false;
    return true;
  }
  std::cout << "camera is local but no lvcs specified - fatal" << std::endl;
  return false;
}
template <class T>
bool bpgl_geotif_camera<T>::local_to_global(T lx, T ly, T lz, T& gx, T& gy, T& gz) const{
    double dlx = static_cast<double>(lx);
    double dly = static_cast<double>(ly);
    double dlz = static_cast<double>(lz);
    double dgx, dgy, dgz;
  if (lvcs_ptr_) {
    if (lvcs_ptr_->get_cs_name() == vpgl_lvcs::utm) {
      if (is_utm_) {  // this CS is utm so keep using utm
        lvcs_ptr_->local_to_global(dlx, dly, dlz, vpgl_lvcs::utm, dgx, dgy, dgz);
      }else {  // this CS is not UTM, convert to wgs84 as global
        lvcs_ptr_->local_to_global(dlx, dly, dlz, vpgl_lvcs::wgs84, dgx, dgy,dgz);
      }
    }else {
      lvcs_ptr_->local_to_global(dlx, dly, dlz, vpgl_lvcs::wgs84, dgx, dgy, dgz);
    }
    gx = static_cast<T>(dgx);
    gy = static_cast<T>(dgy);
    gz = static_cast<T>(dgz);
    return true;
  }
  std::cerr << "global coordinate system not defined" << std::endl;
  return false;
}
template <class T>
bool bpgl_geotif_camera<T>::global_to_local(T gx, T gy, T gz, T& lx, T& ly, T& lz) const {
  if(!lvcs_ptr_){
    std::cout << "No local vertical CS defined - can't map local to global" << std::endl;
    return false;
  }
  double dgx = static_cast<double>(gx);
  double dgy = static_cast<double>(gy);
  double dgz = static_cast<double>(gz);
  double dlx, dly, dlz; 
  bool good = false;
  if(lvcs_ptr_->get_cs_name() == vpgl_lvcs::utm){
    lvcs_ptr_->global_to_local(dgx, dgy, dgz, vpgl_lvcs::utm, dlx, dly, dlz);
    good = true;
  } else if(lvcs_ptr_->get_cs_name() == vpgl_lvcs::wgs84){
    lvcs_ptr_->global_to_local(dgx, dgy, dgz, vpgl_lvcs::wgs84, dlx, dly, dlz);
    good = true;
  }
  if (good) {
      lx = static_cast<T>(dlx);
      ly = static_cast<T>(dly);
      lz = static_cast<T>(dlz);
      return  true;
  }
  std::cout << "lvcs name " << lvcs_ptr_->get_cs_name() << " not handled " << std::endl;
  return false;
}
template <class T>
void bpgl_geotif_camera<T>::project(const T x, const T y, const T z, T& u, T& v) const{

  // case I - The camera and the input points have the same CS
  if(!has_lvcs_){
    general_cam_->project(x, y, z, u, v);
    return;
  }
  // case II - the camera is global WGS 84,
  // and input points are in a local CS
  if (has_lvcs_&& gcam_has_wgs84_cs_) {
    T gx, gy, gz;
    T zadj = z;
    if(!elev_org_at_zero_){
      T elev_org = this->elevation_origin();
      zadj = static_cast<T>(zadj - elev_org);
    }
    this->local_to_global(x, y, zadj, gx, gy, gz);
    general_cam_->project(T(gx), T(gy), T(gz), u, v);
    return;
  }
  // case III - the camera is global
  // and input points are in a global CS
  if (has_lvcs_) {
    T lx, ly, lz;
    if(!this->global_to_local(x, y, z, lx, ly, lz)){
      u = T(-1); v = T(-1);
      return;
    }
    general_cam_->project(T(lx), T(ly), T(lz), u, v);
    return;
  }
  std::cerr << "invalid projection" << std::endl;
  u = T(-1); v = T(-1);
}
//
// use the 4x4 GEOTIFF matrix to transform DSM image coordinates to global coordinates
// either UTM or WGS84
template <class T>
void bpgl_geotif_camera<T>::image_to_global(T i, T j, T& gx, T& gy) const{
  vnl_vector<T> v(4), res(4);
   if (scale_defined_) {
    v[0] = matrix_[0][3] + i*matrix_[0][0];
    v[1] = matrix_[1][3] + j*matrix_[1][1];
  }
  else {
    v[0] = matrix_[0][3] + i;
    v[1] = matrix_[1][3] - j;
  }
   gx = v[0]; gy = v[1];
}

// get the global elevation origin to handle the case where the input points
// were generated with a local z elevation origin of zero. This reference elevation
// is correct if the camera is a local camera by which the input points were
// generated. E.g. a local affine camera. Otherwise, the global offset of the lvcs
// has to be added to the input point elevations before projecting.
template <class T>
T bpgl_geotif_camera<T>::elevation_origin() const{
  if(!lvcs_ptr_)
    return T(0);
  double ox, oy, oz;
  if(is_utm_){
    int temp;
    lvcs_ptr_->get_utm_origin(ox, oy, oz, temp);
    return oz;
  }
  lvcs_ptr_->get_origin(oy, ox, oz);
  return oz;
}

template <class T>
void bpgl_geotif_camera<T>::project_gtif_to_image(const T tifu, const T tifv, const T tifz, T& u, T& v) const {
  if (has_lvcs_ && gcam_has_wgs84_cs_ && !is_utm_) {
    T lat = 0.0, lon = 0.0;
    this->image_to_global(tifu, tifv, lon, lat);
    T zadj = tifz;
    if(elev_org_at_zero_){
      T elev_org = this->elevation_origin();
      zadj = static_cast<T>(zadj + elev_org);
    }
    general_cam_->project(T(lon), T(lat), zadj, u, v);
    return;
  }
  // geotiff is in UTM so conversion is required before projecting 
  // through the WGS84 camera
  if (has_lvcs_ && gcam_has_wgs84_cs_ && is_utm_) {
      T east = 0.0, north = 0.0;
      this->image_to_global(tifu, tifv, east, north);
      bool south_flag = hemisphere_flag_ > 0;
      T zadj = tifz;
      if (elev_org_at_zero_) {
        T elev_org = this->elevation_origin();
        zadj = static_cast<T>(zadj + elev_org);
      }
      T lat = 0.0, lon = 0.0, elev = 0.0;
      double deast = static_cast<double>(east);
      double dnorth = static_cast<double>(north);
      double dzadj = static_cast<double>(zadj);
      double dlat, dlon, delev;
      vpgl_utm utm;
      utm.transform(utm_zone_, deast, dnorth, dzadj, dlat, dlon, delev, south_flag);
      general_cam_->project(T(dlon), T(dlat), T(delev), u, v);
      return;
  }
  std::cout << "improper camera or geographic CS to project DSM points" << std::endl;
}

// Code for easy instantiation.
#undef BPGL_GEOTIF_CAMERA_INSTANTIATE
#define BPGL_GEOTIF_CAMERA_INSTANTIATE(T) \
template class bpgl_geotif_camera<T >



#endif // bpgl_geotif_camera_hxx_
