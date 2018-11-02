// This is bbas/bpgl/bpgl_comp_rational_camera.hxx
#ifndef bpgl_comp_rational_camera_hxx_
#define bpgl_comp_rational_camera_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include "bpgl_comp_rational_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
//--------------------------------------
// Constructors
//

// Create an identity projection, i.e. (x,y,z) identically maps to (x,y)
template <class T>
bpgl_comp_rational_camera<T>::bpgl_comp_rational_camera():
  vpgl_rational_camera<T>()
{
  matrix_.set_identity();
}

//: Constructor from a rational camera and an affine matrix
template <class T>
bpgl_comp_rational_camera<T>::
bpgl_comp_rational_camera(vnl_matrix_fixed<T, 3,3> const& M,
                          vpgl_rational_camera<T> const& rcam) :
  vpgl_rational_camera<T>(rcam), matrix_(M)
{
}

//: Constructor from translation only
template <class T>
bpgl_comp_rational_camera<T>::
bpgl_comp_rational_camera(const T tu, const T tv,
                          vpgl_rational_camera<T> const& rcam) :
  vpgl_rational_camera<T>(rcam)
{
  matrix_.set_identity();
  set_translation(tu,tv);
}

//: Constructor from translation rotation only first rotate then translate
template <class T>
bpgl_comp_rational_camera<T>::
bpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                          vpgl_rational_camera<T> const& rcam):
  vpgl_rational_camera<T>(rcam)
{
  this->set_trans_rotation(tu, tv, angle_in_radians);
  set_translation(T(0),T(0));
}

//: Constructor with all transform parameters
template <class T>
bpgl_comp_rational_camera<T>::
bpgl_comp_rational_camera(const T tu, const T tv, const T angle_in_radians,
                          const T su, const T sv,
                          vpgl_rational_camera<T> const& rcam):
  vpgl_rational_camera<T>(rcam)
{
  this->set_all(tu, tv, angle_in_radians, su, sv);
  set_translation(T(0),T(0));
}

//: read from a file
template <class T>
bpgl_comp_rational_camera<T>::bpgl_comp_rational_camera(std::string cam_path)
{
  vpgl_rational_camera<T> *rcam = read_rational_camera<T >(cam_path);

  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good()) {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return;
  }
  vnl_matrix_fixed<T, 3,3> M;
  std::string input;
  bool good = false;
  while (!file_inp.eof()&&!good) {
    file_inp >> input;
    if (input=="affine_matrix")
    {
      file_inp >> M;
      good = true;
    }
  }
  if  (!good)
  {
    std::cout << "error: not a composite rational camera file\n";
    return;
  }
  *this = bpgl_comp_rational_camera<T>(M, *rcam);
}

template <class T>
bpgl_comp_rational_camera<T>* bpgl_comp_rational_camera<T>::clone(void) const
{
  return new bpgl_comp_rational_camera<T>(*this);
}

// Base projection method
template <class T>
void bpgl_comp_rational_camera<T>::project(const T x, const T y, const T z,
                                           T& u, T& v) const
{
  //first project with the rational camera
  T ur, vr;
  vpgl_rational_camera<T>::project(x, y, z, ur, vr);
  //transform by affine map
  vnl_vector_fixed<T, 3> p, pt;
  p[0]=ur;   p[1]=vr; p[2]=(T)1;
  pt = matrix_*p;
  u = pt[0];
  v = pt[1];
}

//vnl interface methods
template <class T>
vnl_vector_fixed<T, 2>
bpgl_comp_rational_camera<T>::project(vnl_vector_fixed<T, 3> const& world_point)const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2],
                image_point[0], image_point[1]);
  return image_point;
}

//vgl interface methods
template <class T>
vgl_point_2d<T> bpgl_comp_rational_camera<T>::project(vgl_point_3d<T> world_point)const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}

