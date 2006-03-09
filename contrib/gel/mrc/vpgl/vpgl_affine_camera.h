// This is gel/mrc/vpgl/vpgl_affine_camera.h
#ifndef vpgl_affine_camera_h_
#define vpgl_affine_camera_h_

//:
// \file
// \brief A class for the affine camera model.
// \author Thomas Pollard
// \date 01/28/05
// \author Joseph Mundy, Matt Leotta, Vishal Jain


#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include "vpgl_proj_camera.h"


template <class T>
class vpgl_affine_camera : public vpgl_proj_camera<T>
{

public:

  //: Default constructor creates the canonical affine camera.
  vpgl_affine_camera();

  //: Construct from the first two rows.
  vpgl_affine_camera( const vnl_vector_fixed<T,4>& row1,
                       const vnl_vector_fixed<T,4>& row2 );

  //: Construct from a 3x4 matrix, sets the last row to 0001.
  // The bottom right entry had better not be 0.
  vpgl_affine_camera( const vnl_matrix_fixed<T,3,4>& camera_matrix );

  //: Set the top two rows.
  void set_rows( const vnl_vector_fixed<T,4>& row1,
                 const vnl_vector_fixed<T,4>& row2 );


};

#endif // vpgl_affine_camera_h_
