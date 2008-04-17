#include "bvxm_lidar_camera.h"
//:
// \file

#include <vcl_vector.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>

#include <bgeo/bgeo_utm.h>

bvxm_lidar_camera::bvxm_lidar_camera()
{
  trans_matrix_.set_size(4,4);
  trans_matrix_.fill(0);
  trans_matrix_.fill_diagonal(1);

  tiepoints_.resize(1);
  tiepoints_[0].resize(6,0);

  is_utm = false;
  img_u_ = img_v_ = 0;
}

bvxm_lidar_camera::bvxm_lidar_camera(bvxm_lidar_camera const& rhs)
{
  this->trans_matrix_ = rhs.trans_matrix_;
  this->lvcs_ = new bgeo_lvcs(*(rhs.lvcs_));
  this->tiepoints_ = rhs.tiepoints_;
  this->img_u_ = new int(*rhs.img_u_);
  this->img_v_ = new int(*rhs.img_v_);
}

//: transforms a given 3d world point to image plane
void bvxm_lidar_camera::project(const double x, const double y, const double z,
                                double& u, double& v) const
{
  vnl_vector<double> vec(4), res(4);
  double lat, lon, gz;

  if (lvcs_)
    lvcs_->local_to_global(x, y, z, bgeo_lvcs::wgs84, lon, lat, gz);
  else {
    lat = y;
    lon = x;
  }

  double x1=lon, y1=lat;
  if (is_utm) {
    bgeo_utm utm;
    int utm_zone;
    utm.transform(lat, lon, x1, y1, utm_zone);
  }
  vec[0] = x1;
  vec[1] = y1;
  vec[2] = 0;
  vec[3] = 1;

  // do we really need this, const does not allow this
  vnl_matrix<double> tm(trans_matrix_);
  tm[2][2] = 1;

  vnl_matrix<double> trans_matrix_inv = vnl_inverse(tm);
  vcl_cout << trans_matrix_inv << vcl_endl;
  vcl_cout << vec << vcl_endl;
  res = trans_matrix_inv*vec;
  vcl_cout << res[0] << ' ' << res[1] << vcl_endl;
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
  double lat, lon, elev;
  if (is_utm) {
    //find the UTM values
    bgeo_utm utm;
    utm.transform(utm_zone_, vec[0], vec[1], vec[2], lat, lon, elev);
  } else {
    lat = vec[0];
    lon = vec[1];
    elev = vec[2];
  }

  if (lvcs_)
    lvcs_->global_to_local(lon, lat, elev, bgeo_lvcs::wgs84, x, y, z);
  //z = img_view_(u, v);
}

void bvxm_lidar_camera::translate(double tx, double ty)
{
  trans_matrix_[0][3] += tx;
  trans_matrix_[1][3] -= ty;
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
  utm.transform(utm_zone_, v[0], v[1], v[2], lat, lon, elev);
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
  s << p.trans_matrix_ << '\n'
    << p.tiepoints_[0][0] << '\n'
    << p.tiepoints_[0][1] << '\n'
    << p.tiepoints_[0][2] << '\n'
    << p.tiepoints_[0][3] << '\n'
    << p.tiepoints_[0][4] << '\n'
    << p.tiepoints_[0][5] << '\n'
    << *(p.lvcs_) << '\n';

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
