// This is brl/bseg/bbgm/bbgm_viewer.cxx
//:
// \file

#include "bbgm_viewer.h"
#include "bbgm_apply.h"
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_basic_functors.h>
#include <vil/vil_math.h>


//: Probe to see if this viewer can handle this image type
bool
bbgm_mean_viewer::probe(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())) return true;\
  }\
  {\
    typedef DIST T;\
    if(dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())) return true;\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return false;
}


//: Return the maximum number of components if a mixture
// If not a mixture return 1
unsigned int
bbgm_mean_viewer::max_components(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return 0;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())){\
      bsta_mixture_size_functor<T> func;\
      vil_image_view<unsigned int> count;\
      unsigned int fail_val = 0;\
      bbgm_apply(*d, func, count, &fail_val);\
      unsigned int min, max;\
      vil_math_value_range(count, min, max);\
      return max;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro


  return 1;
}


//: Apply the operations to the distribution image to produce a vil image
bool bbgm_mean_viewer::apply(const bbgm_image_sptr& dimg,
                              vil_image_view<double>& image) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      typedef bsta_mean_functor<DIST > func_T;\
      func_T m_func;\
      bsta_mixture_functor<T, func_T > func(m_func,this->active_component_);\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }\
  {\
    typedef DIST T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      bsta_mean_functor<T> func;\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return false;
}

//=============================================================================


//: Probe to see if this viewer can handle this image type
bool
bbgm_variance_viewer::probe(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())) return true;\
  }\
  {\
    typedef DIST T;\
    if(dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())) return true;\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return false;
}


//: Return the maximum number of components if a mixture
// If not a mixture return 1
unsigned int
bbgm_variance_viewer::max_components(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return 0;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())){\
      bsta_mixture_size_functor<T> func;\
      vil_image_view<unsigned int> count;\
      unsigned int fail_val = 0;\
      bbgm_apply(*d, func, count, &fail_val);\
      unsigned int min, max;\
      vil_math_value_range(count, min, max);\
      return max;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return 1;
}


//: Apply the operations to the distribution image to produce a vil image
bool bbgm_variance_viewer::apply(const bbgm_image_sptr& dimg,
                                  vil_image_view<double>& image) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      typedef bsta_diag_covar_functor<DIST > func_T;\
      func_T m_func;\
      bsta_mixture_functor<T, func_T > func(m_func,this->active_component_);\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }\
  {\
    typedef DIST T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      bsta_diag_covar_functor<T> func;\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      typedef bsta_var_functor<DIST > func_T;\
      func_T m_func;\
      bsta_mixture_functor<T, func_T > func(m_func,this->active_component_);\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }\
  {\
    typedef DIST T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      bsta_var_functor<T> func;\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
#undef macro

  return false;
}

//=============================================================================


//: Probe to see if this viewer can handle this image type
bool
bbgm_weight_viewer::probe(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())) return true;\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return false;
}


//: Return the maximum number of components if a mixture
// If not a mixture return 1
unsigned int
bbgm_weight_viewer::max_components(const bbgm_image_sptr& dimg) const
{
  if(!dimg)
    return 0;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr())){\
      bsta_mixture_size_functor<T> func;\
      vil_image_view<unsigned int> count;\
      unsigned int fail_val = 0;\
      bbgm_apply(*d, func, count, &fail_val);\
      unsigned int min, max;\
      vil_math_value_range(count, min, max);\
      return max;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return 1;
}


//: Apply the operations to the distribution image to produce a vil image
bool bbgm_weight_viewer::apply(const bbgm_image_sptr& dimg,
                                vil_image_view<double>& image) const
{
  if(!dimg)
    return false;

#define macro(DIST)\
  {\
    typedef bsta_num_obs<bsta_mixture<DIST > > T;\
    if(const bbgm_image_of<T>* d = dynamic_cast<const bbgm_image_of<T>*>(dimg.ptr()))\
    {\
      bsta_weight_functor<T > func(this->active_component_);\
      bbgm_apply(*d, func, image, fail_val_);\
      return true;\
    }\
  }
  macro(bsta_num_obs<bsta_gauss_f1>);
  macro(bsta_num_obs<bsta_gauss_if3>);
#undef macro

  return false;
}