//: print the camera parameters
template <class T>
void bpgl_comp_rational_camera<T>::print(std::ostream& s) const
{
  vpgl_scale_offset<T> sox = this->scale_offsets()[this->X_INDX];
  vpgl_scale_offset<T> soy = this->scale_offsets()[this->Y_INDX];
  vpgl_scale_offset<T> soz = this->scale_offsets()[this->Z_INDX];
  vpgl_scale_offset<T> sou = this->scale_offsets()[this->U_INDX];
  vpgl_scale_offset<T> sov = this->scale_offsets()[this->V_INDX];

  s << "bpgl_comp_rational_camera:\n"
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
    << "[0] " << this->coefficient_matrix()[0][0]
    << " [1] " << this->coefficient_matrix()[0][1]
    << " [2] " << this->coefficient_matrix()[0][2]
    << " [3] " << this->coefficient_matrix()[0][3] <<'\n'
    << "[4] " << this->coefficient_matrix()[0][4]
    << " [5] " << this->coefficient_matrix()[0][5]
    << " [6] " << this->coefficient_matrix()[0][6]
    << " [7] " << this->coefficient_matrix()[0][7] <<'\n'
    << "[8] "  << this->coefficient_matrix()[0][8]
    << " [9] " << this->coefficient_matrix()[0][9]
    << " [10] " << this->coefficient_matrix()[0][10]
    << " [11] " << this->coefficient_matrix()[0][11] <<'\n'
    << "[12] " << this->coefficient_matrix()[0][12]
    << " [13] " << this->coefficient_matrix()[0][13]
    << " [14] " << this->coefficient_matrix()[0][14]
    << " [15] "  << this->coefficient_matrix()[0][15] <<'\n'
    << "[16] " << this->coefficient_matrix()[0][16]
    << " [17] " << this->coefficient_matrix()[0][17]
    << " [18] " << this->coefficient_matrix()[0][18]
    << " [19] " << this->coefficient_matrix()[0][19] <<"\n\n"

    << "U Denominator\n"
    << "[0] " << this->coefficient_matrix()[1][0]
    << " [1] " << this->coefficient_matrix()[1][1]
    << " [2] " << this->coefficient_matrix()[1][2]
    << " [3] " << this->coefficient_matrix()[1][3] <<'\n'
    << "[4] " << this->coefficient_matrix()[1][4]
    << " [5] " << this->coefficient_matrix()[1][5]
    << " [6] " << this->coefficient_matrix()[1][6]
    << " [7] " << this->coefficient_matrix()[1][7]  <<'\n'
    << "[8] " << this->coefficient_matrix()[1][8]
    << " [9] " << this->coefficient_matrix()[1][9]
    << " [10] " << this->coefficient_matrix()[1][10]
    << " [11] " << this->coefficient_matrix()[1][11] <<'\n'
    << "[12] " << this->coefficient_matrix()[1][12]
    << " [13] " << this->coefficient_matrix()[1][13]
    << " [14] " << this->coefficient_matrix()[1][14]
    << " [15] " << this->coefficient_matrix()[1][15] <<'\n'
    << "[16] " << this->coefficient_matrix()[1][16]
    << " [17] " << this->coefficient_matrix()[1][17]
    << " [18] " << this->coefficient_matrix()[1][18]
    << " [19] " << this->coefficient_matrix()[1][19] <<"\n\n"

    << "V Numerator\n"
    << "[0] " << this->coefficient_matrix()[2][0]
    << " [1] " << this->coefficient_matrix()[2][1]
    << " [2] " << this->coefficient_matrix()[2][2]
    << " [3] " << this->coefficient_matrix()[2][3]<<'\n'
    << "[4] " << this->coefficient_matrix()[2][4]
    << " [5] " << this->coefficient_matrix()[2][5]
    << " [6] " << this->coefficient_matrix()[2][6]
    << " [7] " << this->coefficient_matrix()[2][7] <<'\n'
    << "[8] " << this->coefficient_matrix()[2][8]
    << " [9] " << this->coefficient_matrix()[2][9]
    << " [10] " << this->coefficient_matrix()[2][10]
    << " [11] " << this->coefficient_matrix()[2][11] <<'\n'
    << "[12] " << this->coefficient_matrix()[2][12]
    << " [13] " << this->coefficient_matrix()[2][13]
    << " [14] " << this->coefficient_matrix()[2][14]
    << " [15] " << this->coefficient_matrix()[2][15]<<'\n'
    << "[16] " << this->coefficient_matrix()[2][16]
    << " [17] " << this->coefficient_matrix()[2][17]
    << " [18] " << this->coefficient_matrix()[2][18]
    << " [19] " << this->coefficient_matrix()[2][19] <<"\n\n"

    << "V Denominator\n"
    << "[0] " << this->coefficient_matrix()[3][0]
    << " [1] " << this->coefficient_matrix()[3][1]
    << " [2] " << this->coefficient_matrix()[3][2]
    << " [3] " << this->coefficient_matrix()[3][3]<<'\n'
    << "[4] " << this->coefficient_matrix()[3][4]
    << " [5] " << this->coefficient_matrix()[3][5]
    << " [6] " << this->coefficient_matrix()[3][6]
    << " [7] " << this->coefficient_matrix()[3][7] <<'\n'
    << "[8] " << this->coefficient_matrix()[3][8]
    << " [9] " << this->coefficient_matrix()[3][9]
    << " [10] " << this->coefficient_matrix()[3][10]
    << " [11] " << this->coefficient_matrix()[3][11] <<'\n'
    << "[12] " << this->coefficient_matrix()[3][12]
    << " [13] " << this->coefficient_matrix()[3][13]
    << " [14] " << this->coefficient_matrix()[3][14]
    << " [15] " << this->coefficient_matrix()[3][15]<<'\n'
    << "[16] " << this->coefficient_matrix()[3][16]
    << " [17] " << this->coefficient_matrix()[3][17]
    << " [18] " << this->coefficient_matrix()[3][18]
    << " [19] " << this->coefficient_matrix()[3][19] <<'\n'
    << "\nAffine Matrix\n"
    << matrix_ << '\n'
    <<"------------------------------------------------\n\n";
}

