// This is gel/mrc/vpgl/file_formats/vpgl_geo_camera.cxx
#include "vpgl_geo_camera.h"
//:
// \file

#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>

#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/bgeo/bgeo_utm.h>

vpgl_geo_camera::vpgl_geo_camera()
{
  trans_matrix_.set_size(4,4);
  trans_matrix_.fill(0);
  trans_matrix_.fill_diagonal(1);

  tiepoints_.resize(1);
  tiepoints_[0].resize(6,0);

  is_utm = false;
  tx_ = ty_ = 0;
}

vpgl_geo_camera::vpgl_geo_camera(vpgl_geo_camera const& rhs)
{
  this->trans_matrix_ = rhs.trans_matrix_;
  this->lvcs_ = new bgeo_lvcs(*(rhs.lvcs_));
  this->tiepoints_ = rhs.tiepoints_;
  this->tx_ = rhs.tx_;
  this->ty_ = rhs.ty_;
}

bool vpgl_geo_camera::init_geo_camera(vil_tiff_image* const& gtif_img,
                                      vpgl_geo_camera*& camera)
{
  // check if the tiff file is geotiff
  if (!gtif_img->is_GEOTIFF()) {
    vcl_cout << "vpgl_geo_camera::init_geo_camera -- The image should be a GEOTIFF!\n";
    return false;
  }

  vil_geotiff_header* gtif = gtif_img->get_geotiff_header();
  int utm_zone;
  vil_geotiff_header::GTIF_HEMISPH h;

  // convert given tiepoint to world coordinates (lat, long)
  // based on the model defined in GEOTIFF

  // is this a PCS_WGS84_UTM?
  bool is_utm = false;
  if (gtif->PCS_WGS84_UTM_zone(utm_zone, h))
  {
    vcl_vector<vcl_vector<double> > tiepoints;
    gtif->gtif_tiepoints(tiepoints);
    bool south_flag = false;
    if (h == 1)
      south_flag = true;
    is_utm = true;
    // transform each tiepoint
    bgeo_utm utm;
    double lat, lon, elev ;
    bgeo_lvcs_sptr lvcs;
    for (unsigned i=0; i< tiepoints.size(); i++) {
      assert (tiepoints[i].size() == 6);
      double I = tiepoints[i][0]; // lat
      double J = tiepoints[i][1]; // lon
      double K = tiepoints[i][2]; // elev
      double X = tiepoints[i][3];
      double Y = tiepoints[i][4];
      double Z = tiepoints[i][5];

      utm.transform(utm_zone, X, Y, Z, I, J, K, south_flag );
      lvcs = new bgeo_lvcs(I,J,K); // FIXME - overwrites the previously created one!
      tiepoints[i][0] = I; // lat
      tiepoints[i][1] = J; // lon
      tiepoints[i][2] = K; // elev
      //scale_ = 1;
      // now, we have a mapping (I,J,K)->(X,Y,Z)
    }

    // create a transformation matrix
    // if there is a transormation matrix in GEOTIFF, use that
    vnl_matrix<double> trans_matrix;
    double* trans_matrix_values;
    double sx1, sy1, sz1;
    if (gtif->gtif_trans_matrix(trans_matrix_values)){
      vcl_cout << "Transfer matrix is given, using that...." << vcl_endl;
      trans_matrix.copy_in(trans_matrix_values);
    } else if (gtif->gtif_pixelscale(sx1, sy1, sz1)) {
      vpgl_geo_camera::comp_trans_matrix(sx1, sy1, sz1, tiepoints, trans_matrix);
    } else {
      vcl_cout << "vpgl_geo_camera::init_geo_camera comp_trans_matrix -- Transform matrix cannot be formed..\n";
      return false;
    }

    // create the camera

    camera = new vpgl_geo_camera(trans_matrix, lvcs, tiepoints);
    if (is_utm)
      camera->set_utm(utm_zone, h);
    return true;
  } else {
      vcl_cout << "bmdl_lidar_roi_process::lidar_init()-- Only ProjectedCSTypeGeoKey=PCS_WGS84_UTM_zoneXX_X is defined rigth now, please define yours!!" << vcl_endl;
      return false;
  }
}
//: transforms a given 3d world point to image plane
void vpgl_geo_camera::project(const double x, const double y, const double z,
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
  res = trans_matrix_inv*vec;
  u = res[0];
  v = res[1];
}

//: backprojects an image point into local coordinates (based on lvcs_)
void vpgl_geo_camera::backproject(const double u, const double v,
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
}

void vpgl_geo_camera::translate(double tx, double ty, double z)
{
  trans_matrix_[0][3] += tx;
  trans_matrix_[1][3] -= ty;
  double lon, lat, elev;
  img_to_wgs((unsigned int)tx, (unsigned int)ty, (unsigned int)z, lon, lat, elev);
  vcl_cout << *lvcs_ << vcl_endl
           << "setting origin to " << lon << ' ' << lat << ' ' << elev << vcl_endl;
  lvcs_->set_origin(lon, lat, elev);
  tx_ = tx;
  ty_ = ty;
}

