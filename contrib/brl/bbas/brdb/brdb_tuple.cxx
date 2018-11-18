// This is brl/bbas/brdb/brdb_tuple.cxx
#include <iostream>
#include "brdb_tuple.h"
//:
// \file
//
// updated by Yong Zhao
// Apr 4th, 2007
// make it work with the database initially

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//======================= Constructors / Destructors ========================


//: Constructor using a vector of db value references
brdb_tuple::brdb_tuple(const std::vector<brdb_value*>& values)
: values_(values.size(),nullptr)
{
  for (unsigned int i=0; i<values.size(); ++i){
    values_[i] = values[i]->clone();
  }
}


//: Prototype factory using a vector type name
brdb_tuple_sptr
brdb_tuple::make_prototype(const std::vector<std::string>& types)
{
  typedef std::map<std::string, const brdb_value*> reg_t;
  const reg_t& reg =  brdb_value::registry();

  std::vector<brdb_value* > values(types.size(),nullptr);
  for (unsigned int i=0; i<types.size(); ++i){
    auto f = reg.find(types[i]);
    if (f != reg.end())
      values[i] = f->second->clone();
    else{
      std::cerr << "brdb_tuple: can not create instance of unknown type: "<< types[i] << std::endl;
      return nullptr;
    }
  }

  return new brdb_tuple(values);
}


//: Copy Constructor
brdb_tuple::brdb_tuple(const brdb_tuple& other)
: vbl_ref_count(), values_(other.values_.size(),nullptr)
{
  for (unsigned int i=0; i<other.values_.size(); ++i){
    if (other.values_[i])
      values_[i] = other.values_[i]->clone();
  }
}


//: Destructor
brdb_tuple::~brdb_tuple()
{
  for (unsigned int i=0; i<values_.size(); ++i){
    //delete values_[i];
  }
}


//========================= Accessors / Modifiers ==========================


//: Assignment operator
brdb_tuple&
brdb_tuple::operator = (const brdb_tuple& rhs)
{
  for (unsigned int i=0; i<values_.size(); ++i){
//    delete values_[i];
  }
  values_.resize(rhs.values_.size());
  for (unsigned int i=0; i<rhs.values_.size(); ++i){
    values_[i] = rhs.values_[i]->clone();
  }
  return *this;
}


//: Set a value by index
bool
brdb_tuple::set_value(unsigned int index, const brdb_value& value)
{
  // check index range
  if (index >= values_.size())
    return false;
  // check for whether this value exists
  if (!values_[index]){
    values_[index] = value.clone();
    return values_[index] != nullptr;
  }
  // check the type of the existing value
  assert(value.is_a() == values_[index]->is_a());

  // remove the old value;
//  delete values_[index];
  values_[index] = value.clone();
  return values_[index] != nullptr;
}

//: Get a value by index
bool
brdb_tuple::get_value(unsigned int index, brdb_value& value) const
{
  // check index range
  if (index >= values_.size())
    return false;
  // check the data type
  assert(value.is_a() == this->values_[index]->is_a());

  // check for an existing value
  if (!values_[index])
    return false;
  // assign the value if types agree
  return value.assign(*values_[index]);
}
//: Get a value pointer by index
bool
brdb_tuple::get_value(unsigned int index, brdb_value_sptr& value) const
{
  // check index range
  if (index >= values_.size())
    return false;
  // check for an existing value
  if (!values_[index])
    return false;
  // assign the value if types agree
  value =  values_[index];
  return true;
}

//: add a value into the tuple
bool
brdb_tuple::add_value(const brdb_value& value)
{
  // add it into the tuple
  values_.emplace_back(value.clone());
  return true;
}

//: add a value into the tuple
bool
brdb_tuple::add_value(brdb_value_sptr const& value)
{
  if (!value)
    return false;
  // add it into the tuple
  values_.emplace_back(value->clone());
  return true;
}

//: print all values of tuple
void
brdb_tuple::print() const
{
  for (unsigned int i=0; i<this->arity(); i++)
  {
    values_[i]->print();
  }
  std::cout<< std::endl;
}


//: binary io read values only
// read values from the stream only (assumes arity and types are initialized)
void
brdb_tuple::b_read_values(vsl_b_istream &is)
{
  for (auto & value : values_)
    value->b_read_value(is);
}


//: binary io write values only
// write values to the stream only
void
brdb_tuple::b_write_values(vsl_b_ostream &os) const
{
  for (const auto & value : values_)
    value->b_write_value(os);
}
