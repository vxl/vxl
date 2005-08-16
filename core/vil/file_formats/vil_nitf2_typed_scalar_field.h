// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TYPED_SCALAR_FIELD_H
#define VIL_NITF2_TYPED_SCALAR_FIELD_H

#include "vil_nitf2_scalar_field.h"
#include "vil_nitf2.h"

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
  
  // Return value
  T value() const { return m_value; }

  // Overload vil_nitf2_field::value()
  bool value(T& out_value) const {
    out_value = m_value;
    return true;
  }

  // Set value
  void set_value(const T& value) { m_value = value; }

  // Output to stream. Overload as necessary.
  virtual vcl_ostream& output(vcl_ostream& os) const { return os << m_value; };

private:
  T m_value;
};

//==============================================================================
// implementation
//==============================================================================

#include "vil_nitf2_compound_field_value.h" 

// Overload for vil_nitf2_location* (Necessary because it's a pointer.)
inline vcl_ostream& vil_nitf2_typed_scalar_field<vil_nitf2_location*>::output(vcl_ostream& os) const
{
  if (m_value==0) {
    os << m_value;
  } else {
    os << *m_value;
  } 
  return os;
}

#endif // VIL_NITF2_TYPED_SCALAR_FIELD_H
