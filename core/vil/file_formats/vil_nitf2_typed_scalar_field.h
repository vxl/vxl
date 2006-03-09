// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TYPED_SCALAR_FIELD_H
#define VIL_NITF2_TYPED_SCALAR_FIELD_H

#include "vil_nitf2_scalar_field.h"
#include "vil_nitf2.h"
#include <vcl_iosfwd.h>

// Typed concrete class for scalar NITF fields.
// During file reading, this class is instantiated only for non-blank 
// fields that are successfully parsed. Thus, every instance of
// vil_nitf2_scalar_field represents a valid value.
//
template<class T>
class vil_nitf2_typed_scalar_field : public vil_nitf2_scalar_field
{
 public:
  // Constructor
  vil_nitf2_typed_scalar_field(T value, vil_nitf2_field_definition* definition) 
    : vil_nitf2_scalar_field(definition), m_value(value) {}

  // Desructor
  ~vil_nitf2_typed_scalar_field(){};

  // Set out_value to my value and return true.
  // (This is a partial override of overloaded method 
  // vil_nitf2_scalar_field::value() for my specific type.)
  virtual bool value(T& out_value) const {
    out_value = m_value;
    return true;
  }

  // Return my value 
  // (was named value(), renamed to avoid an internal bcc compiler error)
  T get_value() const { return m_value; }

  // Set value
  void set_value(const T& value) { m_value = value; }

  // Output to stream. Overload as necessary.
  virtual vcl_ostream& output(vcl_ostream& os) const { return os << m_value; };

  virtual field_tree* get_tree() const { return vil_nitf2_scalar_field::get_tree(); }
 private:
  T m_value;
};

//==============================================================================
// implementation
//==============================================================================

#include "vil_nitf2_compound_field_value.h" 

// Overload for vil_nitf2_location* (Necessary because it's a pointer.)
template<>
inline vcl_ostream& vil_nitf2_typed_scalar_field<vil_nitf2_location*>::output(vcl_ostream& os) const
{
  if (m_value==0) {
    os << m_value;
  } else {
    os << *m_value;
  } 
  return os;
}

template<>
inline vil_nitf2_field::field_tree* 
vil_nitf2_typed_scalar_field<vil_nitf2_tagged_record_sequence>::get_tree() const
{
  field_tree* tr = new field_tree;
  tr->columns.push_back( "TREs" );
  vil_nitf2_tagged_record_sequence::const_iterator it;
  for( it = m_value.begin() ; it != m_value.end() ; it++ ) {
    tr->children.push_back( (*it)->get_tree() );
  }
  return tr;
}

template<>
inline vil_nitf2_typed_scalar_field<void*>::~vil_nitf2_typed_scalar_field()
{
  // vector delete corresponds to new char[] of binary data
  delete[] m_value;
} 

template<>
inline vil_nitf2_typed_scalar_field<vil_nitf2_location*>::~vil_nitf2_typed_scalar_field()
{
  delete m_value;
} 

#endif // VIL_NITF2_TYPED_SCALAR_FIELD_H
