// This is brl/bbas/brdb/brdb_value.hxx
#ifndef brdb_value_hxx_
#define brdb_value_hxx_
//:
// \file
// \brief Templated code for brdb_value_t
// \author Matt Leotta
// \date January 25, 2005
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include "brdb_value.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/io/vbl_io_smart_ptr.h>

//: Test for equality under polymorphism
template< class T >
bool
brdb_value_t<T>::eq(const brdb_value& other) const
{
  assert(get_type_string() == other.is_a());
  return this->value_ == static_cast<const brdb_value_t<T>&>(other).value_;
}


//: Test for inequality (less than) under polymorphism
template< class T >
bool
brdb_value_t<T>::lt(const brdb_value& other) const
{
  assert(get_type_string() == other.is_a());
  return this->value_ < static_cast<const brdb_value_t<T>&>(other).value_;
}


//: Assign the value of /p other to this if the types are the same
template< class T >
bool
brdb_value_t<T>::assign(const brdb_value& other)
{
  const brdb_value_t<T>* my_type = dynamic_cast<const brdb_value_t<T>*>(&other);
  if (my_type){
    this->value_ = my_type->value_;
    return true;
  }
  return false;
}


//: binary io read value only
// handles only the value (without version or type info)
template< class T >
void
brdb_value_t<T>::b_read_value(vsl_b_istream& is)
{
  vsl_b_read(is, this->value_);
}


//: binary io write value only
// handles only the value (without version or type info)
template< class T >
void
brdb_value_t<T>::b_write_value(vsl_b_ostream& os) const
{
  vsl_b_write(os, this->value_);
}


//: Use this macro to instantiate brdb_value_t for each storage type
//If a template, a member template or the member of a class template is
//explicitly specialized then that specialization shall be declared before the
//first use of that specialization that would cause an implicit instantiation to
//take place, in every translation unit in which such a use occurs; no diagnostic
//is required.
//However MSVC rejects the declaration [temp.expl.spec]p6, [temp.expl.spec]p13


#define BRDB_VALUE_INSTANTIATE_LONG_FORM(T,FUNCSUFFIX,NAME) \
template <> const std::string& brdb_value_t<T >::get_type_string()\
{\
static std::string type_string = NAME;\
return type_string;\
}\
template class brdb_value_t<T >; \
const brdb_value::registrar reg_inst_##FUNCSUFFIX(new brdb_value_t<T >)

#define BRDB_VALUE_INSTANTIATE(T,NAME) BRDB_VALUE_INSTANTIATE_LONG_FORM(T,T,NAME)

#endif // brdb_value_hxx_
