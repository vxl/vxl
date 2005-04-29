#ifndef random_transform_set_2d_h_
#define random_transform_set_2d_h_
//:
// \file
// \author J.L. Mundy
// \brief A class for randomly generating 2d transformations
//
// \verbatim
//  Modifications<none>
// \endverbatim
#include <vcl_vector.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

template<class T>
class bugl_random_transform_set_2d 
{
 public:
  ~bugl_random_transform_set_2d(){}

  //: generate a set of uniformly distributed equiform transformations
 static void equiform_uniform(const unsigned n_trans,
                              vcl_vector<vgl_h_matrix_2d<T> >& transform_set,
                              const T dx, const T dy,
                              const T dtheta = (T)0, const T ds = (T)0);

  

 protected:
 bugl_random_transform_set_2d(){} //static methods only
};

#define BUGL_RANDOM_TRANSFORM_SET_2D_INSTANTIATE(T) extern "please include bugl/random_transform_set_2d.txx first"

#endif
