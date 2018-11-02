// This is core/vpgl/vpgl_rational_camera.hxx
#ifndef vpgl_rational_camera_hxx_
#define vpgl_rational_camera_hxx_
//:
// \file

#include <vector>
#include <fstream>
#include "vpgl_rational_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>
//#include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
//--------------------------------------
// Constructors
//

// Create an identity projection, i.e. (x,y) identically maps to (u,v)
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera()
{
  rational_coeffs_.fill(0);
  rational_coeffs_[DEN_U][19]=1;
  rational_coeffs_[DEN_V][19]=1;
  rational_coeffs_[NEU_U][9]=1; // x coefficient
  rational_coeffs_[NEU_V][15]=1;// y coefficient
  vpgl_scale_offset<T> soff;
  scale_offsets_.resize(5, soff);
}

//: Constructor with an array encoding of the coefficients
template <class T>
vpgl_rational_camera<T>::
vpgl_rational_camera(std::vector<std::vector<T> > const& rational_coeffs,
                     std::vector<vpgl_scale_offset<T> > const& scale_offsets)
{
  this->set_coefficients(rational_coeffs);
  this->set_scale_offsets(scale_offsets);
}

template <class T>
vpgl_rational_camera<T>::
vpgl_rational_camera(std::vector<T> const& neu_u,
                     std::vector<T> const& den_u,
                     std::vector<T> const& neu_v,
                     std::vector<T> const& den_v,
                     const T x_scale, const T x_off,
                     const T y_scale, const T y_off,
                     const T z_scale, const T z_off,
                     const T u_scale, const T u_off,
                     const T v_scale, const T v_off
                    )
{
  for (unsigned i = 0; i<20; ++i)
  {
    rational_coeffs_[NEU_U][i] = neu_u[i];
    rational_coeffs_[DEN_U][i] = den_u[i];
    rational_coeffs_[NEU_V][i] = neu_v[i];
    rational_coeffs_[DEN_V][i] = den_v[i];
  }
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

//: Constructor from 4 coefficient arrays and 5 scale, offset pairs.
template <class T>
vpgl_rational_camera<T>::
vpgl_rational_camera(const double*  neu_u,
                     const double* den_u,
                     const double* neu_v,
                     const double* den_v,
                     const T x_scale, const T x_off,
                     const T y_scale, const T y_off,
                     const T z_scale, const T z_off,
                     const T u_scale, const T u_off,
                     const T v_scale, const T v_off
                    )
{
  for (unsigned i = 0; i<20; ++i)
  {
    rational_coeffs_[NEU_U][i] = T(neu_u[i]);
    rational_coeffs_[DEN_U][i] = T(den_u[i]);
    rational_coeffs_[NEU_V][i] = T(neu_v[i]);
    rational_coeffs_[DEN_V][i] = T(den_v[i]);
  }
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

template <class T>
vpgl_rational_camera<T>* vpgl_rational_camera<T>::clone(void) const
{
  return new vpgl_rational_camera<T>(*this);
}

template <class T>
void vpgl_rational_camera<T>::
set_coefficients(std::vector<std::vector<T> > const& rational_coeffs)
{
  for (unsigned j = 0; j<4; ++j)
    for (unsigned i = 0; i<20; ++i)
      rational_coeffs_[j][i] = rational_coeffs[j][i];
}

template <class T>
void vpgl_rational_camera<T>::
set_scale_offsets(std::vector<vpgl_scale_offset<T> > const& scale_offsets)
{
  scale_offsets_=scale_offsets;
}

template <class T>
std::vector<std::vector<T> > vpgl_rational_camera<T>::coefficients() const
{
  std::vector<std::vector<T> > result(4);
  for (unsigned j = 0; j<4; ++j)
  {
    result[j].resize(20);
    for (unsigned i = 0; i<20; ++i)
      result[j][i]=rational_coeffs_[j][i];
  }
  return result;
}

//: Create a vector with the standard order of monomial terms
template <class T>
vnl_vector_fixed<T, 20>
vpgl_rational_camera<T>::power_vector(const T x, const T y, const T z) const
{
  // Form the monomials in homogeneous form
  double w  = 1;
  double xx = x*x;
  double xy = x*y;
  double xz = x*z;
  double yy = y*y;
  double yz = y*z;
  double zz = z*z;
  double xxx = x*xx;
  double xxy = x*xy;
  double xxz = x*xz;
  double xyy = x*yy;
  double xyz = x*yz;
  double xzz = x*zz;
  double yyy = y*yy;
  double yyz = y*yz;
  double yzz = y*zz;
  double zzz = z*zz;
  double xww = x*w*w;
  double yww = y*w*w;
  double zww = z*w*w;
  double www = w*w*w;
  double xxw = xx*w;
  double xyw = xy*w;
  double xzw = xz*w;
  double yyw = yy*w;
  double yzw = yz*w;
  double zzw = zz*w;

  //fill the vector
  vnl_vector_fixed<T, 20> pv;
  pv.put( 0, T(xxx));
  pv.put( 1, T(xxy));
  pv.put( 2, T(xxz));
  pv.put( 3, T(xxw));
  pv.put( 4, T(xyy));
  pv.put( 5, T(xyz));
  pv.put( 6, T(xyw));
  pv.put( 7, T(xzz));
  pv.put( 8, T(xzw));
  pv.put( 9, T(xww));
  pv.put(10, T(yyy));
  pv.put(11, T(yyz));
  pv.put(12, T(yyw));
  pv.put(13, T(yzz));
  pv.put(14, T(yzw));
  pv.put(15, T(yww));
  pv.put(16, T(zzz));
  pv.put(17, T(zzw));
  pv.put(18, T(zww));
  pv.put(19, T(www));
  return pv;
}

// Base projection method
template <class T>
void vpgl_rational_camera<T>::project(const T x, const T y, const T z,
                                      T& u, T& v) const
{
  // scale, offset the world point before projection
  T sx = scale_offsets_[X_INDX].normalize(x);
  T sy = scale_offsets_[Y_INDX].normalize(y);
  T sz = scale_offsets_[Z_INDX].normalize(z);
  vnl_vector_fixed<T, 4> polys = rational_coeffs_*power_vector(sx, sy, sz);
  T su = polys[NEU_U]/polys[DEN_U];
  T sv = polys[NEU_V]/polys[DEN_V];
  // unscale the resulting image coordinates
  u = scale_offsets_[U_INDX].un_normalize(su);
  v = scale_offsets_[V_INDX].un_normalize(sv);
}

//vnl interface methods
template <class T>
vnl_vector_fixed<T, 2>
vpgl_rational_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point)const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2],
                image_point[0], image_point[1]);
  return image_point;
}

