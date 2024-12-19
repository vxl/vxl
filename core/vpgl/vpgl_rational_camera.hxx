// This is core/vpgl/vpgl_rational_camera.hxx
#ifndef vpgl_rational_camera_hxx_
#define vpgl_rational_camera_hxx_
//:
// \file

#include <vector>
#include <fstream>
#include <iomanip>
#include "vpgl_rational_camera.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// #include <vnl/io/vnl_io_matrix_fixed.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>


//--------------------------------------
// Constructors

// Create an identity projection, i.e. (x,y) identically maps to (u,v)
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera()
{
  rational_coeffs_.fill(0);
  rational_coeffs_[DEN_U][19] = 1;
  rational_coeffs_[DEN_V][19] = 1;
  rational_coeffs_[NEU_U][9] = 1;  // x coefficient
  rational_coeffs_[NEU_V][15] = 1; // y coefficient
  vpgl_scale_offset<T> soff;
  scale_offsets_.resize(5, soff);
}


template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera(const std::vector<T> & neu_u,
                                              const std::vector<T> & den_u,
                                              const std::vector<T> & neu_v,
                                              const std::vector<T> & den_v,
                                              const T x_scale,
                                              const T x_off,
                                              const T y_scale,
                                              const T y_off,
                                              const T z_scale,
                                              const T z_off,
                                              const T u_scale,
                                              const T u_off,
                                              const T v_scale,
                                              const T v_off,
                                              vpgl_rational_order input_order)
{
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off);
}

//: Constructor from 4 coefficient arrays and 5 scale, offset pairs.
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera(const double * neu_u,
                                              const double * den_u,
                                              const double * neu_v,
                                              const double * den_v,
                                              const T x_scale,
                                              const T x_off,
                                              const T y_scale,
                                              const T y_off,
                                              const T z_scale,
                                              const T z_off,
                                              const T u_scale,
                                              const T u_off,
                                              const T v_scale,
                                              const T v_off,
                                              vpgl_rational_order input_order)
{
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off);
}

// Constructor with an array encoding of the coefficients
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera(const std::vector<std::vector<T>> & rational_coeffs,
                                              const std::vector<vpgl_scale_offset<T>> & scale_offsets,
                                              vpgl_rational_order input_order)
{
  this->set_coefficients(rational_coeffs, input_order);
  this->set_scale_offsets(scale_offsets);
}

// Constructor with a vnl matrix of coefficients
template <class T>
vpgl_rational_camera<T>::vpgl_rational_camera(const vnl_matrix_fixed<T, 4, 20> & rational_coeffs,
                                              const std::vector<vpgl_scale_offset<T>> & scale_offsets,
                                              vpgl_rational_order input_order)
{
  this->set_coefficients(rational_coeffs, input_order);
  this->set_scale_offsets(scale_offsets);
}


//--------------------------------------
// Clone

// clone rational camera
template <class T>
vpgl_rational_camera<T> *
vpgl_rational_camera<T>::clone() const
{
  return new vpgl_rational_camera<T>(*this);
}

//--------------------------------------
// Set coefficient matrix

// set coefficients from 4 vectors
template <class T>
void
vpgl_rational_camera<T>::set_coefficients(const std::vector<T> & neu_u,
                                          const std::vector<T> & den_u,
                                          const std::vector<T> & neu_v,
                                          const std::vector<T> & den_v,
                                          vpgl_rational_order input_order)
{
  vnl_matrix_fixed<T, 4, 20> coeffs;
  for (unsigned i = 0; i < 20; ++i)
  {
    coeffs[NEU_U][i] = neu_u[i];
    coeffs[DEN_U][i] = den_u[i];
    coeffs[NEU_V][i] = neu_v[i];
    coeffs[DEN_V][i] = den_v[i];
  }
  this->set_coefficients(coeffs, input_order);
}

