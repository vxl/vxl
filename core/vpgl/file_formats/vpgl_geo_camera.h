#ifndef vpgl_geo_camera_h_
#define vpgl_geo_camera_h_
//:
// \file
// \brief A geotiff image deduced camera class.
// \author Gamze Tunali
// \date October 24, 2008
// \author Gamze Tunali
//
// Geotiff images contain information about the corresponding world coordinate
// values of the pixel positions. This class is a wrapper of that information
// to project and backproject the 3D points in local coordinates to pixel positions


#include <iosfwd>
#include <utility>

#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/vpgl_lvcs.h>
#include <vnl/vnl_matrix.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_config.h> // defines HAS_GEOTIFF
#if HAS_GEOTIFF
#  include <vil/vil_image_resource_sptr.h>
#endif

class vpgl_geo_camera : public vpgl_camera<double>
{
public:
  //: creates identity matrix and all zero tiepoints
  vpgl_geo_camera();

  //: if scale tag is false be sure that trans_matrix[0][0] and trans_matrix[1][1] is 1.0 otherwise set it to true
  vpgl_geo_camera(vnl_matrix<double> trans_matrix, vpgl_lvcs_sptr lvcs)
    : trans_matrix_(std::move(trans_matrix))
    , is_utm_(false)
    , scale_tag_(false)
    , sx_(0.0)
    , sy_(0.0)
  {
    if (lvcs)
      this->set_lvcs(lvcs);
  }

  // copy constructor
  vpgl_geo_camera(const vpgl_geo_camera & rhs);

  vpgl_geo_camera(const vpgl_camera<double> & rhs);

#if HAS_GEOTIFF
  // Load camera from geotiff file
  bool
  load_from_geotiff(const std::string & file, const vpgl_lvcs * lvcs = nullptr);

  //: Load camera from image resource
  bool
  load_from_resource(const vil_image_resource_sptr & geotiff_img, const vpgl_lvcs * lvcs = nullptr);

  //: uses lvcs to convert local x-y to global longitude and latitude
  static bool
  init_geo_camera(const vil_image_resource_sptr & geotiff_img, const vpgl_lvcs_sptr & lvcs, vpgl_geo_camera *& camera);

  //: Assumes geographic coordinates are global
  static bool
  init_geo_camera(const vil_image_resource_sptr geotiff_img, vpgl_geo_camera *& camera)
  {
    vpgl_lvcs_sptr lvcs = nullptr;
    return init_geo_camera(geotiff_img, lvcs, camera);
  }
#endif

  //: Load camera from GDAL geotransform
  bool
  load_from_geotransform(std::array<double, 6> geotransform,
                         int utm_zone = -1,
                         int northing = 0, // 0 North, 1 South
                         const vpgl_lvcs * lvcs = nullptr);

  //: warning, use this camera cautiously, the output of img_to_global method needs to be adjusted sign wise
  //  for 'S' use -lat and for 'W' -lon
  //  TODO: generalize geo_camera so that img_to_global method makes this adjustment internally if camera is created
  //  using this method
  static bool
  init_geo_camera(const std::string & img_name,
                  unsigned ni,
                  unsigned nj,
                  const vpgl_lvcs_sptr & lvcs,
                  vpgl_geo_camera *& camera);

  // loads a geo_camera from the file and uses global WGS84 coordinates, so no need to convert negative values to
  // positives in the global_to_img method as in the previous method
  static bool
  init_geo_camera_from_filename(const std::string & img_name,
                                unsigned ni,
                                unsigned nj,
                                const vpgl_lvcs_sptr & lvcs,
                                vpgl_geo_camera *& camera);

  //: init using a tfw file, reads the transformation matrix from the tfw
  static bool
  init_geo_camera(const std::string & tfw_name,
                  const vpgl_lvcs_sptr & lvcs,
                  int utm_zone,
                  unsigned northing,
                  vpgl_geo_camera *& camera);

  //: init without lvcs. Assumes geographic coordinates are global
  static bool
  init_geo_camera(std::string tfw_name, int utm_zone, unsigned northing, vpgl_geo_camera *& camera)
  {
    vpgl_lvcs_sptr lvcs = nullptr;
    return init_geo_camera(tfw_name, lvcs, utm_zone, northing, camera);
  }
  ~vpgl_geo_camera() override = default;

  std::string
  type_name() const override
  {
    return "vpgl_geo_camera";
  }

  // northing=0 means North, 1 is south
  void
  set_utm(int utm_zone, unsigned northing)
  {
    is_utm_ = true, utm_zone_ = utm_zone;
    northing_ = northing;
  }

  void
  set_lvcs(const vpgl_lvcs * lvcs)
  {
    lvcs_ = (lvcs) ? lvcs->clone() : nullptr;
  }
  void
  set_lvcs(const vpgl_lvcs_sptr & lvcs)
  {
    lvcs_ = (lvcs) ? lvcs->clone() : nullptr;
  }

  void
  set_scale_format(bool scale_tag)
  {
    scale_tag_ = scale_tag;
  }

  const vpgl_lvcs_sptr
  lvcs()
  {
    return lvcs_;
  }

  double
  lvcs_elev_origin() const;

  //: convert local coordinates to global coordinates in the geo_camera CS
  void
  local_to_global(double lx, double ly, double lz, double & gx, double & gy, double & gz) const;

  //: convert global coordinates in the geo_camera CS to local coordinates
  bool
  global_to_local(double gx, double gy, double gz, double & lx, double & ly, double & lz) const;

