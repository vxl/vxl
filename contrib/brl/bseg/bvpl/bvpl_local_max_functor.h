#ifndef bvpl_local_max_functor_h_
#define bvpl_local_max_functor_h_
//:
// \file
// \brief Functor to find the 2D edges with computing expected values
//
// \author Vishal Jain (vj@lems.brown.edu)
// \date June 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bvpl_kernel_iterator.h"
#include <bvxm/grid/bvxm_opinion.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>

template <class T>
class bvpl_local_max_functor
{
 public:
  //: Default constructor
  bvpl_local_max_functor();

  //: Destructor
  ~bvpl_local_max_functor() {}

  //: Apply a given operation to value val, depending on the dispatch character
  void apply(T& val, bvpl_kernel_dispatch& d);

  //: Returns the final operation of this functor
  T result();
 
  // Some additional functionalities 

  bool greater_than(T& val1, T& val2);
 
  static T min_response();

 private:
  
  T max_;  // max_value over neighborhood
  T cur_val_;  // max_value over neighborhood
  //: Initializes class variables
  void init();
};

// Default constructor
template <class T>
bvpl_local_max_functor<T>::bvpl_local_max_functor()
{
  this->init();
}

//:Initializes all local variables
template <class T>
void bvpl_local_max_functor<T>::init()
{
  max_=T(0);
}

template <class T>
void bvpl_local_max_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{
	if(d.c_==0)
		cur_val_=val;
	else
	{
	if( val>max_)
		max_=val;
	}
}

template <class T>
T bvpl_local_max_functor<T>::result()
{
  T result = cur_val_>=max_?cur_val_:0;

  //reset all variables
  init();

  return result;
}

template <class T>
bool bvpl_local_max_functor<T>::greater_than(T& val1, T& val2)
{
  return val1 > val2;
}

template <class T>
T bvpl_local_max_functor<T>::min_response()
{
  return T(0);
}


template <>
void bvpl_local_max_functor<bvxm_opinion>::init()
{
  max_=bvxm_opinion(1.0,0.0);
}

template <>
void bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::init()
{
  max_= bsta_gauss_f1(0.0f, 1.0f);
}


template <>
bvxm_opinion bvpl_local_max_functor<bvxm_opinion>::result()
{
  if (cur_val_>=max_)
  {
    bvxm_opinion result =    cur_val_;
    init();

    return result;
  }
  else
  {
    bvxm_opinion result =bvxm_opinion(1.0,0.0);
    init();

    return result;
  }
  //reset all variables
}

template <>
bool bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::greater_than(bsta_num_obs<bsta_gauss_f1>& g1, bsta_num_obs<bsta_gauss_f1>& g2)
{
  return vcl_abs(g1.mean()) > (g2.mean());
}

template <>
bsta_num_obs<bsta_gauss_f1> bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::min_response()
{
  return bsta_gauss_f1(0.0f, 1.0f);
}


#endif