template <class T>
bool bpgl_comp_rational_camera<T>::save(std::string cam_path)
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
           << "  lineOffset = " << this->offset(this->V_INDX) << '\n'
           << "  sampOffset = " << this->offset(this->U_INDX) << '\n'
           << "  latOffset = " << this->offset(this->Y_INDX) << '\n'
           << "  longOffset = " << this->offset(this->X_INDX) << '\n'
           << "  heightOffset = " << this->offset(this->Z_INDX) << '\n'
           << "  lineScale = " << this->scale(this->V_INDX) << '\n'
           << "  sampScale = " << this->scale(this->U_INDX) << '\n'
           << "  latScale = " << this->scale(this->Y_INDX) << '\n'
           << "  longScale = " << this->scale(this->X_INDX) << '\n'
           << "  heightScale = " << this->scale(this->Z_INDX) << '\n';
  vnl_matrix_fixed<double,4,20> coeffs = this->coefficient_matrix();
  file_out << "  lineNumCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[this->NEU_V][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  lineDenCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[this->DEN_V][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  sampNumCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[this->NEU_U][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n  sampDenCoef = (";
  for (int i=0; i<20; i++) {
    file_out << "\n    " << coeffs[this->DEN_U][map[i]];
    if (i < 19)
      file_out << ',';
  }
  file_out << ");\n"
           << "END_GROUP = IMAGE\n"
           << "END;\n"

           << "affine_matrix\n"
           << matrix_;
  return true;
}


template <class T>
void
bpgl_comp_rational_camera<T>::set_transform(vnl_matrix_fixed<T, 3,3> const& M)
{
  matrix_ = M;
}

template <class T>
void
bpgl_comp_rational_camera<T>::set_translation(const T tu, const T tv)
{
  matrix_[0][2]=tu;
  matrix_[1][2]=tv;
}

template <class T>
void
bpgl_comp_rational_camera<T>::set_trans_rotation(const T tu, const T tv,
                                                 const T angle_in_radians)
{
  double theta_d = static_cast<double>(angle_in_radians);
  double c = std::cos(theta_d), s = std::sin(theta_d);
  T ct = static_cast<T>(c), st = static_cast<T>(s);
  matrix_[0][0] = ct; matrix_[0][1] = -st; matrix_[0][2] = tu;
  matrix_[1][0] = st; matrix_[1][1] = ct;  matrix_[1][2] = tv;
}

template <class T>
void
bpgl_comp_rational_camera<T>::set_all(const T tu, const T tv,
                                      const T angle_in_radians,
                                      const T su, const T sv)
{
  double theta_d = static_cast<double>(angle_in_radians);
  double c = std::cos(theta_d), s = std::sin(theta_d);
  T ct = static_cast<T>(c), st = static_cast<T>(s);
  matrix_[0][0] = ct*su; matrix_[0][1] = -st*sv; matrix_[0][2] = tu;
  matrix_[1][0] = st*su; matrix_[1][1] = ct*sv;  matrix_[1][2] = tv;
}

template <class T>
vnl_matrix_fixed<T, 3,3> bpgl_comp_rational_camera<T>::transform()
{
  return matrix_;
}

template <class T>
void bpgl_comp_rational_camera<T>::translation(T& tu, T& tv)
{
  tu = matrix_[0][2];
  tv = matrix_[1][2];
}

template <class T>
T bpgl_comp_rational_camera<T>::rotation_in_radians()
{
  double y = static_cast<double>(matrix_[1][0]);
  double x = static_cast<double>(matrix_[0][0]);
  double angle = std::atan2(y, x);
  return static_cast<T>(angle);
}

//: note that scale factors are not negative
template <class T>
void bpgl_comp_rational_camera<T>::image_scale(T& su, T& sv)
{
  double su_sq = static_cast<double>(matrix_[0][0]*matrix_[0][0] +
                                     matrix_[1][0]*matrix_[1][0]);
  double sv_sq = static_cast<double>(matrix_[0][1]*matrix_[0][1] +
                                     matrix_[1][1]*matrix_[1][1]);
  double sud = std::sqrt(su_sq);
  double svd = std::sqrt(sv_sq);
  su = static_cast<T>(sud);
  sv = static_cast<T>(svd);
}

//: Write to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, const bpgl_comp_rational_camera<T >& c )
{
  c.print(s);
  return s;
}


// Code for easy instantiation.
#undef BPGL_COMP_RATIONAL_CAMERA_INSTANTIATE
#define BPGL_COMP_RATIONAL_CAMERA_INSTANTIATE(T) \
template class bpgl_comp_rational_camera<T >; \
template std::ostream& operator<<(std::ostream&, const bpgl_comp_rational_camera<T >&)


#endif // bpgl_comp_rational_camera_hxx_
