// This is gel/mrc/vpgl/vpgl_fundamental_matrix.h
#ifndef vpgl_fundamental_matrix_h_
#define vpgl_fundamental_matrix_h_
//:
// \file
// \brief A class for the fundamental matrix between two projective cameras.
// \author Thomas Pollard
// \date 01/28/05
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
//  A class which holds the fundamental matrix and performs basic computations with it.
//  More advanced functions using the fundamental matrix can be found in
//  "vpgl_fundamental_matrix_functions.h".
//
//  This implementation forces the rank of the fundamental matrix to be rank 2, and if
//  the matrix is set with a rank 3 matrix, it will be reduced in rank using SVD
//  decomposition.


#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_proj_camera.h"


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
                            const vpgl_proj_camera<T>& cl ) : cached_svd_(NULL)
  { set_matrix( cr, cl ); }

  //: Construct from a fundamental matrix in vnl form.
  vpgl_fundamental_matrix( const vnl_matrix_fixed<T,3,3>& F ) : cached_svd_(NULL)
  { set_matrix( F ); }

  //: Copy Constructor
  vpgl_fundamental_matrix(const vpgl_fundamental_matrix<T>& other);

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

  //: Gives the left camera matrix corresponding to the fundamental matrix, when the right camera matrix is assumed to be identity.
  // The variables v, lambda are free parameters as described in H&Z 2nd ed pg 256.
  vpgl_proj_camera<T> extract_left_camera(
    const vnl_vector_fixed<T,3>& v, T lambda ) const;

  //: Alternative left camera extractor.
  // Takes corresponding lists of image points with their world locations
  // to determine the correct camera.  Must give at least 2 pairs of correspondences.
  // This is not a robust algorithm but this shouldn't be a problem
  // as these correspondences will usually be picked by hand.
  vpgl_proj_camera<T> extract_left_camera(
    const vcl_vector< vgl_point_3d<T> >& world_points,
    const vcl_vector< vgl_point_2d<T> >& image_points ) const;

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

//:vpgl_fundamental_matrix stream I/O

template <class T>
vcl_ostream&  operator<<(vcl_ostream& s, vpgl_fundamental_matrix<T> const& p);

//: Read vpgl_perspective_camera  from stream
template <class T>
vcl_istream&  operator>>(vcl_istream& s, vpgl_fundamental_matrix<T>& p);

#endif // vpgl_fundamental_matrix_h_