//vgl interface methods
template <class T>
vgl_point_2d<T> vpgl_rational_camera<T>::project(vgl_point_3d<T> world_point)const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}

//: print the camera parameters
template <class T>
void vpgl_rational_camera<T>::print(std::ostream& s) const
{
  vpgl_scale_offset<T> sox = scale_offsets_[X_INDX];
  vpgl_scale_offset<T> soy = scale_offsets_[Y_INDX];
  vpgl_scale_offset<T> soz = scale_offsets_[Z_INDX];
  vpgl_scale_offset<T> sou = scale_offsets_[U_INDX];
  vpgl_scale_offset<T> sov = scale_offsets_[V_INDX];

  s << "vpgl_rational_camera:\n"
    << "------------------------\n"
    << "xoff = " << sox.offset()
    << "  yoff = " << soy.offset()
    << "  zoff = " << soz.offset() << '\n'
    << "xscale = " << sox.scale()
    << "  yscale = " << soy.scale()
    << "  zscale = " << soz.scale() << '\n'

    << "uoff = " << sou.offset()
    << "  voff = " << sov.offset() << '\n'
    << "uscale = " << sou.scale()
    << "  vscale = " << sov.scale() << "\n\n"

    << "U Numerator\n"
    << "[0] " << rational_coeffs_[0][0]
    << " [1] " << rational_coeffs_[0][1]
    << " [2] " << rational_coeffs_[0][2]
    << " [3] " << rational_coeffs_[0][3] <<'\n'
    << "[4] " << rational_coeffs_[0][4]
    << " [5] " << rational_coeffs_[0][5]
    << " [6] " << rational_coeffs_[0][6]
    << " [7] " << rational_coeffs_[0][7] <<'\n'
    << "[8] "  << rational_coeffs_[0][8]
    << " [9] " << rational_coeffs_[0][9]
    << " [10] " << rational_coeffs_[0][10]
    << " [11] " << rational_coeffs_[0][11] <<'\n'
    << "[12] " << rational_coeffs_[0][12]
    << " [13] " << rational_coeffs_[0][13]
    << " [14] " << rational_coeffs_[0][14]
    << " [15] "  << rational_coeffs_[0][15] <<'\n'
    << "[16] " << rational_coeffs_[0][16]
    << " [17] " << rational_coeffs_[0][17]
    << " [18] " << rational_coeffs_[0][18]
    << " [19] " << rational_coeffs_[0][19] <<"\n\n"

    << "U Denominator\n"
    << "[0] " << rational_coeffs_[1][0]
    << " [1] " << rational_coeffs_[1][1]
    << " [2] " << rational_coeffs_[1][2]
    << " [3] " << rational_coeffs_[1][3] <<'\n'
    << "[4] " << rational_coeffs_[1][4]
    << " [5] " << rational_coeffs_[1][5]
    << " [6] " << rational_coeffs_[1][6]
    << " [7] " << rational_coeffs_[1][7]  <<'\n'
    << "[8] " << rational_coeffs_[1][8]
    << " [9] " << rational_coeffs_[1][9]
    << " [10] " << rational_coeffs_[1][10]
    << " [11] " << rational_coeffs_[1][11] <<'\n'
    << "[12] " << rational_coeffs_[1][12]
    << " [13] " << rational_coeffs_[1][13]
    << " [14] " << rational_coeffs_[1][14]
    << " [15] " << rational_coeffs_[1][15] <<'\n'
    << "[16] " << rational_coeffs_[1][16]
    << " [17] " << rational_coeffs_[1][17]
    << " [18] " << rational_coeffs_[1][18]
    << " [19] " << rational_coeffs_[1][19] <<"\n\n"

    << "V Numerator\n"
    << "[0] " << rational_coeffs_[2][0]
    << " [1] " << rational_coeffs_[2][1]
    << " [2] " << rational_coeffs_[2][2]
    << " [3] " << rational_coeffs_[2][3]<<'\n'
    << "[4] " << rational_coeffs_[2][4]
    << " [5] " << rational_coeffs_[2][5]
    << " [6] " << rational_coeffs_[2][6]
    << " [7] " << rational_coeffs_[2][7] <<'\n'
    << "[8] " << rational_coeffs_[2][8]
    << " [9] " << rational_coeffs_[2][9]
    << " [10] " << rational_coeffs_[2][10]
    << " [11] " << rational_coeffs_[2][11] <<'\n'
    << "[12] " << rational_coeffs_[2][12]
    << " [13] " << rational_coeffs_[2][13]
    << " [14] " << rational_coeffs_[2][14]
    << " [15] " << rational_coeffs_[2][15]<<'\n'
    << "[16] " << rational_coeffs_[2][16]
    << " [17] " << rational_coeffs_[2][17]
    << " [18] " << rational_coeffs_[2][18]
    << " [19] " << rational_coeffs_[2][19] <<"\n\n"

    << "V Denominator\n"
    << "[0] " << rational_coeffs_[3][0]
    << " [1] " << rational_coeffs_[3][1]
    << " [2] " << rational_coeffs_[3][2]
    << " [3] " << rational_coeffs_[3][3]<<'\n'
    << "[4] " << rational_coeffs_[3][4]
    << " [5] " << rational_coeffs_[3][5]
    << " [6] " << rational_coeffs_[3][6]
    << " [7] " << rational_coeffs_[3][7] <<'\n'
    << "[8] " << rational_coeffs_[3][8]
    << " [9] " << rational_coeffs_[3][9]
    << " [10] " << rational_coeffs_[3][10]
    << " [11] " << rational_coeffs_[3][11] <<'\n'
    << "[12] " << rational_coeffs_[3][12]
    << " [13] " << rational_coeffs_[3][13]
    << " [14] " << rational_coeffs_[3][14]
    << " [15] " << rational_coeffs_[3][15]<<'\n'
    << "[16] " << rational_coeffs_[3][16]
    << " [17] " << rational_coeffs_[3][17]
    << " [18] " << rational_coeffs_[3][18]
    << " [19] " << rational_coeffs_[3][19] <<'\n'
    <<"------------------------------------------------\n\n";
}

