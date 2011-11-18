// This is core/vpgl/vpgl_affine_fundamental_matrix.h
#ifndef vpgl_affine_fundamental_matrix_h_
#define vpgl_affine_fundamental_matrix_h_
//:
// \file
// \brief A class for the fundamental matrix between two affine cameras..
// \author Thomas Pollard
// \date June 8, 2005
// \author Joseph Mundy, Matt Leotta, Vishal Jain
//
// The fundamental matrix has the form:
// \verbatim
// | 0  0  e |
// | 0  0  d |
// | a  b  c |
// \endverbatim

#include <vnl/vnl_fwd.h>
#include "vpgl_fundamental_matrix.h"

template <class T>
class vpgl_affine_fundamental_matrix : public vpgl_fundamental_matrix<T>
{
 public:

  // Constructors:----------------------

  //: Default constructor creates dummy matrix.
  vpgl_affine_fundamental_matrix();

  //: Cast up from a regular vpgl_fundamental_matrix.
  vpgl_affine_fundamental_matrix( const vpgl_fundamental_matrix<T>& fm );

  // Getters and Setters:----------------

  //: Form the matrix from its free parameters.
  void set_from_params( T a, T b, T c, T d, T e );
};

#endif // vpgl_affine_fundamental_matrix_h_
