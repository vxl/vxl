// This is core/vpgl/vpgl_local_rational_camera.h
#ifndef vpgl_local_rational_camera_h_
#define vpgl_local_rational_camera_h_
//:
// \file
// \brief A local rational camera model
// \author Joseph Mundy
// \date February 16, 2008
//
// Rational camera models are defined with respect to global geographic
// coordinates. In many applications it is necessary to project points with
// local 3-d Cartesian coordinates. This camera class incorporates a
// Local Vertical Coordinate System (LVCS) to convert local coordinates
// to geographic coordinates to input to the native geographic RPC model.

#include <iostream>
#include <string>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vpgl_rational_camera.h"
#include "vpgl_lvcs.h"
//
//--------------------=== composite rational camera ===---------------------------
//
template <class T>
class vpgl_local_rational_camera : public vpgl_rational_camera<T>
{
public:
  //: default constructor
  vpgl_local_rational_camera();

  //: Constructor from a rational camera and a lvcs
  vpgl_local_rational_camera(const vpgl_lvcs & lvcs, const vpgl_rational_camera<T> & rcam);

  //: Constructor from a rational camera and a geographic origin
  vpgl_local_rational_camera(T longitude, T latitude, T elevation, const vpgl_rational_camera<T> & rcam);

  ~vpgl_local_rational_camera() override = default;

  std::string
  type_name() const override
  {
    return "vpgl_local_rational_camera";
  }

  //: Clone `this': creation of a new object and initialization
  // legal C++ because the return type is covariant with vpgl_camera<T>*
  vpgl_local_rational_camera<T> *
  clone() const override;

  //: Equality test
  inline bool
  operator==(const vpgl_local_rational_camera<T> & that) const
  {
    return this == &that ||
           (static_cast<const vpgl_rational_camera<T> &>(*this) == static_cast<const vpgl_rational_camera<T> &>(that) &&
            this->lvcs() == that.lvcs());
  }

  //: set the local vertical coordinate system
  void
  set_lvcs(const vpgl_lvcs & lvcs)
  {
    lvcs_ = lvcs;
  }
  void
  set_lvcs(const double & lon, const double & lat, const double & elev);

  //: get the local vertical coordinate system
  vpgl_lvcs
  lvcs() const
  {
    return lvcs_;
  }

  //: project 3D->2D, x,y,z are relative to the lvcs
  using vpgl_rational_camera<T>::project; // "project" overload from base class
  void
  project(const T x, const T y, const T z, T & u, T & v) const override;

  // write PVL (paramter value language) to output stream
  void
  write_pvl(std::ostream & s, vpgl_rational_order output_order) const override;

  //: read from PVL (parameter value language) file/stream
  using vpgl_rational_camera<T>::read_pvl; // "read_pvl" overload from base class
  bool
  read_pvl(std::istream & istr) override;

  //: read from TXT file/stream
  using vpgl_rational_camera<T>::read_txt; // "read_txt" overload from base class
  bool
  read_txt(std::istream & istr) override;

protected:
  // members
  vpgl_lvcs lvcs_;
};

//: Write to stream
// \relatesalso vpgl_local_rational_camera
template <class T>
std::ostream &
operator<<(std::ostream & s, const vpgl_local_rational_camera<T> & p);

//: Read from stream
// \relatesalso vpgl_local_rational_camera
template <class T>
std::istream &
operator>>(std::istream & is, vpgl_local_rational_camera<T> & p);

//: Creates a local rational camera from a PVL file
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera(std::string cam_path);

//: Creates a local rational camera from a PVL input stream
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera(std::istream & istr);

//: Creates a local rational camera from a TXT file
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera_from_txt(std::string cam_path);

//: Creates a local rational camera from a TXT input stream
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T> *
read_local_rational_camera_from_txt(std::istream & istr);


#define VPGL_LOCAL_RATIONAL_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_local_rational_camera.hxx first"


#endif // vpgl_local_rational_camera_h_
