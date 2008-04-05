#ifndef bvxm_lidar_camera_h_
#define bvxm_lidar_camera_h_
//:
// \file

#include <vcl_iosfwd.h>
#include <bgeo/bgeo_lvcs.h>
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_matrix.h>

#include "bvxm_world_params.h"

class bvxm_lidar_camera : public vpgl_camera<double>
{
 public:
  //: creates identity matrix and all zero tiepoints
  bvxm_lidar_camera();

  bvxm_lidar_camera(vnl_matrix<double> trans_matrix,
                    bgeo_lvcs_sptr lvcs,
                    vcl_vector<vcl_vector<double> > tiepoints)
    : trans_matrix_(trans_matrix), lvcs_(lvcs), tiepoints_(tiepoints), is_utm(false) {}

  // copy constructor
  bvxm_lidar_camera(bvxm_lidar_camera const& rhs);

  ~bvxm_lidar_camera() {}

  //northing=0 is North, 1 is east
  void set_utm(int utm_zone, unsigned northing) { is_utm=true, utm_zone_=utm_zone; northing_=northing; }

  //: Implementing the generic camera interface of vpgl_camera.
  //  x,y,z are in local coordinates, u represents image column, v image row
  void project(const double x, const double y, const double z, double& u, double& v) const;

  //: backprojects an image point into local coordinates (based on lvcs_)
  void backproject(const double u, const double v, double& x, double& y, double& z);

  bool operator ==(bvxm_lidar_camera const& rhs) const;

  //: Write camera to stream
  friend vcl_ostream&  operator<<(vcl_ostream& s, bvxm_lidar_camera const& p);

  //: Read camera  from stream
  friend vcl_istream&  operator>>(vcl_istream& s, bvxm_lidar_camera& p);

 private:

  //: lvcs of world parameters
  bgeo_lvcs_sptr lvcs_;
  vnl_matrix<double> trans_matrix_;           // 4x4 matrix
  vcl_vector<vcl_vector<double> > tiepoints_; // set of 6 values, normally 1 set
  bool is_utm;
  int utm_zone_;
  int northing_; //0 North, 1 South
  void img_to_wgs(const unsigned i, const unsigned j, double& lon, double& lat);
};

#endif

