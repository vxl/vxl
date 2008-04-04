#include "bvxm_lidar_camera.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>

#include <bgeo/bgeo_utm.h>

bvxm_lidar_camera::bvxm_lidar_camera()
{
  trans_matrix_.set_size(4,4);
  trans_matrix_.fill(0);
  trans_matrix_.fill_diagonal(1);
  trans_matrix_[1][1] = -1;

  tiepoints_.resize(1);
  tiepoints_[0].resize(6,0);

  lvcs_=new bgeo_lvcs();
}

bvxm_lidar_camera::bvxm_lidar_camera(bvxm_lidar_camera const& rhs)
{
  this->trans_matrix_ = rhs.trans_matrix_;
  this->lvcs_ = new bgeo_lvcs(*(rhs.lvcs_));
  this->tiepoints_ = rhs.tiepoints_;
}

//: transforms a given 3d world point to image plane
void bvxm_lidar_camera::project(const double x, const double y, const double z, 
                                double& u, double& v) const
{
  vnl_vector<double> vec(4), res(4);
  double lat, lon, gz;
  lvcs_->local_to_global(x, y, z, bgeo_lvcs::wgs84, lon, lat, gz);
  bgeo_utm utm;
  //double lat, lon, elev;
  double x1, y1;
  int utm_zone;
  utm.transform(lat, lon, x1, y1, utm_zone);
  vec[0] = x1;
  vec[1] = y1;
  vec[2] = 0;
  vec[3] = 1;

  // do we really need this, const does not allow this
  vnl_matrix<double> tm(trans_matrix_);
  tm[2][2] = 1;

  //vcl_cout << trans_matrix_ << vcl_endl;
  vnl_matrix<double> trans_matrix_inv = vnl_inverse(tm);
  res = trans_matrix_inv*vec;
  //vcl_cout << res[0] << ' ' << res[1] << vcl_endl;
  u = res[0];
  v = res[1];
}

//: backprojects an image point into local coordinates (based on lvcs_)
void bvxm_lidar_camera::backproject(const double u, const double v, 
                             double& x, double& y, double& z)
{
  vnl_vector<double> vec(4), res(4);
  vec[0] = tiepoints_[0][3] + u;
  vec[1] = tiepoints_[0][4] - v;
  vec[2] = 0;
  vec[3] = 1;

  //vcl_cout << "Northing=" << v[0] << " Easting=" << v[1];

  //find the UTM values
  bgeo_utm utm;
  double lat, lon, elev;

  utm.transform(38, vec[0], vec[1], vec[2], lat, lon, elev);
  lvcs_->global_to_local(lon, lat, elev, bgeo_lvcs::wgs84, x, y, z);
  //z = img_view_(u, v);
}

void bvxm_lidar_camera::img_to_wgs(const unsigned i, const unsigned j,
                                     double& lon, double& lat)
{
  vnl_vector<double> v(4), res(4);
  v[0] = tiepoints_[0][3] + i;
  v[1] = tiepoints_[0][4] - j;
  v[2] = 0;
  v[3] = 1;
  //find the UTM values
  bgeo_utm utm;
  double elev;
  utm.transform(38, v[0], v[1], v[2], lat, lon, elev);
}

bool bvxm_lidar_camera::operator==(bvxm_lidar_camera const& rhs) const
{
  if ((this->trans_matrix_ == rhs.trans_matrix_) && (*(this->lvcs_) == *(rhs.lvcs_)))
    return true;
  else 
    return false;
}

//: Write vpgl_perspective_camera to stream
vcl_ostream&  operator<<(vcl_ostream& s,
                         bvxm_lidar_camera const& p)
{
  s << p.trans_matrix_ << "\n";
  s << p.tiepoints_[0][0] << "\n";
  s << p.tiepoints_[0][1] << "\n";
  s << p.tiepoints_[0][2] << "\n";
  s << p.tiepoints_[0][3] << "\n";
  s << p.tiepoints_[0][4] << "\n";
  s << p.tiepoints_[0][5] << "\n";
  s << *(p.lvcs_) << "\n";

  return s ;
}

//: Read vpgl_perspective_camera from stream
vcl_istream&  operator>>(vcl_istream& s,
                         bvxm_lidar_camera& p)
{
  vnl_matrix_fixed<double,4,4> tr_matrix;
  s >> tr_matrix;

  // read a set of tiepoints
  vcl_vector<vcl_vector<double> > tiepoints(1);
  double t0, t1, t2, t3, t4, t5;
  s >> t0 >> t1 >> t2 >> t3 >> t4 >> t5;
  tiepoints[0].resize(6);
  tiepoints[0][0] = t0;
  tiepoints[0][1] = t1;
  tiepoints[0][2] = t2;
  tiepoints[0][3] = t3;
  tiepoints[0][4] = t4;
  tiepoints[0][5] = t5;

  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  s >> (*lvcs);
  p = bvxm_lidar_camera(tr_matrix, lvcs, tiepoints);
  return s ;
}