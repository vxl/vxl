//:
// \file
// \author Tim Cootes
// \brief Return 2 x 3 projection matrix based on viewing from angle (a,b)

#ifndef m23d_make_ortho_projection_h_
#define m23d_make_ortho_projection_h_

#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>

//: Return 2 x 3 projection matrix based on viewing from angle (Ax,Ay,Az)
//  If Ax=Ay=Az, then returns matrix (I|0)
vnl_matrix<double> m23d_make_ortho_projection(double Ax, double Ay, double Az);

//: Return projection matrix combination for ns shapes, nm modes
//  Matrix is 2*ns x 3*(1+nm) in size
//  The i,j-th sub matrix of size 2x3 corresponds to w_ij * P_i,
//  where P_i is a projection matrix for shape i, and w_ij is the
//  weight for the j-th shape basis.
//
// \param first_is_identity  When true, projection for first shape is scaled identity
// \param basis_true         When true, w_ij=(i==j) if i<=nm (ie first shapes define a basis)
vnl_matrix<double> m23d_make_ortho_projection(vnl_random& r,
                                              unsigned ns, unsigned nm=0,
                                              bool first_is_identity=false,
                                              bool basis_true=false);

#endif // m23d_make_ortho_projection_h_
