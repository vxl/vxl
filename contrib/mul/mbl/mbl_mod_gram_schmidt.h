// This is mul/mbl/mbl_mod_gram_schmidt.h
#ifndef mbl_mod_gram_schmidt_h_
#define mbl_mod_gram_schmidt_h_
//:
// \file
// \brief Orthogonalise a basis using modified Gram-Schmidt (and normalise)
// \author Martin Roberts
//
// Note: Modified Gram-Schmidt is more numerically stable than the classical version
// The partially constructed transformed jth vector is used in the successive projections rather than the untransformed
// \verbatim
// Modifications
// Mar. 2011 - Patrick Sauer - added variant that returns normalisation multipliers
// \endverbatim

#include <vnl/vnl_matrix.h>

//=======================================================================
//: Orthogonalise a basis using modified Gram-Schmidt
// Transform basis {vk} to orthonormal basis {ek} with k in range 1..N
// \code
// for j = 1 to N
//     ej = vj
//     for k = 1 to j-1
//         ej = ej - <ej,ek>ek   //NB Classical GS has vj in inner product
//     end
//     ej = ej/|ej|
//  end
// \endcode

//: Convert input basis {v} to orthonormal basis {e}
// Each basis vector is a column of v, and likewise the orthonormal bases are returned as columns of e
void mbl_mod_gram_schmidt(const vnl_matrix<double>& v,
                          vnl_matrix<double>& e);

//: Convert input basis {v} to orthonormal basis {e}
// Each basis vector is a column of v, and likewise the orthonormal bases are returned as columns of e
// The multipliers used to normalise each vector in {e} are returned in n.
void mbl_mod_gram_schmidt( const vnl_matrix<double>& v,
                           vnl_matrix<double>& e, vnl_vector<double>& n );

#endif // mbl_mod_gram_schmidt_h_
