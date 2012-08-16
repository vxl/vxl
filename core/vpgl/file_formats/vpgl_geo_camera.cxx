// This is core/vpgl/file_formats/vpgl_geo_camera.cxx
#include "vpgl_geo_camera.h"
//:
// \file

#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_inverse.h>

#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_utm.h>

#include <vil/file_formats/vil_geotiff_header.h>
#include <vil/file_formats/vil_tiff.h>
#include <vil/file_formats/vil_nitf2_image.h>

vpgl_geo_camera::vpgl_geo_camera()
{
  trans_matrix_.set_size(4,4);
  trans_matrix_.fill(0);
  trans_matrix_.fill_diagonal(1);
  is_utm = false;
  scale_tag_ = false;
}

vpgl_geo_camera::vpgl_geo_camera(vpgl_geo_camera const& rhs)
{
  this->trans_matrix_ = rhs.trans_matrix_;
  this->lvcs_ = new vpgl_lvcs(*(rhs.lvcs_));
  this->is_utm = rhs.is_utm;
  this->utm_zone_ = rhs.utm_zone_;
  this->scale_tag_ = rhs.scale_tag_;
}

bool vpgl_geo_camera::init_geo_camera(vil_image_resource_sptr const geotiff_img,
                                      vpgl_lvcs_sptr lvcs,
                                      vpgl_geo_camera*& camera)
{
  // check if the image is tiff
  vil_tiff_image* geotiff_tiff = dynamic_cast<vil_tiff_image*> (geotiff_img.ptr());
  if (!geotiff_tiff) {
      vcl_cerr << "vpgl_geo_camera::init_geo_camera : Error casting vil_image_resource to a tiff image.\n";
      return false;
  }
  if (!geotiff_tiff->get_geotiff_header()) {
    vcl_cerr << "no geotiff header!\n";
    return false;
  }

  // check if the tiff file is geotiff
  if (!geotiff_tiff->is_GEOTIFF()) {
    vcl_cerr << "vpgl_geo_camera::init_geo_camera -- The image should be a GEOTIFF!\n";
    return false;
  }

  vil_geotiff_header* gtif = geotiff_tiff->get_geotiff_header();
  int utm_zone;
  vil_geotiff_header::GTIF_HEMISPH h;

  vcl_vector<vcl_vector<double> > tiepoints;
  gtif->gtif_tiepoints(tiepoints);

  // create a transformation matrix
  // if there is a transformation matrix in GEOTIFF, use that
  vnl_matrix<double> trans_matrix;
  double* trans_matrix_values;
  double sx1, sy1, sz1;
  bool scale_tag=false;
  if (gtif->gtif_trans_matrix(trans_matrix_values)) {
    vcl_cout << "Transfer matrix is given, using that...." << vcl_endl;
    trans_matrix.copy_in(trans_matrix_values);
    vcl_cout << "Warning LIDAR sample spacing different than 1 meter will not be handled correctly!\n";
  }
  else if (gtif->gtif_pixelscale(sx1, sy1, sz1)) {
    scale_tag = true;
    vpgl_geo_camera::comp_trans_matrix(sx1, sy1, sz1, tiepoints,
                                       trans_matrix, scale_tag);
  }
  else {
    vcl_cout << "vpgl_geo_camera::init_geo_camera comp_trans_matrix -- Transform matrix cannot be formed..\n";
    return false;
  }

  // create the camera
  camera = new vpgl_geo_camera(trans_matrix, lvcs);
  camera->set_scale_format(scale_tag);

  // check if the model type is geographic and also the units
  if (gtif->GCS_WGS84_MET_DEG())
    return true;

  // otherwise check if it is projected to UTM and figure out the zone
  if (gtif->PCS_WGS84_UTM_zone(utm_zone, h) || gtif->PCS_NAD83_UTM_zone(utm_zone, h))
  {
    camera->set_utm(utm_zone, h);
    return true;
  }
  else {
    vcl_cout << "vpgl_geo_camera::init_geo_camera()-- if UTM only PCS_WGS84_UTM and PCS_NAD83_UTM, if geographic (GCS_WGS_84) only linear units in meters, angular units in degrees are supported, please define otherwise!" << vcl_endl;
    return false;
  }
}

