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


#include <vcl_iosfwd.h>
#include <vcl_vector.h>

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

  vpgl_geo_camera(vnl_matrix<double> trans_matrix,
                  vpgl_lvcs_sptr lvcs,
                  vcl_vector<vcl_vector<double> > /*tiepoints*/) // FIXME - unused parameter
    : trans_matrix_(trans_matrix), is_utm(false), scale_tag_(false) { this->set_lvcs(lvcs); }

  // copy constructor
  vpgl_geo_camera(vpgl_geo_camera const& rhs);

  vpgl_geo_camera(vpgl_camera<double> const& rhs);

  static bool init_geo_camera(vil_image_resource_sptr const geotiff_img,
                              vpgl_lvcs_sptr lvcs,
                              vpgl_geo_camera*& camera);

  ~vpgl_geo_camera() {}

  virtual vcl_string type_name() const { return "vpgl_geo_camera"; }

  //northing=0 means North, 1 is east
  void set_utm(int utm_zone, unsigned northing) { is_utm=true, utm_zone_=utm_zone; northing_=northing; }

  void set_lvcs(vpgl_lvcs_sptr lvcs) {lvcs_ = new vpgl_lvcs(*lvcs); }

  void set_scale_format(bool scale_tag) { scale_tag_=scale_tag; }

  vpgl_lvcs_sptr const lvcs() {return lvcs_;}

  //: Implementing the generic camera interface of vpgl_camera.
  //  x,y,z are in local coordinates, u represents image column, v image row
  void project(const double x, const double y, const double z, double& u, double& v) const;

  //: backprojects an image point into local coordinates (based on lvcs_)
  void backproject(const double u, const double v, double& x, double& y, double& z);

  // adds translation to the trans matrix
  void translate(double tx, double ty, double z);

  //: the lidar pixel size in meters assumes square pixels
  double pixel_spacing() { if (scale_tag_) return trans_matrix_[0][0];
                           else return 1.0; }

  bool operator ==(vpgl_geo_camera const& rhs) const;

  static bool comp_trans_matrix(double sx1, double sy1, double sz1,
                                vcl_vector<vcl_vector<double> > tiepoints,
                                vnl_matrix<double>& trans_matrix,
                                bool scale_tag = false);

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vpgl_geo_camera"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const
  { return cls==is_a() || cls==vcl_string("vpgl_geo_camera"); }

  //: Write camera to stream
  friend vcl_ostream&  operator<<(vcl_ostream& s, vpgl_geo_camera const& p);

  //: Read camera  from stream
  friend vcl_istream&  operator>>(vcl_istream& s, vpgl_geo_camera& p);

  //: returns the corresponding geographical coordinates for a given pixel position (i,j)
  void img_to_wgs(const unsigned i, const unsigned j, const unsigned z,
                  double& lon, double& lat, double& elev);

#if 0
  //: returns the corresponding pixel position (i,j) for a given geographical coordinates (lon, lat)
  void wgs_to_img(double lon, double lat,
                  unsigned& i, unsigned& j);
#endif // 0

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
