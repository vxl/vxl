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

#include <vgl/vgl_fwd.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
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
  vpgl_local_rational_camera(vpgl_lvcs const& lvcs,
                             vpgl_rational_camera<T> const& rcam);

  //: Constructor from a rational camera and a geographic origin
  vpgl_local_rational_camera(T longitude, T latitude, T elevation,
                             vpgl_rational_camera<T> const& rcam);


  virtual ~vpgl_local_rational_camera() {}

  virtual vcl_string type_name() const { return "vpgl_local_rational_camera"; }

  //: Clone `this': creation of a new object and initialization
  //  See Prototype pattern
  virtual vpgl_local_rational_camera<T>* clone(void) const;

  //: Equality test
  inline bool operator==(vpgl_local_rational_camera<T> const &that) const
  { return this == &that ||
           ( static_cast<vpgl_rational_camera<T> const&>(*this) ==
             static_cast<vpgl_rational_camera<T> const&>(that)      &&
             this->lvcs() == that.lvcs() );
  }
// Mutators/Accessors

//: set the local vertical coordinate system
void set_lvcs(vpgl_lvcs const& lvcs) {lvcs_ = lvcs;}

vpgl_lvcs lvcs() const {return lvcs_;}

//: The generic camera interface. u represents image column, v image row.
virtual void project(const T x, const T y, const T z, T& u, T& v) const;

// Interface for vnl

//: Project a world point onto the image
virtual vnl_vector_fixed<T, 2> project(vnl_vector_fixed<T, 3> const& world_point) const;

// Interface for vgl

//: Project a world point onto the image
virtual vgl_point_2d<T> project(vgl_point_3d<T> world_point) const;


//: print the camera parameters
virtual void print(vcl_ostream& s = vcl_cout) const;

//: save to file (the lvcs is after the global rational camera parameters)
virtual bool save(vcl_string cam_path);


protected:
vpgl_lvcs lvcs_;
};

//: Creates a local rational camera from a file
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T>* read_local_rational_camera(vcl_string cam_path);

//: Creates a local rational camera from a file
// \relatesalso vpgl_local_rational_camera
template <class T>
vpgl_local_rational_camera<T>* read_local_rational_camera(vcl_istream& istr);

//: Write to stream
// \relatesalso vpgl_local_rational_camera
template <class T>
vcl_ostream& operator<<(vcl_ostream& s, const vpgl_local_rational_camera<T>& p);

//: Read from stream
// \relatesalso vpgl_local_rational_camera
template <class T>
vcl_istream& operator>>(vcl_istream& is, vpgl_local_rational_camera<T>& p);

#define VPGL_LOCAL_RATIONAL_CAMERA_INSTANTIATE(T) extern "please include vgl/vpgl_local_rational_camera.txx first"


#endif // vpgl_local_rational_camera_h_