// set coefficients from 4 vector pointers
template <class T>
void
vpgl_rational_camera<T>::set_coefficients(const double * neu_u,
                                          const double * den_u,
                                          const double * neu_v,
                                          const double * den_v,
                                          vpgl_rational_order input_order)
{
  vnl_matrix_fixed<T, 4, 20> coeffs;
  for (unsigned i = 0; i < 20; ++i)
  {
    coeffs[NEU_U][i] = neu_u[i];
    coeffs[DEN_U][i] = den_u[i];
    coeffs[NEU_V][i] = neu_v[i];
    coeffs[DEN_V][i] = den_v[i];
  }
  this->set_coefficients(coeffs, input_order);
}

// set coefficients from array encoding
template <class T>
void
vpgl_rational_camera<T>::set_coefficients(const std::vector<std::vector<T>> & rational_coeffs,
                                          vpgl_rational_order input_order)
{
  auto order = vpgl_rational_order_func::to_vector(input_order);
  for (unsigned j = 0; j < 4; ++j)
    for (unsigned i = 0; i < 20; ++i)
      rational_coeffs_[j][i] = rational_coeffs[j][order[i]];
}

// set coefficients from vnl matrix
template <class T>
void
vpgl_rational_camera<T>::set_coefficients(const vnl_matrix_fixed<T, 4, 20> & rational_coeffs,
                                          vpgl_rational_order input_order)
{
  auto order = vpgl_rational_order_func::to_vector(input_order);
  for (unsigned j = 0; j < 4; ++j)
    for (unsigned i = 0; i < 20; ++i)
      rational_coeffs_[j][i] = rational_coeffs[j][order[i]];
}

//--------------------------------------
// Get coefficient matrix

// get coefficients as vnl matrix
template <class T>
vnl_matrix_fixed<T, 4, 20>
vpgl_rational_camera<T>::coefficient_matrix(vpgl_rational_order output_order) const
{
  auto order = vpgl_rational_order_func::to_vector(output_order);
  vnl_matrix_fixed<T, 4, 20> result;
  for (unsigned j = 0; j < 4; ++j)
    for (unsigned i = 0; i < 20; ++i)
      result[j][order[i]] = rational_coeffs_[j][i];
  return result;
}

// get coefficients as std vector of vectors
template <class T>
std::vector<std::vector<T>>
vpgl_rational_camera<T>::coefficients(vpgl_rational_order output_order) const
{
  auto order = vpgl_rational_order_func::to_vector(output_order);
  std::vector<std::vector<T>> result(4, std::vector<T>(20));
  for (unsigned j = 0; j < 4; ++j)
    for (unsigned i = 0; i < 20; ++i)
      result[j][order[i]] = rational_coeffs_[j][i];
  return result;
}


//--------------------------------------
// Set scale/offset values

// set all scale offsets from individual values
template <class T>
void
vpgl_rational_camera<T>::set_scale_offsets(const T x_scale,
                                           const T x_off,
                                           const T y_scale,
                                           const T y_off,
                                           const T z_scale,
                                           const T z_off,
                                           const T u_scale,
                                           const T u_off,
                                           const T v_scale,
                                           const T v_off)
{
  scale_offsets_.resize(5);
  scale_offsets_[X_INDX] = vpgl_scale_offset<T>(x_scale, x_off);
  scale_offsets_[Y_INDX] = vpgl_scale_offset<T>(y_scale, y_off);
  scale_offsets_[Z_INDX] = vpgl_scale_offset<T>(z_scale, z_off);
  scale_offsets_[U_INDX] = vpgl_scale_offset<T>(u_scale, u_off);
  scale_offsets_[V_INDX] = vpgl_scale_offset<T>(v_scale, v_off);
}

// set all scale offsets from vector
template <class T>
void
vpgl_rational_camera<T>::set_scale_offsets(const std::vector<vpgl_scale_offset<T>> & scale_offsets)
{
  scale_offsets_ = scale_offsets;
}


//--------------------------------------
// Utility functions

