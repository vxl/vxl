// This is brl/bpro/bprb/bprb_parameters.hxx
#ifndef bprb_parameters_hxx_
#define bprb_parameters_hxx_
//:
// \file
// \brief Templated code for bprb parameters.
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date July 2, 2004
// \verbatim
//  Modifications
//   Jan 30, 2008  J.L. Mundy - ported
// \endverbatim

#include "bprb_parameters.h"
//
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Set the current value to \p val
template<class T>
bool
bprb_param_type<T>::set_value( const T& val )
{
  if ( has_bounds_ && (val < min_value_ || max_value_ < val) )
    return false;

  value_ = val;
  return true;
}


//: Create a string representation of the value
template<class T>
std::string
bprb_param_type<T>::create_string(const T& val) const
{
  std::stringstream stm;
  stm << val;
  return stm.str();
}


//: Parse a string representation of the value
template<class T>
T
bprb_param_type<T>::parse_string(const std::string& input) const
{
  T val;
  std::istringstream strm(input);
  strm >> val;
  return val;
}

//==============================================================================

//: Use this macro to instantiate bprb_parameters for each parameter type
#define BPRB_PARAMETERS_INSTANTIATE(T) \
template class bprb_param_type< T >

#endif // bprb_parameters_hxx_
