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
 bpgl_geotif_camera():has_lvcs_(false), gcam_has_wgs84_cs_(false), elev_org_at_zero_(false){}
  
  virtual ~bpgl_geotif_camera() = default;
  
  //: factory methods to allow for failure conditions

  // if the lvcs is not defined then the general camera must be a rational camera with WGS84 CS,
  // or a local rational camera with an internal lvcs that defines the local Cartesian CS 
  // Notes: 1) input points may have been generated with CS elevation origin = 0. In this case
  //           the local rational camera lvcs elevation origin is correct. Otherwise and adjustment required
  //        2) the lvcs CS may not match the GEOTIFF header CS so extra internal conversion may be necessary,
  //           e.g., the lvcs CS is WGS84 and the GEOTIFF header CS is UTM
  //
  bool construct_from_geotif(vpgl_camera<T> const& general_cam, vil_image_resource_sptr resc, bool elev_org_at_zero = false,
                             vpgl_lvcs_sptr lvcs_ptr = nullptr);

  //: construct from a 4x4 transform matrix.
  //if the lvcs is not defined then the camera must be a RPC camera in the WGS84 CS
  //the UTM CS case is indicated by northing being either 0 - Northern Hemisphere or 1 - Southern Hemisphere
  //when the lvcs_ptr is null, UTM coordinates must be converted to WGS84 before projecting through the camera
  bool construct_from_matrix(vpgl_camera<T> const& general_cam, vnl_matrix<double> const& geo_transform_matrix,
                             bool elev_org_at_zero = false, vpgl_lvcs_sptr lvcs_ptr = nullptr,int northing = -1, int zone = 0);

  //: project from local or global 3-d coordinates, to an image location (u, v)
  // coordinates are local only if the lvcs was defined
  // otherwise the global coordinates are in the CS of the GEOTIFF metadata
  virtual void project(const T x, const T y, const T z, T& u, T& v) const;

  //: project from an image location in the GEOTIFF image to an image location (u, v),
  // as projected by the general camera.
  // tifz is the height value of the GEOTIFF image at (tifu, tifv)
  void project_gtif_to_image(const T tifu, const T tifv, const T tifz , T& u, T& v) const;

  //: legal C++ because the return type is covariant with vpgl_camera<T>*
  virtual bpgl_geotif_camera<T>* clone(void) const {return new bpgl_geotif_camera<T>(*this);}
 protected:

  //: the DSM is constructed with a zero elevation origin, i.e. local Cartesian elevation
  bool elev_org_at_zero_;

  //: the points to be projected are in a local CS
  bool has_lvcs_;

  //: the general camera to be used to project
  std::shared_ptr<vpgl_camera<T> > general_cam_;

  //: the general camera projects using WGS84 global coordinates
  bool gcam_has_wgs84_cs_;

  //: the local vertical CS to convert local to global
  vpgl_lvcs_sptr lvcs_;
  //: internal class to manage geographic coordinate transforms
  // and assist in projection from a DSM image to general cam image coordinates
  std::shared_ptr<vpgl_geo_camera> geo_cam_;
};

//: Write to stream
// \relatesalso bpgl_geotif_camera
template <class T>
std::ostream& operator<<(std::ostream& s, const bpgl_geotif_camera<T>& p);

//: Read from stream
// \relatesalso bpgl_geotif_camera
template <class T>
std::istream& operator>>(std::istream& is, bpgl_geotif_camera<T>& p);

#define BPGL_GEOTIF_CAMERA_INSTANTIATE(T) extern "please include vgl/bpgl_geotif_camera.txx first"


#endif // bpgl_geotif_camera_h_