template <class T>
bool vpgl_rational_camera<T>::save(std::string cam_path)
{
  std::ofstream file_out;
  file_out.open(cam_path.c_str());
  if (!file_out.good()) {
    std::cerr << "error: bad filename: " << cam_path << std::endl;
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
           << "  lineOffset = " << offset(V_INDX) << '\n'
           << "  sampOffset = " << offset(U_INDX) << '\n'
           << "  latOffset = " << offset(Y_INDX) << '\n'
           << "  longOffset = " << offset(X_INDX) << '\n'
           << "  heightOffset = " << offset(Z_INDX) << '\n'
           << "  lineScale = " << scale(V_INDX) << '\n'
           << "  sampScale = " << scale(U_INDX) << '\n'
           << "  latScale = " << scale(Y_INDX) << '\n'
           << "  longScale = " << scale(X_INDX) << '\n'
           << "  heightScale = " << scale(Z_INDX) << '\n';
  vnl_matrix_fixed<T,4,20> coeffs = this->coefficient_matrix();
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
           << "END;\n";

  return true;
}

//: Write to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, const vpgl_rational_camera<T >& c )
{
  c.print(s);
  return s;
}

//: read from a file
template <class T>
vpgl_rational_camera<T>* read_rational_camera(std::string cam_path)
{
  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return nullptr;
  }
  vpgl_rational_camera<T>* rcam = read_rational_camera<T>(file_inp);
  file_inp.close();
  return rcam;
}
//: read from an open istream
template <class T>
vpgl_rational_camera<T>* read_rational_camera(std::istream& istr)
{
  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off;

  std::string input;
  char bulk[100];

  while (!istr.eof()) {
    istr >> input;

    if (input=="sampScale") {
      istr >> input;
      istr >> u_scale;
    }
    if (input=="sampOffset") {
      istr >> input;
      istr >> u_off;
    }

    if (input=="lineScale") {
      istr >> input;
      istr >> v_scale;
    }
    if (input=="lineOffset") {
      istr >> input;
      istr >> v_off;
    }

    if (input=="longScale") {
      istr >> input;
      istr >> x_scale;
    }
    if (input=="longOffset") {
      istr >> input;
      istr >> x_off;
    }

    if (input=="latScale") {
      istr >> input;
      istr >> y_scale;
    }
    if (input=="latOffset") {
      istr >> input;
      istr >> y_off;
    }

    if (input=="heightScale") {
      istr >> input;
      istr >> z_scale;
    }
    if (input=="heightOffset") {
      istr >> input;
      istr >> z_off;
    }

    T temp_dbl;
    if (input=="lineNumCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="lineDenCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="sampNumCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="sampDenCoef") {
      istr >> input;
      istr >> input;
      for (int i=0; i<20; i++) {
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
      break;
    }
  }
  istr >> input;
  if (input!="END_GROUP")
    return nullptr;
  istr >> input;
  if (input!="=")
    return nullptr;
  istr >> input;
  if (input!="IMAGE")
    return nullptr;
  istr >> input;
  if (input!="END;")
    return nullptr;
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

  if ((neu_u.size() != 20) || (den_u.size() != 20)) {
    std::cerr << "the input is not a valid rational camera\n";
    return nullptr;
  }

  T temp_vector[20];
  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_u[j];
  }
  for (int j=0; j<20; j++) {
    neu_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_u[j];
  }
  for (int j=0; j<20; j++) {
    den_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_v[j];
  }
  for (int j=0; j<20; j++) {
    neu_v[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_v[j];
  }
  for (int j=0; j<20; j++) {
    den_v[map[j]] = temp_vector[j];
  }

  vpgl_rational_camera<T>* cam = new vpgl_rational_camera<T>(neu_u, den_u, neu_v, den_v,
                                                             x_scale, x_off, y_scale, y_off, z_scale, z_off,
                                                             u_scale, u_off, v_scale, v_off);
  return cam;
}

//: Creates a rational camera from a txt file
// \relatesalso vpgl_rational_camera
template <class T>
vpgl_rational_camera<T>* read_rational_camera_from_txt(std::string cam_path)
{
  std::ifstream istr;
  istr.open(cam_path.c_str());
  if (!istr.good()) {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return nullptr;
  }

  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale,x_off,y_scale,y_off,z_scale,z_off,u_scale,u_off,v_scale,v_off;

  std::string input;
  char bulk[100];

  while (!istr.eof()) {
    istr >> input;

    if (input=="SAMP_SCALE:") {
      //istr >> input;
      istr >> u_scale;
    }
    if (input=="SAMP_OFF:") {
      //istr >> input;
      istr >> u_off;
    }

    if (input=="LINE_SCALE:") {
      //istr >> input;
      istr >> v_scale;
    }
    if (input=="LINE_OFF:") {
      //istr >> input;
      istr >> v_off;
    }

    if (input=="LONG_SCALE:") {
      //istr >> input;
      istr >> x_scale;
    }
    if (input=="LONG_OFF:") {
      //istr >> input;
      istr >> x_off;
    }

    if (input=="LAT_SCALE:") {
      //istr >> input;
      istr >> y_scale;
    }
    if (input=="LAT_OFF:") {
      //istr >> input;
      istr >> y_off;
    }

    if (input=="HEIGHT_SCALE:") {
      //istr >> input;
      istr >> z_scale;
    }
    if (input=="HEIGHT_OFF:") {
      //istr >> input;
      istr >> z_off;
    }

    T temp_dbl;
    if (input=="LINE_NUM_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      neu_v.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="LINE_DEN_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      den_v.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="SAMP_NUM_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      neu_u.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
    }

    if (input=="SAMP_DEN_COEFF_1:") {
      //istr >> input;
      //istr >> input;
      istr >> temp_dbl;
      den_u.push_back(temp_dbl);
      for (int i=1; i<20; i++) {
        istr >> input;
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk,200);
      }
      break;
    }
  }

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

  if ((neu_u.size() != 20) || (den_u.size() != 20)) {
    std::cerr << "the input is not a valid rational camera\n";
    return nullptr;
  }

  T temp_vector[20];
  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_u[j];
  }
  for (int j=0; j<20; j++) {
    neu_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_u[j];
  }
  for (int j=0; j<20; j++) {
    den_u[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = neu_v[j];
  }
  for (int j=0; j<20; j++) {
    neu_v[map[j]] = temp_vector[j];
  }
  for (int j=0; j<20; j++) {
    temp_vector[j] = den_v[j];
  }
  for (int j=0; j<20; j++) {
    den_v[map[j]] = temp_vector[j];
  }

  vpgl_rational_camera<T>* cam = new vpgl_rational_camera<T>(neu_u, den_u, neu_v, den_v,
                                                             x_scale, x_off, y_scale, y_off, z_scale, z_off,
                                                             u_scale, u_off, v_scale, v_off);

  istr.close();
  return cam;
}


//: Read from stream
template <class T>
std::istream&  operator >>(std::istream& s, vpgl_rational_camera<T >& c )
{
  vpgl_rational_camera<T>* cptr = read_rational_camera<T>(s);
  c = *cptr;
  return s;
}

// Code for easy instantiation.
#undef vpgl_RATIONAL_CAMERA_INSTANTIATE
#define vpgl_RATIONAL_CAMERA_INSTANTIATE(T) \
template class vpgl_scale_offset<T >; \
template class vpgl_rational_camera<T >; \
template std::ostream& operator<<(std::ostream&, const vpgl_rational_camera<T >&); \
template std::istream& operator>>(std::istream&, vpgl_rational_camera<T >&); \
template vpgl_rational_camera<T > * read_rational_camera(std::string); \
template vpgl_rational_camera<T > * read_rational_camera(std::istream&); \
template vpgl_rational_camera<T > * read_rational_camera_from_txt(std::string);


#endif // vpgl_rational_camera_hxx_
