// This is gel/mrc/vpgl/vpgl_local_rational_camera.txx
#ifndef vpgl_local_rational_camera_txx_
#define vpgl_local_rational_camera_txx_
//:
// \file
#include "vpgl_local_rational_camera.h"
#include <vcl_vector.txx>
#include <vcl_fstream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
//--------------------------------------
// Constructors
//

// Create an identity projection, i.e. (x,y,z) identically maps to (x,y)
template <class T>
vpgl_local_rational_camera<T>::vpgl_local_rational_camera():
  vpgl_rational_camera<T>()
{
}

//: Constructor from a rational camera and an affine matrix
template <class T>
vpgl_local_rational_camera<T>::
vpgl_local_rational_camera(bgeo_lvcs const& lvcs,
                           vpgl_rational_camera<T> const& rcam):

  vpgl_rational_camera<T>(rcam), lvcs_(lvcs)
{
}

//: Constructor from a rational camera and a geographic origin
template <class T>
vpgl_local_rational_camera<T>::
vpgl_local_rational_camera(T longitude, T latitude, T elevation,
                           vpgl_rational_camera<T> const& rcam) :
  vpgl_rational_camera<T>(rcam),
  lvcs_(bgeo_lvcs(latitude, longitude, elevation,
                  bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS))
  {
  }


template <class T>
vpgl_local_rational_camera<T>* vpgl_local_rational_camera<T>::clone(void) const
{
  return new vpgl_local_rational_camera<T>(*this);
}

// Base projection method, x, y, z are in local Cartesian coordinates
template <class T>
void vpgl_local_rational_camera<T>::project(const T x, const T y, const T z,
                                           T& u, T& v) const
{
  //first convert to global geographic  coordinates
  double lon, lat, gz;
  bgeo_lvcs& non_const_lvcs = const_cast<bgeo_lvcs&>(lvcs_);
  non_const_lvcs.local_to_global(x, y, z, bgeo_lvcs::wgs84, lon, lat, gz);
  vpgl_rational_camera<T>::project(lon, lat, gz, u, v);
}

//vnl interface methods
template <class T>
vnl_vector_fixed<T, 2>
vpgl_local_rational_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point)const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2],
                image_point[0], image_point[1]);
  return image_point;
}

//vgl interface methods
template <class T>
vgl_point_2d<T> vpgl_local_rational_camera<T>::project(vgl_point_3d<T> world_point)const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}

//: print the camera parameters
template <class T>
void vpgl_local_rational_camera<T>::print(vcl_ostream& s) const
{
  vpgl_rational_camera<T>::print(s);
  s << lvcs_ <<'\n'
    <<"------------------------------------------------\n\n";
}

template <class T>
bool vpgl_local_rational_camera<T>::save(vcl_string cam_path)
{
  vcl_ofstream file_out;
  file_out.open(cam_path.c_str());
  if (!file_out.good()) {
    vcl_cerr << "error: bad filename: " << cam_path << vcl_endl;
    return false;
  }
  file_out.precision(12);

  int map[20];
  map[0]=19;
  map[1]=9;
  map[2]=15;
  map[3]=18;
  map[4]=6;
  map[5]=8;
  map[6]=14;
  map[7]=3;
  map[8]=12;
  map[9]=17;
  map[10]=5;
  map[11]=0;
  map[12]=4;
  map[13]=7;
  map[14]=1;
  map[15]=10;
  map[16]=13;
  map[17]=2;
  map[18]=11;
  map[19]=16;

  file_out << "satId = \"????\";\n"
           << "bandId = \"RGB\";\n"
           << "SpecId = \"RPC00B\";\n"
           << "BEGIN_GROUP = IMAGE\n"
           << "\n\n"  // skip errBias and errRand fields
           << "  lineOffset = " << offset(vpgl_rational_camera<T>::V_INDX) << '\n'
           << "  sampOffset = " << offset(vpgl_rational_camera<T>::U_INDX) << '\n'
           << "  latOffset = " << offset(vpgl_rational_camera<T>::Y_INDX) << '\n'
           << "  longOffset = " << offset(vpgl_rational_camera<T>::X_INDX) << '\n'
           << "  heightOffset = " << offset(vpgl_rational_camera<T>::Z_INDX) << '\n'
           << "  lineScale = " << scale(vpgl_rational_camera<T>::V_INDX) << '\n'
           << "  sampScale = " << scale(vpgl_rational_camera<T>::U_INDX) << '\n'
           << "  latScale = " << scale(vpgl_rational_camera<T>::Y_INDX) << '\n'
           << "  longScale = " << scale(vpgl_rational_camera<T>::X_INDX) << '\n'
           << "  heightScale = " << scale(vpgl_rational_camera<T>::Z_INDX) << '\n';
  vnl_matrix_fixed<double,4,20> coeffs = this->coefficient_matrix();
  file_out << "  lineNumCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[NEU_V][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  lineDenCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[DEN_V][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  sampNumCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[NEU_U][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  sampDenCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[DEN_U][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n"
           << "END_GROUP = IMAGE\n"
           << "END;\n"

           << "lvcs\n";
  double longitude, latitude, elevation;
  lvcs_.get_origin(latitude, longitude, elevation);
  file_out << longitude << '\n'
           << latitude << '\n'
           << elevation << '\n';
  return true;
}
// Binary I/O

//: Binary read self from stream
template <class T> void vpgl_local_rational_camera<T>::
b_read(vsl_b_istream &is)
{
  vpgl_rational_camera<T>::b_read(is);
  lvcs_.b_read(is);
  return;
}


//: Binary save self to stream.
template <class T> void vpgl_local_rational_camera<T>::
b_write(vsl_b_ostream &os) const
{
  vpgl_rational_camera<T>::b_write(os);
  lvcs_.b_write(os);
  return;
}

// read from a file
template <class T>
vpgl_local_rational_camera<T>* read_local_rational_camera(vcl_string cam_path)
{
  vcl_ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    vcl_cout << "error: bad filename: " << cam_path << vcl_endl;
    return 0;
  }
  return read_local_rational_camera<T>(file_inp);
}
//: read from an open istream
template <class T>
vpgl_local_rational_camera<T>* read_local_rational_camera(vcl_istream& istr)
{
  vpgl_rational_camera<T>* rcam = read_rational_camera<T>(istr);
  if (!rcam)
    return 0;
  vcl_string input;
  bool good = false;
  bgeo_lvcs lvcs;
  while (!istr.eof()&&!good) {
    istr >> input;
    if (input=="lvcs")
    {
      double longitude, latitude, elevation;
      istr >> longitude >> latitude >> elevation;
      lvcs = bgeo_lvcs(latitude, longitude, elevation,
                     bgeo_lvcs::wgs84, bgeo_lvcs::DEG, bgeo_lvcs::METERS);
      good = true;
    }
  }
  if  (!good)
  {
    vcl_cout << "error: not a composite rational camera file\n";
    return 0;
  }
  return new vpgl_local_rational_camera<T>(lvcs, *rcam);
}


//: Write to stream
template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, const vpgl_local_rational_camera<T>& c )
{
  c.print(s);
  return s;
}


// Code for easy instantiation.
#undef vpgl_LOCAL_RATIONAL_CAMERA_INSTANTIATE
#define vpgl_LOCAL_RATIONAL_CAMERA_INSTANTIATE(T) \
template class vpgl_local_rational_camera<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vpgl_local_rational_camera<T >&); \
template vpgl_local_rational_camera<T >* read_local_rational_camera(vcl_string); \
template vpgl_local_rational_camera<T >* read_local_rational_camera(vcl_istream&)

#endif // vpgl_local_rational_camera_txx_