//: transforms a given local 3d world point to image plane
void vpgl_geo_camera::project(const double x, const double y, const double z,
                              double& u, double& v) const
{
  vnl_vector<double> vec(4), res(4);
  double lat, lon, gz;

  if (lvcs_) {
    if (lvcs_->get_cs_name() == vpgl_lvcs::utm) {
      if (is_utm) {  // geo cam is also utm so keep using utm
        double gx, gy;
        lvcs_->local_to_global(x, y, z, vpgl_lvcs::utm, gx, gy, gz);
        this->global_utm_to_img(gx, gy, utm_zone_, gz, u, v);
      }
      else {  // geo cam is not utm, convert to wgs84 as global
        lvcs_->local_to_global(x, y, z, vpgl_lvcs::wgs84, lon, lat, gz);
        this->global_to_img(lon, lat, gz, u, v);
      }
    }
    else {
      lvcs_->local_to_global(x, y, z, vpgl_lvcs::wgs84, lon, lat, gz);
      this->global_to_img(lon, lat, gz, u, v);
    }
  }
  else // if there is no lvcs, then we assume global coords are given in wgs84, i.e. x is lon and y is lat
    this->global_to_img(x, y, z, u, v);
}

//: backprojects an image point into local coordinates (based on lvcs_)
void vpgl_geo_camera::backproject(const double u, const double v,
                                  double& x, double& y, double& z)
{
  vnl_vector<double> vec(4), res(4);
  if (scale_tag_) {
    vec[0] = trans_matrix_[0][3] + trans_matrix_[0][0]*u;
    vec[1] = trans_matrix_[1][3] + trans_matrix_[1][1]*v;
  }
  else { // assumes scale is 1
    vec[0] = trans_matrix_[0][3] + u;
    vec[1] = trans_matrix_[1][3] - v;
  }
  vec[2] = 0;
  vec[3] = 1;
  //vcl_cout << '\n' << vec << vcl_endl;

  double lat, lon, elev;
  if (is_utm) {
    if (lvcs_) {
      if (lvcs_->get_cs_name() == vpgl_lvcs::utm) { // the local cs of lvcs is also utm, so use it directly
        lvcs_->global_to_local(vec[0], vec[1], vec[2], vpgl_lvcs::utm, x, y, z);
        return;
      }
    }
    //find the UTM values
    vpgl_utm utm;
    utm.transform(utm_zone_, vec[0], vec[1], vec[2], lat, lon, elev);
  }
  else {
    lat = vec[0];
    lon = vec[1];
    elev = vec[2];
  }

  if (lvcs_)
    lvcs_->global_to_local(lon, lat, elev, vpgl_lvcs::wgs84, x, y, z);
}

void vpgl_geo_camera::translate(double tx, double ty, double z)
{
  // use the scale values
  if (scale_tag_) {
    trans_matrix_[0][3] += tx*trans_matrix_[0][0];
    trans_matrix_[1][3] += ty*trans_matrix_[1][1]; // multiplying by -1.0 to get sy
  }
  else {
    vcl_cout << "Warning! Translation offset will only be computed correctly for lidar pixel spacing = 1 meter\n";
    trans_matrix_[0][3] += tx;
    trans_matrix_[1][3] -= ty;
  }
}

//: returns the corresponding geographical coordinates for a given pixel position (i,j)
//  The output global coord is wgs84
void vpgl_geo_camera::img_to_global(const double i, const double j,
                                    double& lon, double& lat) const
{
  vnl_vector<double> v(4), res(4);
  if (scale_tag_) {
    v[0] = trans_matrix_[0][3] + i*trans_matrix_[0][0];
    v[1] = trans_matrix_[1][3] + j*trans_matrix_[1][1];
  }
  else {
    v[0] = trans_matrix_[0][3] + i;
    v[1] = trans_matrix_[1][3] - j;
  }
  v[2] = 0;
  v[3] = 1;
  if (is_utm) {
    vpgl_utm utm; double dummy;
    utm.transform(utm_zone_, v[0], v[1], v[2], lat, lon, dummy);
  }
  else {
    //lon = v[0]; lat = v[1]; elev = v[2];
    lon = v[0]; lat = v[1];
  }
}

//: returns the corresponding pixel position for given geographical coordinates
//  The input global coord is wgs84
void vpgl_geo_camera::global_to_img(const double lon, const double lat, const double gz,
                                    double& u, double& v) const
{
  vnl_vector<double> vec(4), res(4);
  double x1=lon, y1=lat, z1=gz;
  if (is_utm) {
    vpgl_utm utm;
    int utm_zone;
    utm.transform(lat, lon, x1, y1, utm_zone);
    //vcl_cout << "utm returned x1: " << x1 << " y1: " << y1 << vcl_endl;
    //z1 = 0;
  }
  vec[0] = x1;
  vec[1] = y1;
  vec[2] = z1;
  vec[3] = 1;

  // do we really need this, const does not allow this
  vnl_matrix<double> tm(trans_matrix_);
  tm[2][2] = 1;

  if (scale_tag_) {
    u = (vec[0] - trans_matrix_[0][3])/trans_matrix_[0][0];
    v = (vec[1] - trans_matrix_[1][3])/trans_matrix_[1][1];
  }
  else {
    vnl_matrix<double> trans_matrix_inv = vnl_inverse(tm);
    res = trans_matrix_inv*vec;
    u = res[0];
    v = res[1];
  }
}

