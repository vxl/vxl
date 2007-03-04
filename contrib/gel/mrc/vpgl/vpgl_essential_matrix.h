// This is gel/mrc/vpgl/vpgl_essential_matrix.h
#ifndef vpgl_essential_matrix_h_
#define vpgl_essential_matrix_h_
//:
// \file
// \brief A class for the essential matrix between two projective cameras.
// \author Joseph Mundy
// \date 02/28/07
//
//
//  The essential matrix is related to the fundamental matrix by
//       E = [K_left]^t [F] [K_right]
// The notation "left" and "right" refers to camera producing points used 
// on the left side of the F or E matrix and vice versa.
// Note: the extract left camera function requires points be transformed to
// the camera focal plane. That is, e.g.,
// left_coor(in camera focal plane coordinates) =
//    [K_left]^-1 p_left(in image coordinates). 
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_perspective_camera.h"
#include "vpgl_calibration_matrix.h"
#include "vpgl_fundamental_matrix.h"



template <class T>
class vpgl_essential_matrix : public vpgl_fundamental_matrix<T>
{
 public:
  // Constructors:----------------------

  //: Default constructor creates dummy rank 2 matrix.
  vpgl_essential_matrix();

  //: construct from a fundamental matrix and left and right calibration matrices
  vpgl_essential_matrix( const vpgl_fundamental_matrix<T>& F,
                         const vpgl_calibration_matrix<T>& K_left,
                         const vpgl_calibration_matrix<T>& K_right);


  //: Main constructor takes two perspective cameras.
  //  The RHS of the essential matrix will correspond to cr and the LHS to cl.
  vpgl_essential_matrix( const vpgl_perspective_camera<T>& cr,
                         const vpgl_perspective_camera<T>& cl );

  //: Construct from a essential matrix in vnl form.
  vpgl_essential_matrix( const vnl_matrix_fixed<T,3,3>& E ) : vpgl_fundamental_matrix(E){}


  //: Copy Constructor
  vpgl_essential_matrix(const vpgl_essential_matrix<T>& other);

  //: Assignment
  const vpgl_essential_matrix<T>& operator=( const vpgl_essential_matrix<T>& em );


  //: Destructor
  virtual ~vpgl_essential_matrix();
};

//Public functions
//: Left camera extractor. Normalized correspondence pair is needed to determine which of four solutions should be used, i.e. focal plane coordinates
// The translation part of the camera is normalized to the indicated magnitude.
template <class T>
bool extract_left_camera(const vpgl_essential_matrix<T>& E,
                         const vgl_point_2d<T>& left_corr,//focal plane coordinates
                         const vgl_point_2d<T>& right_corr,//focal plane coordinates
                         vpgl_perspective_camera<T>& p_left,
                         const T translation_mag = (T)1);

//:vpgl_essential_matrix stream I/O

template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vpgl_essential_matrix<T> const& p);

//: Read vpgl_perspective_camera  from stream
template <class T>
vcl_istream&  operator>>(vcl_istream& s, vpgl_essential_matrix<T>& p);

#endif // vpgl_essential_matrix_h_
