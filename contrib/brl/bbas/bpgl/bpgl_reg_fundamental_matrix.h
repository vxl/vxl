// This is bbas/bpgl/bpgl_reg_fundamental_matrix.h
#ifndef bpgl_reg_fundamental_matrix_h_
#define bpgl_reg_fundamental_matrix_h_
//:
// \file
// \brief A class for the fundamental matrix between two affine cameras registered to a ground plane.
// \author Thomas Pollard
// \date June 8, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
// The fundamental matrix for two registered images has a simplified form and can be
// written as:
// \verbatim
// |  0  0  a |
// |  0  0 -b |
// | -a  b  0 |
// \endverbatim

#include <vgl/vgl_fwd.h>
#include <vpgl/vpgl_fundamental_matrix.h>

template <class T>
class bpgl_reg_fundamental_matrix : public vpgl_fundamental_matrix<T>
{
 public:
  // Constructors:----------------------

  //: Default constructor creates dummy matrix.
  bpgl_reg_fundamental_matrix();

  //: Main constructor takes corresponding points from right and left images.
  bpgl_reg_fundamental_matrix( const vgl_point_2d<T>& pr, const vgl_point_2d<T>& pl );

  //: Cast up from a regular bpgl_fundamental_matrix.
  bpgl_reg_fundamental_matrix( const vpgl_fundamental_matrix<T>& fm );

  // Getters and Setters:----------------

  //: Form the matrix from corresponding points from right and left images.
  bool set_from_points( const vgl_point_2d<T>& pr, const vgl_point_2d<T>& pl );

  //: Form the matrix from its free parameters.
  void set_from_params( T a, T b );
};

#endif // bpgl_reg_fundamental_matrix_h_