void vpgl_geo_camera::img_to_wgs(const unsigned i, const unsigned j, const unsigned z,
                                 double& lon, double& lat, double& elev)
{
  vnl_vector<double> v(4), res(4);
  v[0] = tiepoints_[0][3] + i + tx_;
  v[1] = tiepoints_[0][4] - j - ty_;
  v[2] = z;
  v[3] = 1;
  //find the UTM values
  bgeo_utm utm;
  utm.transform(utm_zone_, v[0], v[1], v[2], lat, lon, elev);
}

bool vpgl_geo_camera::operator==(vpgl_geo_camera const& rhs) const
{
  return this->trans_matrix_ == rhs.trans_matrix_ &&
         *(this->lvcs_) == *(rhs.lvcs_);
}

//: Write vpgl_perspective_camera to stream
vcl_ostream&  operator<<(vcl_ostream& s,
                         vpgl_geo_camera const& p)
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
                         vpgl_geo_camera& p)
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
  p = vpgl_geo_camera(tr_matrix, lvcs, tiepoints);
  return s ;
}

bool vpgl_geo_camera::comp_trans_matrix(double sx1, double sy1, double sz1,//vil_geotiff_header* gtif,
                                        vcl_vector<vcl_vector<double> > tiepoints,
                                        vnl_matrix<double>& trans_matrix)
{
  double sx, sy, sz;

  // use tiepoints and scale values to create a transformation matrix
  // for now use the first tiepoint if there are more than one
  assert (tiepoints.size() > 0);
  assert (tiepoints[0].size() == 6);
  double I = tiepoints[0][0];
  double J = tiepoints[0][1];
  double K = tiepoints[0][2];
  double X = tiepoints[0][3];
  double Y = tiepoints[0][4];
  double Z = tiepoints[0][5];

  // Define a transformation matrix as follows:
  //
  //      |-                         -|
  //      |   Sx    0.0   0.0   Tx    |
  //      |                           |      Tx = X - I*Sx
  //      |   0.0  -Sy    0.0   Ty    |      Ty = Y + J*Sy
  //      |                           |      Tz = Z - K*Sz
  //      |   0.0   0.0   Sz    Tz    |
  //      |                           |
  //      |   0.0   0.0   0.0   1.0   |
  //      |-                         -|

  double scale = 1;
  sx = scale; sy=scale; sz=scale;
  double Tx = X - I*sx;
  double Ty = Y + J*sy;
  double Tz = Z - K*sz;

  vnl_matrix<double> m(4,4);
  m.fill(0.0);
  m[0][0] = sx;
  m[1][1] = -1*sy;
  m[2][2] = sz;
  m[3][3] = 1.0;
  m[0][3] = Tx;
  m[1][3] = Ty;
  m[2][3] = Tz;
  trans_matrix = m;
  vcl_cout << trans_matrix << vcl_endl;
  return true;
}

// Binary I/O

//: Binary read self from stream
void vpgl_geo_camera::b_read(vsl_b_istream &is)
{
  // read transformation matrix
  unsigned r,c;
  double v;
  vsl_b_read(is,r);
  vsl_b_read(is,c);
  trans_matrix_.set_size(r,c);
  for (unsigned i=0;r; i++) {
    for (unsigned j=0;c; i++) {
      vsl_b_read(is,v);
      trans_matrix_.put(i,j,v);
    }
  }

  // read lvcs
  lvcs_->b_read(is);
  // read tiepoints
  unsigned size_i, size_j;
  vsl_b_read(is,size_i);
  tiepoints_.resize(size_i);
  for (unsigned i=0; i<size_i; ++i) {
    vsl_b_read(is,size_j);
    tiepoints_[i].resize(size_j);
    for (unsigned j=0; j<size_j; ++j) {
      vsl_b_read(is,tiepoints_[i][j]);
    }
  }
  vsl_b_read(is,is_utm);
  vsl_b_read(is,utm_zone_);
  vsl_b_read(is,northing_);

  return;
}


//: Binary save self to stream.
void vpgl_geo_camera::b_write(vsl_b_ostream &os) const
{
  // write transformation matrix
  vsl_b_write(os,trans_matrix_.rows());
  vsl_b_write(os,trans_matrix_.cols());
  for (unsigned i=0; trans_matrix_.rows(); i++)
    for (unsigned j=0; trans_matrix_.cols(); i++)
      vsl_b_write(os,trans_matrix_(i,j));

  // write tiepoints
  lvcs_->b_write(os);
  vsl_b_write(os,tiepoints_.size());
  for (unsigned i=0; i<tiepoints_.size(); ++i) {
    vsl_b_write(os,tiepoints_[i].size());
    for (unsigned j=0; j<tiepoints_[i].size(); ++j) {
      vsl_b_write(os,tiepoints_[i][j]);
    }
  }
  vsl_b_write(os,is_utm);
  vsl_b_write(os,utm_zone_);
  vsl_b_write(os,northing_);
  return;
}