//: Create a vector with the internal VXL order of monomial terms
template <class T>
vnl_vector_fixed<T, 20>
vpgl_rational_camera<T>::power_vector(const T x, const T y, const T z) const
{
  // Form the monomials in homogeneous form
  double w = 1;
  double xx = x * x;
  double xy = x * y;
  double xz = x * z;
  double yy = y * y;
  double yz = y * z;
  double zz = z * z;
  double xxx = x * xx;
  double xxy = x * xy;
  double xxz = x * xz;
  double xyy = x * yy;
  double xyz = x * yz;
  double xzz = x * zz;
  double yyy = y * yy;
  double yyz = y * yz;
  double yzz = y * zz;
  double zzz = z * zz;
  double xww = x * w * w;
  double yww = y * w * w;
  double zww = z * w * w;
  double www = w * w * w;
  double xxw = xx * w;
  double xyw = xy * w;
  double xzw = xz * w;
  double yyw = yy * w;
  double yzw = yz * w;
  double zzw = zz * w;

  // fill the vector
  vnl_vector_fixed<T, 20> pv;
  pv.put(0, T(xxx));
  pv.put(1, T(xxy));
  pv.put(2, T(xxz));
  pv.put(3, T(xxw));
  pv.put(4, T(xyy));
  pv.put(5, T(xyz));
  pv.put(6, T(xyw));
  pv.put(7, T(xzz));
  pv.put(8, T(xzw));
  pv.put(9, T(xww));
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


//--------------------------------------
// Project 3D world point into 2D image space

// generic interface
template <class T>
void
vpgl_rational_camera<T>::project(const T x, const T y, const T z, T & u, T & v) const
{
  // scale, offset the world point before projection
  T sx = scale_offsets_[X_INDX].normalize(x);
  T sy = scale_offsets_[Y_INDX].normalize(y);
  T sz = scale_offsets_[Z_INDX].normalize(z);
  // projection
  vnl_vector_fixed<T, 4> polys = rational_coeffs_ * power_vector(sx, sy, sz);
  T su = polys[NEU_U] / polys[DEN_U];
  T sv = polys[NEU_V] / polys[DEN_V];
  // unscale the resulting image coordinates
  u = scale_offsets_[U_INDX].un_normalize(su);
  v = scale_offsets_[V_INDX].un_normalize(sv);
}

// vnl interface
template <class T>
vnl_vector_fixed<T, 2>
vpgl_rational_camera<T>::project(const vnl_vector_fixed<T, 3> & world_point) const
{
  vnl_vector_fixed<T, 2> image_point;
  this->project(world_point[0], world_point[1], world_point[2], image_point[0], image_point[1]);
  return image_point;
}

// vgl interface
template <class T>
vgl_point_2d<T>
vpgl_rational_camera<T>::project(vgl_point_3d<T> world_point) const
{
  T u = 0, v = 0;
  this->project(world_point.x(), world_point.y(), world_point.z(), u, v);
  return vgl_point_2d<T>(u, v);
}


//--------------------------------------
// Output

// write camera parameters to output stream as PVL (paramter value language)
template <class T>
void
vpgl_rational_camera<T>::write_pvl(std::ostream & ostr, vpgl_rational_order output_order) const
{
  // current ostream settings (restore at end of function)
  auto ostr_flags = ostr.flags();
  auto ostr_precision = ostr.precision();

  // print header & scale/offset values
  ostr << "satId = \"????\";\n"
       << "bandId = \"RGB\";\n"
       << "SpecId = \"" << vpgl_rational_order_func::to_string(output_order) << "\";" << std::endl;

  // begin image group
  ostr << "BEGIN_GROUP = IMAGE" << std::endl;

  // print scale/offset values
  ostr << std::fixed;
  ostr.precision(6);

  ostr << std::endl // skip errBias
       << std::endl // skip errRand
       << "\tlineOffset = " << offset(V_INDX) << std::endl
       << "\tsampOffset = " << offset(U_INDX) << std::endl
       << "\tlatOffset = " << offset(Y_INDX) << std::endl
       << "\tlongOffset = " << offset(X_INDX) << std::endl
       << "\theightOffset = " << offset(Z_INDX) << std::endl
       << "\tlineScale = " << scale(V_INDX) << std::endl
       << "\tsampScale = " << scale(U_INDX) << std::endl
       << "\tlatScale = " << scale(Y_INDX) << std::endl
       << "\tlongScale = " << scale(X_INDX) << std::endl
       << "\theightScale = " << scale(Z_INDX) << std::endl;

  // print coefficients in specified order
  auto coeffs = this->coefficient_matrix(output_order);

  std::vector<std::pair<std::string, poly_index>> items;
  items.push_back(std::make_pair("lineNumCoef", NEU_V));
  items.push_back(std::make_pair("lineDenCoef", DEN_V));
  items.push_back(std::make_pair("sampNumCoef", NEU_U));
  items.push_back(std::make_pair("sampDenCoef", DEN_U));

  ostr << std::scientific << std::showpos;
  ostr.precision(12);

  for (const auto & item : items)
  {
    ostr << "\t" << item.first << " = (" << std::endl;
    for (int i = 0; i < 20; i++)
    {
      ostr << "\t\t" << coeffs[item.second][i];
      if (i < 19)
        ostr << "," << std::endl;
      else
        ostr << ");" << std::endl;
    }
  }

  // end image group
  ostr << "END_GROUP = IMAGE" << std::endl;

  // footer
  ostr << "END;" << std::endl;

  // restore ostream settings
  ostr.flags(ostr_flags);
  ostr.precision(ostr_precision);
}

// print camera parameters to output stream
template <class T>
void
vpgl_rational_camera<T>::print(std::ostream & ostr, vpgl_rational_order output_order) const
{
  this->write_pvl(ostr, output_order);
}

// save camera parameters to file
template <class T>
bool
vpgl_rational_camera<T>::save(std::string cam_path, vpgl_rational_order output_order) const
{
  // open file
  std::ofstream file_out;
  file_out.open(cam_path.c_str());
  if (!file_out.good())
  {
    std::cerr << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // print to file
  this->write_pvl(file_out, output_order);

  // cleanup
  file_out.close();
  return true;
}


//--------------------------------------
// Input

// read from PVL (parameter value language) file
template <class T>
bool
vpgl_rational_camera<T>::read_pvl(std::string cam_path)
{
  // open file
  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good())
  {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // read from file stream
  bool success = read_pvl(file_inp);

  // cleanup
  file_inp.close();
  return success;
}

// read from PVL input stream
template <class T>
bool
vpgl_rational_camera<T>::read_pvl(std::istream & istr)
{
  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off;

  bool has_xs = false, has_xo = false, has_ys = false, has_yo = false, has_zs = false, has_zo = false;
  bool has_us = false, has_uo = false, has_vs = false, has_vo = false;

  // assume RPC00B ordering as default
  auto input_order = vpgl_rational_order::RPC00B;

  std::string input;
  char bulk[100];
  T temp_dbl;

  while (!istr.eof())
  {
    istr >> input;

    if (input == "SpecId")
    {
      istr >> input;
      istr >> input;
      try
      {
        input_order = vpgl_rational_order_func::from_string(input);
      }
      catch (const std::exception & err)
      {
        std::cerr << "<" << input << "> unrecognized as vpgl_rational_order" << std::endl;
        std::cerr << err.what() << std::endl;
        return false;
      }
    }

    else if (input == "sampScale")
    {
      istr >> input;
      istr >> u_scale;
      has_us = true;
    }
    else if (input == "sampOffset")
    {
      istr >> input;
      istr >> u_off;
      has_uo = true;
    }

    else if (input == "lineScale")
    {
      istr >> input;
      istr >> v_scale;
      has_vs = true;
    }
    else if (input == "lineOffset")
    {
      istr >> input;
      istr >> v_off;
      has_vo = true;
    }

    else if (input == "longScale")
    {
      istr >> input;
      istr >> x_scale;
      has_xs = true;
    }
    else if (input == "longOffset")
    {
      istr >> input;
      istr >> x_off;
      has_xo = true;
    }

    else if (input == "latScale")
    {
      istr >> input;
      istr >> y_scale;
      has_ys = true;
    }
    else if (input == "latOffset")
    {
      istr >> input;
      istr >> y_off;
      has_yo = true;
    }

    else if (input == "heightScale")
    {
      istr >> input;
      istr >> z_scale;
      has_zs = true;
    }
    else if (input == "heightOffset")
    {
      istr >> input;
      istr >> z_off;
      has_zo = true;
    }

    else if (input == "lineNumCoef")
    {
      istr >> input;
      istr >> input;
      for (int i = 0; i < 20; i++)
      {
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "lineDenCoef")
    {
      istr >> input;
      istr >> input;
      for (int i = 0; i < 20; i++)
      {
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "sampNumCoef")
    {
      istr >> input;
      istr >> input;
      for (int i = 0; i < 20; i++)
      {
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "sampDenCoef")
    {
      istr >> input;
      istr >> input;
      for (int i = 0; i < 20; i++)
      {
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
      break;
    }
  }

  istr >> input;
  if (input != "END_GROUP")
    return false;
  istr >> input;
  if (input != "=")
    return false;
  istr >> input;
  if (input != "IMAGE")
    return false;
  istr >> input;
  if (input != "END;")
    return false;

  if ((!has_xs) || (!has_xo) || (!has_ys) || (!has_yo) || (!has_zs) || (!has_zo) || (!has_us) || (!has_uo) ||
      (!has_vs) || (!has_vo))
  {
    std::cerr << "rational camera missing scale/offset values" << std::endl;
    return false;
  }

  if ((neu_u.size() != 20) || (den_u.size() != 20) || (neu_v.size() != 20) || (den_v.size() != 20))
  {
    std::cerr << "the input is not a valid rational camera" << std::endl;
    return false;
  }

  // set values & cleanup
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off);
  return true;
}


// read from TXT file
template <class T>
bool
vpgl_rational_camera<T>::read_txt(std::string cam_path)
{
  // open file
  std::ifstream file_inp;
  file_inp.open(cam_path.c_str());
  if (!file_inp.good())
  {
    std::cout << "error: bad filename: " << cam_path << std::endl;
    return false;
  }

  // read from file stream
  bool success = read_txt(file_inp);

  // cleanup
  file_inp.close();
  return success;
}

// read from TXT input stream
template <class T>
bool
vpgl_rational_camera<T>::read_txt(std::istream & istr)
{
  std::vector<T> neu_u;
  std::vector<T> den_u;
  std::vector<T> neu_v;
  std::vector<T> den_v;
  T x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off;

  bool has_xs = false, has_xo = false, has_ys = false, has_yo = false, has_zs = false, has_zo = false;
  bool has_us = false, has_uo = false, has_vs = false, has_vo = false;

  // assume RPC00B ordering as default
  auto input_order = vpgl_rational_order::RPC00B;

  std::string input;
  char bulk[100];
  T temp_dbl;

  while (!istr.eof())
  {
    istr >> input;

    if (input == "SAMP_SCALE:")
    {
      // istr >> input;
      istr >> u_scale;
      has_us = true;
    }
    else if (input == "SAMP_OFF:")
    {
      // istr >> input;
      istr >> u_off;
      has_uo = true;
    }

    else if (input == "LINE_SCALE:")
    {
      // istr >> input;
      istr >> v_scale;
      has_vs = true;
    }
    else if (input == "LINE_OFF:")
    {
      // istr >> input;
      istr >> v_off;
      has_vo = true;
    }

    else if (input == "LONG_SCALE:")
    {
      // istr >> input;
      istr >> x_scale;
      has_xs = true;
    }
    else if (input == "LONG_OFF:")
    {
      // istr >> input;
      istr >> x_off;
      has_xo = true;
    }

    else if (input == "LAT_SCALE:")
    {
      // istr >> input;
      istr >> y_scale;
      has_ys = true;
    }
    else if (input == "LAT_OFF:")
    {
      // istr >> input;
      istr >> y_off;
      has_yo = true;
    }

    else if (input == "HEIGHT_SCALE:")
    {
      // istr >> input;
      istr >> z_scale;
      has_zs = true;
    }
    else if (input == "HEIGHT_OFF:")
    {
      // istr >> input;
      istr >> z_off;
      has_zo = true;
    }

    else if (input == "LINE_NUM_COEFF_1:")
    {
      // istr >> input;
      // istr >> input;
      istr >> temp_dbl;
      neu_v.push_back(temp_dbl);
      for (int i = 1; i < 20; i++)
      {
        istr >> input;
        istr >> temp_dbl;
        neu_v.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "LINE_DEN_COEFF_1:")
    {
      // istr >> input;
      // istr >> input;
      istr >> temp_dbl;
      den_v.push_back(temp_dbl);
      for (int i = 1; i < 20; i++)
      {
        istr >> input;
        istr >> temp_dbl;
        den_v.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "SAMP_NUM_COEFF_1:")
    {
      // istr >> input;
      // istr >> input;
      istr >> temp_dbl;
      neu_u.push_back(temp_dbl);
      for (int i = 1; i < 20; i++)
      {
        istr >> input;
        istr >> temp_dbl;
        neu_u.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
    }

    else if (input == "SAMP_DEN_COEFF_1:")
    {
      // istr >> input;
      // istr >> input;
      istr >> temp_dbl;
      den_u.push_back(temp_dbl);
      for (int i = 1; i < 20; i++)
      {
        istr >> input;
        istr >> temp_dbl;
        den_u.push_back(temp_dbl);
        istr.getline(bulk, 200);
      }
      break;
    }
  }

  if ((!has_xs) || (!has_xo) || (!has_ys) || (!has_yo) || (!has_zs) || (!has_zo) || (!has_us) || (!has_uo) ||
      (!has_vs) || (!has_vo))
  {
    std::cerr << "rational camera missing scale/offset values" << std::endl;
    return false;
  }

  if ((neu_u.size() != 20) || (den_u.size() != 20) || (neu_v.size() != 20) || (den_v.size() != 20))
  {
    std::cerr << "the input is not a valid rational camera" << std::endl;
    return false;
  }

  // set values & cleanup
  this->set_coefficients(neu_u, den_u, neu_v, den_v, input_order);
  this->set_scale_offsets(x_scale, x_off, y_scale, y_off, z_scale, z_off, u_scale, u_off, v_scale, v_off);
  return true;
}


//--------------------------------------
// Convenience functions

// write to stream
template <class T>
std::ostream &
operator<<(std::ostream & s, const vpgl_rational_camera<T> & c)
{
  c.print(s);
  return s;
}

// read from stream
template <class T>
std::istream &
operator>>(std::istream & s, vpgl_rational_camera<T> & c)
{
  c.read_pvl(s);
  return s;
}

// read from a PVL file/stream
template <class T>
vpgl_rational_camera<T> *
read_rational_camera(std::string cam_path)
{
  vpgl_rational_camera<T> cam;
  if (!cam.read_pvl(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_rational_camera<T> *
read_rational_camera(std::istream & istr)
{
  vpgl_rational_camera<T> cam;
  if (!cam.read_pvl(istr))
    return nullptr;
  else
    return cam.clone();
}

// read from a TXT file/stream
template <class T>
vpgl_rational_camera<T> *
read_rational_camera_from_txt(std::string cam_path)
{
  vpgl_rational_camera<T> cam;
  if (!cam.read_txt(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_rational_camera<T> *
read_rational_camera_from_txt(std::istream & istr)
{
  vpgl_rational_camera<T> cam;
  if (!cam.read_txt(istr))
    return nullptr;
  else
    return cam.clone();
}


// Code for easy instantiation.
#undef vpgl_RATIONAL_CAMERA_INSTANTIATE
#define vpgl_RATIONAL_CAMERA_INSTANTIATE(T)                                            \
  template class vpgl_scale_offset<T>;                                                 \
  template class vpgl_rational_camera<T>;                                              \
  template std::ostream & operator<<(std::ostream &, const vpgl_rational_camera<T> &); \
  template std::istream & operator>>(std::istream &, vpgl_rational_camera<T> &);       \
  template vpgl_rational_camera<T> * read_rational_camera(std::string);                \
  template vpgl_rational_camera<T> * read_rational_camera(std::istream &);             \
  template vpgl_rational_camera<T> * read_rational_camera_from_txt(std::string);       \
  template vpgl_rational_camera<T> * read_rational_camera_from_txt(std::istream &)


#endif // vpgl_rational_camera_hxx_
