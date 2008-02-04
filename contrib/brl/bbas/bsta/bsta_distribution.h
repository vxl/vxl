// This is brl/bbas/bsta/bsta_distribution.h
#ifndef bsta_distribution_h_
#define bsta_distribution_h_
//:
// \file
// \brief A base class for probability distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vnl/vnl_vector_fixed.h>

//: A base class for probability distributions
template <class T, unsigned n>
class bsta_distribution
{
  public:
    //: The dimension of the distribution
    enum { dimension = n };
    //: The type used for calculations
    typedef T math_type;
    //: The type used for a n-dimensional vector of math types
    typedef vnl_vector_fixed<math_type,dimension> vector_type;
};


//: A base class for 1D probability distributions
//  Warning: this is partial specialization
template <class T>
class bsta_distribution<T,1>
{
  public:
    //: The dimension of the distribution
    enum { dimension = 1 };
    //: The type used for calculations
    typedef T math_type;
    //: The type used for a n-dimensional vector of math types
    typedef T vector_type;
};


#endif // bsta_distribution_h_
