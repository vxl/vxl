// This is core/vpgl/vpgl_fundamental_matrix.h
#ifndef vpgl_fundamental_matrix_h_
#define vpgl_fundamental_matrix_h_
//:
// \file
// \brief A class for the fundamental matrix between two projective cameras.
// \author Thomas Pollard
// \date January 28, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
//  A class which holds the fundamental matrix and performs basic computations with it.
//  More advanced functions using the fundamental matrix can be found in
//  "vpgl_fundamental_matrix_functions.h".
//
//  This implementation forces the rank of the fundamental matrix to be rank 2, and if
//  the matrix is set with a rank 3 matrix, it will be reduced in rank using SVD
//  decomposition.
//
//  The notation "left" and "right" refers to camera producing points used on
//  the left side of the F matrix and vice versa.
//
// \verbatim
//  Modifications
//   May 06, 2009  Ricardo Fabbri   Overloaded {l,r}_epipolar_line to take line as input
//   May 10, 2010 Andrew Hoelscher - Added a constructor based off two
//      calibration matrices and an essential matrix.
// \endverbatim

#include <iosfwd>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "vpgl_proj_camera.h"

template <class T> class vpgl_essential_matrix;
template <class T> class vpgl_calibration_matrix;

template <class T>
class vpgl_fundamental_matrix
{
 public:
  // Constructors:----------------------

  //: Default constructor creates dummy rank 2 matrix.
  vpgl_fundamental_matrix();

  //: Main constructor takes two projective cameras.
  //  The RHS of the fundamental matrix will correspond to cr and the LHS to cl.
  vpgl_fundamental_matrix( const vpgl_proj_camera<T>& cr,
                           const vpgl_proj_camera<T>& cl ) : cached_svd_(nullptr)
  { set_matrix( cr, cl ); }

  //: Construct from a fundamental matrix in vnl form.
  vpgl_fundamental_matrix( const vnl_matrix_fixed<T,3,3>& F ) : cached_svd_(nullptr)
  { set_matrix( F ); }

  //: Copy Constructor
  vpgl_fundamental_matrix(const vpgl_fundamental_matrix<T>& other);

  //: Construct from an essential matrix and two calibration matrices.
  // Since E = Kl^T * F * Kr, then F = Kl^-T * E * Kr^-1.
  // WARNING! This constructor uses two 3x3 inverse calculations, so it is expensive.
  vpgl_fundamental_matrix(const vpgl_calibration_matrix<T> &kr,
                          const vpgl_calibration_matrix<T> &kl,
                          const vpgl_essential_matrix<T> &em);
  //: Assignment
  const vpgl_fundamental_matrix<T>& operator=( const vpgl_fundamental_matrix<T>& fm );

  //: Destructor
  virtual ~vpgl_fundamental_matrix();

  // Basic Operations:-------------------

  //: Put the coordinates of the epipoles in er, el.
  void get_epipoles( vgl_homg_point_2d<T>& er, vgl_homg_point_2d<T>& el ) const;

  //: Given a point in one image, find the corresponding epipolar line in the other image.
  vgl_homg_line_2d<T> r_epipolar_line( const vgl_homg_point_2d<T>& pl ) const;
  vgl_homg_line_2d<T> l_epipolar_line( const vgl_homg_point_2d<T>& pr ) const;

  //: Given an epipolar line in one image, find the corresponding epipolar line in the other image.
  // H&Z 2nd ed p. 247
  vgl_homg_line_2d<T> r_epipolar_line(const vgl_homg_line_2d<T> &epiline_l) const;
  vgl_homg_line_2d<T> l_epipolar_line(const vgl_homg_line_2d<T> &epiline_r) const;

  //: Gives the left camera matrix corresponding to the fundamental matrix
  // The right camera matrix is assumed to be identity.
  // The variables v, lambda are free parameters as described in H&Z 2nd ed p. 256.
  vpgl_proj_camera<T> extract_left_camera(
    const vnl_vector_fixed<T,3>& v, T lambda ) const;

  //: Alternative left camera extractor.
  // Takes corresponding lists of image points with their world locations
  // to determine the correct camera.  Must give at least 2 pairs of correspondences.
  // This is not a robust algorithm but this shouldn't be a problem
  // as these correspondences will usually be picked by hand.
  vpgl_proj_camera<T> extract_left_camera(
    const std::vector< vgl_point_3d<T> >& world_points,
    const std::vector< vgl_point_2d<T> >& image_points ) const;

  // Getters and Setters:----------------

  //: Get a copy of the FM in vnl form.
  const vnl_matrix_fixed<T,3,3>& get_matrix() const { return F_; }

  //: Get a copy of the svd of the fundamental matrix.
  // The svd is computed when the matrix is first set, so this just accesses a cached version.
  const vnl_svd<T>& svd() const{ return *cached_svd_; }

  void set_matrix( const vpgl_proj_camera<T>& cr,
                   const vpgl_proj_camera<T>& cl );

  void set_matrix( const vnl_matrix_fixed<T,3,3>& F );

 protected:
  //: Internal representation of the fundamental matrix.
  vnl_matrix_fixed<T,3,3> F_;

  //: Cached copy of the svd.
  mutable vnl_svd<T>* cached_svd_;
};

//: Write vpgl_fundamental_matrix to stream
template <class T>
std::ostream&  operator<<(std::ostream& s, vpgl_fundamental_matrix<T> const& p);

//: Read vpgl_fundamental_matrix from stream
template <class T>
std::istream&  operator>>(std::istream& s, vpgl_fundamental_matrix<T>& p);

#endif // vpgl_fundamental_matrix_h_
