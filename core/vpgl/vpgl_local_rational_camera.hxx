// This is core/vpgl/vpgl_local_rational_camera.hxx
#ifndef vpgl_local_rational_camera_hxx_
#define vpgl_local_rational_camera_hxx_
//:
// \file
#include <vector>
#include <fstream>
#include <iomanip>
#include "vpgl_local_rational_camera.h"
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
vpgl_local_rational_camera<T>::vpgl_local_rational_camera()
  : vpgl_rational_camera<T>()
{}

//: Constructor from a rational camera and an affine matrix
template <class T>
vpgl_local_rational_camera<T>::vpgl_local_rational_camera(const vpgl_lvcs & lvcs, const vpgl_rational_camera<T> & rcam)
  : vpgl_rational_camera<T>(rcam)
  , lvcs_(lvcs)
{}

//: Constructor from a rational camera and a geographic origin
template <class T>
vpgl_local_rational_camera<T>::vpgl_local_rational_camera(T longitude,
                                                          T latitude,
                                                          T elevation,
                                                          const vpgl_rational_camera<T> & rcam)
  : vpgl_rational_camera<T>(rcam)
  , lvcs_(vpgl_lvcs(latitude, longitude, elevation, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS))
{}

// clone local rational camera
template <class T>
vpgl_local_rational_camera<T> *
vpgl_local_rational_camera<T>::clone() const
{
  return new vpgl_local_rational_camera<T>(*this);
}

//--------------------------------------
// Accessor/Mutator

// set lvcs from lon/lat/elev
template <class T>
void
vpgl_local_rational_camera<T>::set_lvcs(const double & longitude, const double & latitude, const double & elevation)
{
  lvcs_ = vpgl_lvcs(latitude, longitude, elevation, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
}


//--------------------------------------
// Point projections

// generic interface, x, y, z are in local Cartesian coordinates
template <class T>
void
vpgl_local_rational_camera<T>::project(const T x, const T y, const T z, T & u, T & v) const
{
  // first, convert to global geographic coordinates
  double lon, lat, gz;
  lvcs_.local_to_global(x, y, z, vpgl_lvcs::wgs84, lon, lat, gz);

  // then, project global to 2D
  vpgl_rational_camera<T>::project((T)lon, (T)lat, (T)gz, u, v);
}


//--------------------------------------
// Output

// write camera parameters to output stream as PVL (paramter value language)
template <class T>
void
vpgl_local_rational_camera<T>::write_pvl(std::ostream & ostr, vpgl_rational_order output_order) const
{
  // parent print
  vpgl_rational_camera<T>::write_pvl(ostr, output_order);

  // append lvcs
  double longitude, latitude, elevation;
  lvcs_.get_origin(latitude, longitude, elevation);
  ostr << "lvcs" << std::endl
       << std::setprecision(12) << longitude << std::endl
       << std::setprecision(12) << latitude << std::endl
       << std::setprecision(12) << elevation << std::endl;
}


//--------------------------------------
// Input

// read from a PVL file stream
template <class T>
bool
vpgl_local_rational_camera<T>::read_pvl(std::istream & istr)
{
  // read rational camera
  if (!vpgl_rational_camera<T>::read_pvl(istr))
    return false;

  // read lvcs
  double longitude, latitude, elevation;
  bool has_lvcs = false;
  std::string input;

  while (!istr.eof() && !has_lvcs)
  {
    istr >> input;
    if (input == "lvcs")
    {
      istr >> longitude >> latitude >> elevation;
      has_lvcs = true;
    }
  }

  if (!has_lvcs)
  {
    // std::cerr << "error: not a composite rational camera file\n";
    return false;
  }

  // store lvcs & return
  this->set_lvcs(longitude, latitude, elevation);
  return true;
}

// read from a TXT file stream
template <class T>
bool
vpgl_local_rational_camera<T>::read_txt(std::istream & istr)
{
  // read rational camera
  if (!vpgl_rational_camera<T>::read_txt(istr))
    return false;

  // read lvcs
  double longitude, latitude, elevation;
  bool has_lvcs = false;
  std::string input;

  while (!istr.eof() && !has_lvcs)
  {
    istr >> input;
    if (input == "lvcs")
    {
      istr >> longitude >> latitude >> elevation;
      has_lvcs = true;
    }
  }

  if (!has_lvcs)
  {
    // std::cerr << "error: not a composite rational camera file\n";
    return false;
  }

  // store lvcs & return
  this->set_lvcs(longitude, latitude, elevation);
  return true;
}


//--------------------------------------
// Convenience functions

// write to stream
template <class T>
std::ostream &
operator<<(std::ostream & s, const vpgl_local_rational_camera<T> & c)
{
  c.print(s);
  return s;
}

// read from stream
template <class T>
std::istream &
operator>>(std::istream & s, vpgl_local_rational_camera<T> & c)
{
  c.read_pvl(s);
  return s;
}

// read from a PVL file/stream
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera(std::string cam_path)
{
  vpgl_local_rational_camera<T> cam;
  if (!cam.read_pvl(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera(std::istream & istr)
{
  vpgl_local_rational_camera<T> cam;
  if (!cam.read_pvl(istr))
    return nullptr;
  else
    return cam.clone();
}

// read from a TXT file/stream
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera_from_txt(std::string cam_path)
{
  vpgl_local_rational_camera<T> cam;
  if (!cam.read_txt(cam_path))
    return nullptr;
  else
    return cam.clone();
}

template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera_from_txt(std::istream & istr)
{
  vpgl_local_rational_camera<T> cam;
  if (!cam.read_txt(istr))
    return nullptr;
  else
    return cam.clone();
}


// Code for easy instantiation.
#undef vpgl_LOCAL_RATIONAL_CAMERA_INSTANTIATE
#define vpgl_LOCAL_RATIONAL_CAMERA_INSTANTIATE(T)                                            \
  template class vpgl_local_rational_camera<T>;                                              \
  template std::ostream & operator<<(std::ostream &, const vpgl_local_rational_camera<T> &); \
  template std::istream & operator>>(std::istream &, vpgl_local_rational_camera<T> &);       \
  template vpgl_local_rational_camera<T> * read_local_rational_camera(std::string);          \
  template vpgl_local_rational_camera<T> * read_local_rational_camera(std::istream &);       \
  template vpgl_local_rational_camera<T> * read_local_rational_camera_from_txt(std::string); \
  template vpgl_local_rational_camera<T> * read_local_rational_camera_from_txt(std::istream &)

#endif // vpgl_local_rational_camera_hxx_
