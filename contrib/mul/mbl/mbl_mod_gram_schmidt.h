// This is mul/mbl/mbl_mod_gram_schmidt.h
#ifndef mbl_mod_gram_schmidt_h_
#define mbl_mod_gram_schmidt_h_
//:
// \file
// \brief Orthoganalise a basis using modified Gram-Schmidt (and normalise)
// \author Martin Roberts

//Note Modified Gram-Schmidt is more numericdally stable than the classical version
//The partially constructed transformed jth vector is used in the successive projections rather than the untransformed

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//=======================================================================
//: Orthoganalise a basis using modified Gram-Schmidt
// Transform basis {vk} to orthonormal basis {ek} with k in range 1..N
// for j = 1 to N
//     ej = vj
//     for k = 1 to j-1
//         ej = ej - <ej,ek>ek   //NB Classical GS has vj in inner product
//     end
//     ej = ej/|ej|
//  end

//: Convert input basis {v} to orthonormal basis {e}
// Each basis vector is a column of v, and likewise the orthonormal bases are returned as columns of e
void mbl_mod_gram_schmidt(const vnl_matrix<double>& v,
                          vnl_matrix<double>& e);
#endif


