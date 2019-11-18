// This is bbas/bpgl/bpgl_geotif_camera.h
#ifndef bpgl_geotif_camera_h_
#define bpgl_geotif_camera_h_
//:
// \file
// \brief Converts a vpgl_camera to a geographic camera based on geotiff header information
// \author Joseph Mundy
// \date November 1, 2019
//
// The motivation is that 3-d data can  be represented as a geotif image, or a point cloud derived from
// the geotif image. It is convenient to be able to project a pixel from the geotif image into another image
// based on a general camera viewpoint with respect to the geotif data. Similarly, a 3-d point in a point cloud
// derived from the geotif data, or in some cases used to create the geotif data, can be projected with respect to a general
// camera view.
//
//  the geotif header defines the mapping from geotif image coordinates to 2-d geographic coordinates,
//  and the image values define the global elevation at each 2-d location, i.e., z(x,y)
// 
//  for the point cloud, a lvcs is required to relate the point coordinates to geographic coordinates, not necessarily
//  in the same as the geographic coordinate system as the geotif image.
//
#include <iostream>
#include <string>
#include <memory>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vil/vil_image_resource.h>
// templated since may want to project <float> points
template <class T>
class bpgl_geotif_camera : vpgl_camera<T>
{
 public:
  //: default constructor - may want a container of cameras
  //  the default member values represent the most common case, e.g. a local_rational_camera
 bpgl_geotif_camera():has_lvcs_(true), gcam_has_wgs84_cs_(true),
    elev_org_at_zero_(true), is_utm_(false), project_local_points_(true),scale_defined_(false){}
  
  virtual ~bpgl_geotif_camera() = default;
  
  //: factory methods to allow for failure conditions

  // if the lvcs is not defined (null) then the general camera must be a rational camera with WGS84 CS,
  // or a local rational camera with an internal lvcs that defines the local Cartesian CS 
  // Notes: 1) input points may have been generated with local CS elevation origin = 0. In this case
  //           the local rational camera lvcs elevation origin is correct, and is used by default.
  //           If the elevation origin is global due to post processing, elev_org_at_zero = false and
  //           the necessary adjustment to elevation values is made.
  //        2) the lvcs CS may not match the GEOTIFF header CS so extra internal conversion may be necessary,
  //           e.g., the lvcs CS is WGS84 and the GEOTIFF header CS is UTM
  //
  bool construct_from_geotif(vpgl_camera<T> const& general_cam, vil_image_resource_sptr resc, bool elev_org_at_zero = false,
                             vpgl_lvcs_sptr lvcs_ptr = nullptr);

  //: construct from a 4x4 transform matrix.
  //if the lvcs is not defined then the camera must be a RPC camera in the WGS84 CS
  //the UTM CS case is indicated by hemisphere flag being either 0 - Northern Hemisphere or 1 - Southern Hemisphere
  //when the lvcs_ptr is null, UTM coordinates must be converted to WGS84 before projecting through the camera
  bool construct_from_matrix(vpgl_camera<T> const& general_cam, vnl_matrix<T> const& geo_transform_matrix,
                             bool elev_org_at_zero = false, vpgl_lvcs_sptr lvcs_ptr = nullptr,int hemisphere_flag = -1, int zone = 0);

  //: are the input points in a local CS
  bool project_local_points() const {return project_local_points_;}

  //: project from local or global 3-d coordinates, to an image location (u, v)
  // if project_local_points() == true, coordinates are in a local CS otherwise in a global CS 
  virtual void project(const T x, const T y, const T z, T& u, T& v) const;

  //: project from an image location in the GEOTIFF image to an image location (u, v),
  // as projected by the general camera. tifz is the height value of the GEOTIFF image at (tifu, tifv)
  void project_gtif_to_image(const T tifu, const T tifv, const T tifz , T& u, T& v) const;

  //: legal C++ because the return type is covariant with vpgl_camera<T>*
  virtual bpgl_geotif_camera<T>* clone(void) const {return new bpgl_geotif_camera<T>(*this);}

  //: accessors
  bool elevation_origin_at_zero() const {return elev_org_at_zero_;}
  bool is_utm() const {return is_utm_;}
  bool has_lvcs() const {return has_lvcs_;}
  bool general_cam_has_wgs84_cs() const {return gcam_has_wgs84_cs_;}
  std::shared_ptr<vpgl_camera<T> > general_camera() const {return general_cam_;}
  vpgl_lvcs_sptr lvcs_ptr() {return lvcs_ptr_;}
  vnl_matrix<T> matrix() const {return matrix_;}
 protected:
  bool construct_matrix(T sx, T sy, T sz, std::vector<std::vector<T> > tiepoints);
  bool init_from_geotif(vil_image_resource_sptr const& resc);
  bool local_to_global(T lx, T ly, T lz, T& gx, T& gy, T& gz) const;
  bool global_to_local(T gx, T gy, T gz, T& lx, T& ly, T& lz) const;
  void image_to_global(T i, T j, T& gx, T& gy) const;
  T elevation_origin() const;

  //: if true, the input points are in a local CS, or if false in a global CS
  bool project_local_points_;

  //: the DSM is constructed with a zero elevation origin, i.e. local Cartesian elevation
  bool elev_org_at_zero_;

  //: the points to be projected are in a local CS
  bool has_lvcs_;

  //: the general camera to be used to project
  std::shared_ptr<vpgl_camera<T> > general_cam_;

  //: the general camera projects using WGS84 global coordinates
  bool gcam_has_wgs84_cs_;

  //: the local vertical CS to convert local to global
  vpgl_lvcs_sptr lvcs_ptr_;

  vnl_matrix<T> matrix_;
  bool scale_defined_;
  bool is_utm_;
  int utm_zone_;
  int hemisphere_flag_; //0 North, 1 South
};

#define BPGL_GEOTIF_CAMERA_INSTANTIATE(T) extern "please include vgl/bpgl_geotif_camera.txx first"


#endif // bpgl_geotif_camera_h_
