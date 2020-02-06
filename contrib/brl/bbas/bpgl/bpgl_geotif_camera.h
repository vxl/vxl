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
#include <memory>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_resource.h>
#include <vnl/vnl_matrix.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/vpgl_camera.h>

// templated since may want to project <float> points
template <class T>
class bpgl_geotif_camera : vpgl_camera<T>
{
 public:
  //: default constructor - may want a container of cameras
  //  the default member values represent the most common case, e.g. a local_rational_camera
  bpgl_geotif_camera()
    : has_lvcs_(true), gcam_has_wgs84_cs_(true),
      elev_org_at_zero_(true), is_utm_(false),
      project_local_points_(true), scale_defined_(false),
      projection_enabled_(false) {}

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
  //        3) The geographic bounds of a geotif camera are determined from: a) the image bounds or b)from the DSM bounds or
  //           c) from RPC scale and offset bounds, in this order, when defined
  //
  bool construct_from_geotif(vpgl_camera<T> const& general_cam, vil_image_resource_sptr resc,
                             vgl_box_2d<T> const& image_bounds = vgl_box_2d<T>(), bool elev_org_at_zero = false,
                             vpgl_lvcs_sptr lvcs_ptr = nullptr);

  //: construct from a 4x4 transform matrix.
  // if the lvcs is not defined then the camera must be a RPC camera in the WGS84 CS
  // the UTM CS case is indicated by hemisphere flag being either 0 - Northern Hemisphere or 1 - Southern Hemisphere
  // when the lvcs_ptr is null, UTM coordinates must be converted to WGS84 before projecting through the camera
  // The geographic bounds of a geotif camera are determined from: a) the image bounds or b)from the DSM bounds or
  //  c) from RPC scale and offset bounds, in this order, when defined
  bool construct_from_matrix(vpgl_camera<T> const& general_cam, vnl_matrix<T> const& geo_transform_matrix,
                             vgl_box_2d<T> const& image_bounds = vgl_box_2d<T>(), bool elev_org_at_zero = false,
                             vpgl_lvcs_sptr lvcs_ptr = nullptr,int hemisphere_flag = -1, int zone = 0);

  //: construct without a camera to provide geographic transforms
  //  when projection functions are not needed
  bool construct_geo_data_only(vil_image_resource_sptr resc);

  //: are the input points in a local CS ?
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
  // in some versions of DSM algorithms the elevations are relative to
  // the local CS, i.e. the origin for z is 0;
  bool elevation_origin_at_zero() const {return elev_org_at_zero_;}

  //: is the cs of the DSM UTM?
  bool is_utm() const {return is_utm_;}

  //: does the general camera used for projection have a lvcs?
  bool has_lvcs() const {return has_lvcs_;}

  //: is the general camera coordinate system wgs84?
  bool general_cam_has_wgs84_cs() const {return gcam_has_wgs84_cs_;}

  //: return a pointer to the general camera, e.g. a local rational camera
  std::shared_ptr<vpgl_camera<T> > general_camera() const {return general_cam_;}

  //: return the lvcs pointer if defined, otherwise null
  vpgl_lvcs_sptr lvcs_ptr() {return lvcs_ptr_;}

  //: the geographic matrix extracted from the geotiff header
  vnl_matrix<T> matrix() const {return matrix_;}

  //: spacing between dsm samples in meters
  T dsm_spacing() const {return dsm_spacing_;}

  //: utm CS information
  int utm_zone() const {return utm_zone_;}
  int hemisphere_flag() const {return hemisphere_flag_;}

  //: construct a lvcs at the lower_left corner of the geotiff image array
  // can be either wgs84 or utm depending on CS of geotiff image
  vpgl_lvcs_sptr lower_left_lvcs(T elev_ll = T(0)) const;

  //: if not empty, defines the geographic region of validity of the camera
  // both a polygonal region and an enclosing bounding box are provided
  vgl_box_2d<T> geo_bb() const {return geo_bb_;}
  vgl_polygon<T> geo_boundary() const {return geo_boundary_;}

  //: =====Transforms between DSM image and global geo coordinates====
  // [e.g., (u,v)->(lon, lat) or (lon, lat, elev)->(u,v)]

  //: map dsm image location to global geo X-Y coordinates (wgs84 or UTM)
  void dsm_to_global(T i, T j, T& gx, T& gy) const;

  //: map global geo X-Y (wgs84 or UTM) to dsm u,v (uses GEOTIFF matrix)
  void global_to_dsm(T gx, T gy, T& i, T& j) const;

  //=====================================================================
 protected:
  //: extract the geographic matrix from the geotiff header
  bool construct_matrix(T sx, T sy, T sz, std::vector<std::vector<T> > tiepoints);

  //: intialize geographic info from the geotiff header
  bool init_from_geotif(vil_image_resource_sptr const& resc);

  //: map local coordinate to global coordinates
  bool local_to_global(T lx, T ly, T lz, T& gx, T& gy, T& gz) const;
  //: the opposite direction
  bool global_to_local(T gx, T gy, T gz, T& lx, T& ly, T& lz) const;

  //: the elevation value at the CS origin
  T elevation_origin() const;

  //: set the DSM spacing using the wgs CS - convert deg./pix to meters/pix
  bool set_spacing_from_wgs_matrix();

  // set the geographic bounds given a rational camera
  static bool geo_bounds_from_rational_cam(vpgl_camera<T>* rat_cam_ptr,  vgl_box_2d<T> const& image_bounds,
                                           vgl_box_2d<T>& geo_bb, vgl_polygon<T>& geo_boundary);

  // set the geographic bounds when the general camera is a local camera (e.g. affine)
  bool geo_bounds_from_local_cam(std::shared_ptr<vpgl_camera<T> >const& lcam_ptr);

  //:a general camera is defined, e.g. RPC
  bool projection_enabled_;

  //: the dsm grid spacing in meters
  T dsm_spacing_;

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
  vgl_box_2d<T> image_bounds_;
  vgl_box_2d<T> geo_bb_;
  vgl_polygon<T> geo_boundary_;
};

#define BPGL_GEOTIF_CAMERA_INSTANTIATE(T) extern "please include vgl/bpgl_geotif_camera.txx first"


#endif // bpgl_geotif_camera_h_