  //: Implementing the generic camera interface of vpgl_camera.
  //  x,y,z are in local coordinates, u represents image column, v image row
  void
  project(const double x, const double y, const double z, double & u, double & v) const override;

  //: backprojects an image point into local coordinates (based on lvcs_)
  void
  backproject(const double u, const double v, double & x, double & y, double & z) const;

  // adds translation to the trans matrix
  void
  translate(double tx, double ty, double z);

  //: determine the pixel spacing
  void
  extract_pixel_size();

  //: the lidar pixel size in meters if pixel is square (mean of xy spacing)
  double
  pixel_spacing() const
  {
    return 0.5 * (sx_ + sy_);
  }

  //: the lidar pixel size in meters, general case
  void
  pixel_spacing(double & sx, double & sy) const
  {
    sx = sx_;
    sy = sy_;
  }

  bool
  operator==(const vpgl_geo_camera & rhs) const;

  static bool
  comp_trans_matrix(double sx1,
                    double sy1,
                    double sz1,
                    std::vector<std::vector<double>> tiepoints,
                    vnl_matrix<double> & trans_matrix,
                    bool scale_tag = false);

  bool
  is_utm() const
  {
    return is_utm_;
  }

  //: Return a platform independent string identifying the class
  virtual std::string
  is_a() const
  {
    return std::string("vpgl_geo_camera");
  }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool
  is_class(const std::string & cls) const
  {
    return cls == is_a() || cls == std::string("vpgl_geo_camera");
  }

  //: save the camera as tfw
  void
  save_as_tfw(const std::string & tfw_filename);

  //: Write camera to stream
  friend std::ostream &
  operator<<(std::ostream & s, const vpgl_geo_camera & p);

  //: Read camera  from stream
  friend std::istream &
  operator>>(std::istream & s, vpgl_geo_camera & p);

  //: returns the corresponding geographical coordinates for a given pixel position (i,j)
  //  the output global coord is wgs84
  void
  img_to_global(const double i, const double j, double & lon, double & lat) const;

  //: returns the corresponding pixel position for given geographical coordinates
  //  the input global coord is wgs84
  void
  global_to_img(const double lon, const double lat, const double elev, double & u, double & v) const;

  //: returns the corresponding geographical coordinates for a given pixel position (i,j)
  //  the output global coord is UTM: x east, y north
  void
  img_to_global_utm(const double i, const double j, double & x, double & y) const;

  //: returns the corresponding pixel position for given geographical coordinates
  //  the input global coord is UTM: x east, for y north
  void
  global_utm_to_img(const double x, const double y, int zone, double elev, double & u, double & v) const;

  //: returns the corresponding utm location for the given local position
  void
  local_to_utm(const double x, const double y, const double z, double & e, double & n, int & utm_zone) const;

  int
  utm_zone() const
  {
    return utm_zone_;
  }
  int
  utm_northing() const
  {
    return northing_;
  }

  bool
  img_four_corners_in_utm(const unsigned ni,
                          const unsigned nj,
                          double elev,
                          double & e1,
                          double & n1,
                          double & e2,
                          double & n2) const;

  //: returns the corresponding geographical coordinate (lon, lat, elev) for a given pixel position (i,j,k)
  //  Note: not yet implemented -- PVr, 16 aug 2012
  void
  img_to_wgs(unsigned i, unsigned j, unsigned k, double & lon, double & lat, double & elev) const;

  vnl_matrix<double>
  trans_matrix() const
  {
    return trans_matrix_;
  }

  //: since vpgl_geo_camera is not templated only vpgl_camera<double>* is covariant with vpgl_camera<T>*
  vpgl_geo_camera *
  clone() const override
  {
    return new vpgl_geo_camera(*this);
  }
#if 0
  //: returns the corresponding pixel position (i,j) for a given geographical coordinate (lon, lat)
  void wgs_to_img(double lon, double lat,
                  unsigned& i, unsigned& j);
#endif // 0

  //: Binary save self to stream.
  void
  b_write(vsl_b_ostream & os) const;

  //: Binary load self from stream.
  void
  b_read(vsl_b_istream & is);

  //: Return IO version number;
  short
  version() const
  {
    return 1;
  }

private:
  // x and y pixel spacing in meters
  double sx_;
  double sy_;
  vnl_matrix<double> trans_matrix_; // 4x4 matrix
  //: lvcs of world parameters
  vpgl_lvcs_sptr lvcs_ = nullptr;
  bool is_utm_ = false;
  int utm_zone_ = 0;
  int northing_ = 0; // 0 North, 1 South
  bool scale_tag_ = false;
};

#if HAS_GEOTIFF
//: Create a vpgl_geo_camera from a geotiff file
vpgl_geo_camera
load_geo_camera_from_geotiff(const std::string & file, const vpgl_lvcs * lvcs = nullptr);

//: Create a vpgl_geo_camera from a vil_image_resource_sptr & optional LVCS
vpgl_geo_camera
load_geo_camera_from_resource(const vil_image_resource_sptr & geotiff_img, const vpgl_lvcs * lvcs = nullptr);
#endif

//: Create a vpgl_geo_camera from GDAL geotransform
// https://gdal.org/user/raster_data_model.html#affine-geotransform
vpgl_geo_camera
load_geo_camera_from_geotransform(std::array<double, 6> geotransform,
                                  int utm_zone = -1,
                                  int northing = 0, // 0 North, 1 South
                                  const vpgl_lvcs * lvcs = nullptr);


#endif // vpgl_geo_camera_h_
