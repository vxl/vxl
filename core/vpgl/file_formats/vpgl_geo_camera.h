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
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/vpgl_lvcs.h>
#include <vnl/vnl_matrix.h>

#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_resource_sptr.h>

class vpgl_geo_camera : public vpgl_camera<double>
{
 public:
  //: creates identity matrix and all zero tiepoints
  vpgl_geo_camera();

  //: if scale tag is false be sure that trans_matrix[0][0] and trans_matrix[1][1] is 1.0 otherwise set it to true
  vpgl_geo_camera(vnl_matrix<double> trans_matrix,
                  vpgl_lvcs_sptr lvcs)
    : trans_matrix_(trans_matrix), is_utm(false), scale_tag_(false) {if(lvcs) this->set_lvcs(lvcs); }

  // copy constructor
  vpgl_geo_camera(vpgl_geo_camera const& rhs);

  vpgl_geo_camera(vpgl_camera<double> const& rhs);

  //: uses lvcs to convert local x-y to global longitude and latitude
  static bool init_geo_camera(vil_image_resource_sptr const& geotiff_img,
                              const vpgl_lvcs_sptr& lvcs,
                              vpgl_geo_camera*& camera);

  //: Assumes geographic coordinates are global
  static bool init_geo_camera(vil_image_resource_sptr const geotiff_img,
                              vpgl_geo_camera*& camera){
    vpgl_lvcs_sptr lvcs = nullptr;
    return init_geo_camera(geotiff_img, lvcs, camera);
  }

  //: warning, use this camera cautiously, the output of img_to_global method needs to be adjusted sign wise
  //  for 'S' use -lat and for 'W' -lon
  //  TODO: generalize geo_camera so that img_to_global method makes this adjustment internally if camera is created using this method
  static bool init_geo_camera(const std::string& img_name, unsigned ni, unsigned nj, const vpgl_lvcs_sptr& lvcs, vpgl_geo_camera*& camera);

  // loads a geo_camera from the file and uses global WGS84 coordinates, so no need to convert negative values to positives in the global_to_img method as in the previous method
  static bool init_geo_camera_from_filename(const std::string& img_name, unsigned ni, unsigned nj, const vpgl_lvcs_sptr& lvcs, vpgl_geo_camera*& camera);

  //: init using a tfw file, reads the transformation matrix from the tfw
  static bool init_geo_camera(const std::string& tfw_name, const vpgl_lvcs_sptr& lvcs, int utm_zone, unsigned northing, vpgl_geo_camera*& camera);

  //: init without lvcs. Assumes geographic coordinates are global
  static bool init_geo_camera(std::string tfw_name,  int utm_zone, unsigned northing, vpgl_geo_camera*& camera){
    vpgl_lvcs_sptr lvcs = nullptr;
    return init_geo_camera(tfw_name, lvcs, utm_zone, northing, camera);
  }
  ~vpgl_geo_camera() override = default;

  std::string type_name() const override { return "vpgl_geo_camera"; }

  //northing=0 means North, 1 is south
  void set_utm(int utm_zone, unsigned northing) { is_utm=true, utm_zone_=utm_zone; northing_=northing; }

  void set_lvcs(vpgl_lvcs_sptr lvcs) {lvcs_ = new vpgl_lvcs(*lvcs); }

  void set_scale_format(bool scale_tag) { scale_tag_=scale_tag; }

  vpgl_lvcs_sptr const lvcs() {return lvcs_;}

  //: Implementing the generic camera interface of vpgl_camera.
  //  x,y,z are in local coordinates, u represents image column, v image row
  void project(const double x, const double y, const double z, double& u, double& v) const override;

  //: backprojects an image point into local coordinates (based on lvcs_)
  void backproject(const double u, const double v, double& x, double& y, double& z);

  // adds translation to the trans matrix
  void translate(double tx, double ty, double z);

  //: the lidar pixel size in meters assumes square pixels
  double pixel_spacing() { if (scale_tag_) return trans_matrix_[0][0];
                           else return 1.0; }

  bool operator ==(vpgl_geo_camera const& rhs) const;

  static bool comp_trans_matrix(double sx1, double sy1, double sz1,
                                std::vector<std::vector<double> > tiepoints,
                                vnl_matrix<double>& trans_matrix,
                                bool scale_tag = false);

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const { return std::string("vpgl_geo_camera"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(std::string const& cls) const
  { return cls==is_a() || cls==std::string("vpgl_geo_camera"); }

  //: save the camera as tfw
  void save_as_tfw(std::string const& tfw_filename);

  //: Write camera to stream
  friend std::ostream&  operator<<(std::ostream& s, vpgl_geo_camera const& p);

  //: Read camera  from stream
  friend std::istream&  operator>>(std::istream& s, vpgl_geo_camera& p);

  //: returns the corresponding geographical coordinates for a given pixel position (i,j)
  //  the output global coord is wgs84
  void img_to_global(const double i, const double j,
                     double& lon, double& lat) const;

  //: returns the corresponding pixel position for given geographical coordinates
  //  the input global coord is wgs84
  void global_to_img(const double lon, const double lat, const double elev,
                     double& u, double& v) const;

  //: returns the corresponding geographical coordinates for a given pixel position (i,j)
  //  the output global coord is UTM: x east, y north
  void img_to_global_utm(const double i, const double j,
                         double& x, double& y) const;

  //: returns the corresponding pixel position for given geographical coordinates
  //  the input global coord is UTM: x east, for y north
  void global_utm_to_img(const double x, const double y, int zone, double elev,
                         double& u, double& v) const;

  //: returns the corresponding utm location for the given local position
  void local_to_utm(const double x, const double y, const double z, double& e, double& n, int& utm_zone);

  int utm_zone() { return utm_zone_; }

  bool img_four_corners_in_utm(const unsigned ni, const unsigned nj, double elev, double& e1, double& n1, double& e2, double& n2);

  //: returns the corresponding geographical coordinate (lon, lat, elev) for a given pixel position (i,j,k)
  //  Note: not yet implemented -- PVr, 16 aug 2012
  void img_to_wgs(unsigned i, unsigned j, unsigned k, double& lon, double& lat, double& elev);

  vnl_matrix<double>  trans_matrix(){return trans_matrix_; }

#if 0
  //: returns the corresponding pixel position (i,j) for a given geographical coordinate (lon, lat)
  void wgs_to_img(double lon, double lat,
                  unsigned& i, unsigned& j);
#endif // 0

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const { return 1; }

 private:

  vnl_matrix<double> trans_matrix_;           // 4x4 matrix
  //: lvcs of world parameters
  vpgl_lvcs_sptr lvcs_;
  bool is_utm;
  int utm_zone_;
  int northing_; //0 North, 1 South
  bool scale_tag_;
};

#endif // vpgl_geo_camera_h_
