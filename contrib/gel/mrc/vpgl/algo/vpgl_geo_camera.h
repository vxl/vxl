#ifndef vpgl_geo_camera_h_
#define vpgl_geo_camera_h_
//:
// \file
// \brief A geotiff image deduced camera class.
// \author Gamze Tunali
// \date 10/24/2008
// \author Gamze Tunali
//
//   Geotiff images contain information about the corresponding world coordinate 
//   values of the pixel positions. This class is a wwapper of that information
//   to project and backproject the 3D points in local coordinates to pixel positions


#include <vcl_iosfwd.h>
#include <vcl_vector.h>

#include <vpgl/bgeo/bgeo_lvcs_sptr.h>
#include <vnl/vnl_matrix.h>

#include <vpgl/vpgl_camera.h>
#include <vsl/vsl_binary_io.h>

class vpgl_geo_camera : public vpgl_camera<double>
{
 public:
  //: creates identity matrix and all zero tiepoints
  vpgl_geo_camera();

  vpgl_geo_camera(vnl_matrix<double> trans_matrix,
                    bgeo_lvcs_sptr lvcs,
                    vcl_vector<vcl_vector<double> > tiepoints)
    : trans_matrix_(trans_matrix), lvcs_(lvcs), 
    tiepoints_(tiepoints), is_utm(false) {}

  // copy constructor
  vpgl_geo_camera(vpgl_geo_camera const& rhs);

  vpgl_geo_camera(vpgl_camera<double> const& rhs);

  ~vpgl_geo_camera() {}

  virtual vcl_string type_name() const { return "vpgl_geo_camera"; }

  //northing=0 means North, 1 is east
  void set_utm(int utm_zone, unsigned northing) { is_utm=true, utm_zone_=utm_zone; northing_=northing; }

  void set_lvcs(bgeo_lvcs_sptr lvcs) {lvcs_ = lvcs;}

  bgeo_lvcs_sptr const lvcs() {return lvcs_;}

  //: Implementing the generic camera interface of vpgl_camera.
  //  x,y,z are in local coordinates, u represents image column, v image row
  void project(const double x, const double y, const double z, double& u, double& v) const;

  //: backprojects an image point into local coordinates (based on lvcs_)
  void backproject(const double u, const double v, double& x, double& y, double& z);

  // adds translation to the trans matrix
  void translate(double tx, double ty);

  bool operator ==(vpgl_geo_camera const& rhs) const;

  static bool comp_trans_matrix(double sx1, double sy1, double sz1,
                                vcl_vector<vcl_vector<double> > tiepoints,
                                vnl_matrix<double>& trans_matrix);

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const { return vcl_string("vpgl_geo_camera"); }

  //: Return true if the argument matches the string identifying the class or any parent class
  virtual bool is_class(vcl_string const& cls) const
  { return cls==is_a() || cls==vcl_string("vpgl_camera"); }

  // binary IO
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);

  //: Write camera to stream
  friend vcl_ostream&  operator<<(vcl_ostream& s, vpgl_geo_camera const& p);

  //: Read camera  from stream
  friend vcl_istream&  operator>>(vcl_istream& s, vpgl_geo_camera& p);
  
  void img_to_wgs(const unsigned i, const unsigned j, double& lon, double& lat);

 private:

  vnl_matrix<double> trans_matrix_;           // 4x4 matrix
  //: lvcs of world parameters
  bgeo_lvcs_sptr lvcs_;
  //: set of 6 values, normally 1 set
  vcl_vector<vcl_vector<double> > tiepoints_; 
  bool is_utm;
  int utm_zone_;
  int northing_; //0 North, 1 South
  
};

#endif // vpgl_geo_camera

