// This is core/vnl/vnl_beta.h
#ifndef vnl_beta_h_
#define vnl_beta_h_

//:
//  \file
//  \brief The implementation of beta function, also called the Euler integral 
//         of the first kind
//  \author Gamze Tunali

#include "vnl_gamma.h" 

//: Computation of beta function in terms of gamma
template <class T>
inline double vnl_beta(T x, T y) {return (vnl_gamma(x)*vnl_gamma(y))/vnl_gamma(x+y); }


#endif