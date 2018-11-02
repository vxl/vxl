// This is brl/bseg/bbgm/bbgm_planes_to_sample.h
#ifndef bbgm_planes_to_sample_h_
#define bbgm_planes_to_sample_h_
//:
// \file
// \brief Templated Helper function extracting a data sample from image planes
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 13, 2006
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T1, class T2, unsigned n>
struct bbgm_planes_to_sample
{
  static void apply(const T1* data, T2& sample, std::ptrdiff_t step){
    typename T2::iterator s_itr = sample.begin();
    for (unsigned int p=0; p<n; ++p, data += step, ++s_itr){
      *s_itr = *data;
    }
  }
};

template <class T1, class T2>
struct bbgm_planes_to_sample<T1,T2,3>
{
  static void apply(const T1* data, T2& sample, std::ptrdiff_t step){
  typename T2::iterator s_itr = sample.begin();
  *s_itr = *data;
  *(++s_itr) = *(data+=step);
  *(++s_itr) = *(data+=step);
  }
};

template <class T1, class T2>
struct bbgm_planes_to_sample<T1,T2,1>
{
  static void apply(const T1* data, T2& sample, std::ptrdiff_t /*step*/){
    sample = *data;
  }
};


#endif // bbgm_planes_to_sample_h_
