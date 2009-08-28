// This is brl/bseg/bvpl/bvpl_local_max_functor.h

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
  T result(T cur_val);

  // Some additional functionalities

  bool greater_than(T& val1, T& val2);

  static T min_response();

  float filter_response(unsigned id, unsigned target_id, T curr_val);

 private:
  //: max_value over neighborhood
  T max_;
  //: cur_value over neighborhood
  T cur_val_;
  //: Initializes all local class variables
  void init() { max_=T(0); }
};

// Default constructor
template <class T>
bvpl_local_max_functor<T>::bvpl_local_max_functor()
{
  this->init();
}

template <class T>
void bvpl_local_max_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{

	if( val>max_)
		max_=val;
}

template <class T>
T bvpl_local_max_functor<T>::result( T cur_val)
{
  T result = cur_val>=max_?cur_val:0;

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

template <class T>
float bvpl_local_max_functor<T>::filter_response(unsigned id, unsigned target_id, T curr_val)
{
  if (id !=target_id)
    return 0.0f;

  return (float)curr_val;
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
void bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::apply(bsta_num_obs<bsta_gauss_f1>& val, bvpl_kernel_dispatch& d)
{
  if (d.c_==0)
    cur_val_=val;
  else
  {
    if ( vcl_abs(val.mean()) > vcl_abs(max_.mean()) )
      max_=val;
  }
}

template <>
bvxm_opinion bvpl_local_max_functor<bvxm_opinion>::result(bvxm_opinion cur_val)
{
  if (cur_val>max_)
  {

    bvxm_opinion result =    cur_val;

    init();
    return result;
  }
  else
  {
    bvxm_opinion result = bvxm_opinion(1.0,0.0);
    init();
    return result;
  }
}

template <>
bsta_num_obs<bsta_gauss_f1> bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::result(bsta_num_obs<bsta_gauss_f1> cur_val)
{
  bsta_num_obs<bsta_gauss_f1> result;

  
  if( (vcl_abs(cur_val.mean()))>=(vcl_abs(max_.mean() - 1e-5)))
    result = cur_val;

  result =  bsta_gauss_f1(0.0f, 1.0f);
  //reset all variables
  init();

  return result;
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

template <>
float bvpl_local_max_functor<bsta_num_obs<bsta_gauss_f1> >::filter_response(unsigned id, unsigned target_id, bsta_num_obs<bsta_gauss_f1> curr_val)
{
  if (id !=target_id)
    return 0.0f;

  return vcl_abs(curr_val.mean());
}

#endif
