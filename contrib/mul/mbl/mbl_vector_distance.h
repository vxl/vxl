#ifndef mbl_vector_distance_h_
#define mbl_vector_distance_h_
//:
// \file
// \brief  Functor object to compute distance between two vectors
// \author Tim Cootes

#include <vnl/vnl_vector.h>
#include <vcl_cmath.h>

//: Functor object to compute distance between two vectors
template<class T>
class mbl_vector_distance {
public:
  static double d(const vnl_vector<T>& v1,
                  const vnl_vector<T>& v2)
   { return vcl_sqrt(vnl_vector_ssd(v1,v2)); }
};

#endif // mbl_vector_distance_h_