//: returns the corresponding geographical coordinates for a given pixel position (i,j)
//  The output global coord is UTM: x east, y north
void vpgl_geo_camera::img_to_global_utm(const double i, const double j, double& x, double& y) const
{
  vnl_vector<double> v(4), res(4);
  if (scale_tag_) {
    v[0] = trans_matrix_[0][3] + i*trans_matrix_[0][0];
    v[1] = trans_matrix_[1][3] + j*trans_matrix_[1][1];
  }
  else {
    v[0] = trans_matrix_[0][3] + i;
    v[1] = trans_matrix_[1][3] - j;
  }
  v[2] = 0;
  v[3] = 1;
  if (is_utm) {
    x = v[0];
    y = v[1];
  }
  else {  // the trans matrix was using lat,lon coord, transform output to utm
    vpgl_utm utm; int dummy_zone;
    utm.transform(v[0], v[1], x, y, dummy_zone);
  }
}

//: returns the corresponding pixel position for given geographical coordinates
//  The input global coord is UTM: x east, for y north
void vpgl_geo_camera::global_utm_to_img(const double x, const double y, int zone, double elev, double& u, double& v) const
{
  vnl_vector<double> vec(4), res(4);
  if (is_utm) {
    vec[0] = x;
    vec[1] = y;
    vec[2] = elev;
  }
  else {
    vpgl_utm utm;
    double lat, lon, z;
    utm.transform(zone, x, y, elev, lat, lon, z);
    vec[0] = lat;
    vec[1] = lon;
    vec[2] = z;
  }
  vec[3] = 1;

  // do we really need this, const does not allow this
  vnl_matrix<double> tm(trans_matrix_);
  tm[2][2] = 1;

  if (scale_tag_) {
    u = (vec[0] - trans_matrix_[0][3])/trans_matrix_[0][0];
    v = (vec[1] - trans_matrix_[1][3])/trans_matrix_[1][1];
  }
  else {
    vnl_matrix<double> trans_matrix_inv = vnl_inverse(tm);
    res = trans_matrix_inv*vec;
    u = res[0];
    v = res[1];
  }
}

//: returns the corresponding utm location for the given local position
void vpgl_geo_camera::local_to_utm(const double x, const double y, const double z, double& e, double& n, int& utm_zone)
{
  double lat, lon, gz;
  lvcs_->local_to_global(x,y,z,vpgl_lvcs::wgs84,lon, lat, gz);

  vpgl_utm utm;
  utm.transform(lat, lon, e, n, utm_zone);
}

bool vpgl_geo_camera::img_four_corners_in_utm(const unsigned ni, const unsigned nj, double elev, double& e1, double& n1, double& e2, double& n2)
{
  if (!is_utm) {
    vcl_cerr << "In vpgl_geo_camera::img_four_corners_in_utm() -- UTM hasn't been set!\n";
    return false;
  }
  double lon,lat;
  this->img_to_global(0, 0, lon, lat);
  vpgl_utm utm;int utm_zone;
  utm.transform(lat, lon, e1, n1, utm_zone);
  this->img_to_global(ni, nj, lon, lat);
  utm.transform(lat, lon, e2, n2, utm_zone);
  return true;
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
  s << p.trans_matrix_ << '\n'<< *(p.lvcs_) << '\n';
  if (p.is_utm) {
    s << "geocam is using UTM with zone: " << p.utm_zone_ << '\n';
  }

  return s ;
}

//: Read vpgl_perspective_camera from stream
vcl_istream&  operator>>(vcl_istream& s,
                         vpgl_geo_camera& p)
{
  vnl_matrix_fixed<double,4,4> tr_matrix;
  s >> tr_matrix;
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  s >> (*lvcs);
  p = vpgl_geo_camera(tr_matrix.as_ref(), lvcs);
  return s ;
}

bool vpgl_geo_camera::comp_trans_matrix(double sx1, double sy1, double sz1,
                                        vcl_vector<vcl_vector<double> > tiepoints,
                                        vnl_matrix<double>& trans_matrix,
                                        bool scale_tag)
{
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
  double sx = 1.0, sy = 1.0, sz = 1.0;
  if (scale_tag) {
    sx = sx1; sy = sy1; sz = sz1;
  }
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

void vpgl_geo_camera::img_to_wgs(unsigned i, unsigned j, unsigned k, double& lon, double& lat, double& elev)
{
  assert(!"Not yet implemented");
}
