//:
// \file
// \author Tim Cootes
// \brief Extracts scaled rotation/projection component of 2x3 projection matrix P

#ifndef m23d_scaled_ortho_projection_h_
#define m23d_scaled_ortho_projection_h_

#include <vnl/vnl_matrix.h>

//: Extracts scaled rotation/projection component of 2x3 projection matrix P
// Resulting 2x3 is of the form s*(I|0)*R
vnl_matrix<double> m23d_scaled_ortho_projection(const vnl_matrix<double>& P);


#endif // m23d_scaled_ortho_projection_h_
