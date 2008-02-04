// This is brl/bbas/brdb/brdb_value.txx
#ifndef brdb_value_txx_
#define brdb_value_txx_
//:
// \file
// \brief Templated code for brdb_value_t
// \author Matt Leotta
// \date 1/25/05
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim


#include "brdb_value.h"
#include <vcl_cassert.h>


//: Test for equality under polymorphism
template< class T >
bool
brdb_value_t<T>::eq(const brdb_value& other) const
{
  assert(type_string_ == other.is_a());
  return this->value_ == static_cast<const brdb_value_t<T>&>(other).value_;
}


//: Test for inequality (less than) under polymorphism
template< class T >
bool
brdb_value_t<T>::lt(const brdb_value& other) const
{
  assert(type_string_ == other.is_a());
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
#define BRDB_VALUE_INSTANTIATE(T,NAME) \
template class brdb_value_t<T >; \
template <> const vcl_string brdb_value_t<T >::type_string_(NAME); \
const brdb_value::registrar reg_inst_##T(new brdb_value_t<T >)

#endif // brdb_value_txx_
