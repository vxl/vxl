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
// The fundamental matrix has the form (using Hartly and Zisserman convention for a-e as of Nov. 2018):
// \verbatim
// | 0  0  a |
// | 0  0  b |
// | c  d  e |
// \endverbatim

#include <vnl/vnl_fwd.h>
#include "vpgl_fundamental_matrix.h"
#include "vpgl_affine_camera.h"

template <class T>
class vpgl_affine_fundamental_matrix : public vpgl_fundamental_matrix<T>
{
 public:

  // Constructors:----------------------

  //: Default constructor creates dummy matrix.
  vpgl_affine_fundamental_matrix();

  //: Construct from a fundamental matrix in vnl form.
  vpgl_affine_fundamental_matrix( const vnl_matrix_fixed<T,3,3>& F );

  //: Cast up from a regular vpgl_fundamental_matrix.
  vpgl_affine_fundamental_matrix( const vpgl_fundamental_matrix<T>& fm );

  //: construct from two affine cameras (Ar, the camera with image points on the right of F, and Al on the left of F)
  vpgl_affine_fundamental_matrix( const vpgl_affine_camera<T>& Ar, const vpgl_affine_camera<T>& Al);

  // Getters and Setters:----------------

  //: Form the matrix from 3x3 vnl_fixed_matrix
  void set_matrix( const vnl_matrix_fixed<T,3,3>& F ) override;

  //: Form the matrix from its free parameters. (JLM changed to H&Z convention 11/12/2018)
  void set_from_params( T a, T b, T c, T d, T e );
};

#endif // vpgl_affine_fundamental_matrix_h_
