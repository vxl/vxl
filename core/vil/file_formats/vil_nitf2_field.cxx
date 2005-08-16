// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2.h"
#include "vil_nitf2_field.h"
#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_index_vector.h"

// TO DO: move this to vil_nitf2.cpp
vil_nitf2::enum_log_level vil_nitf2::s_log_level = vil_nitf2::log_none;

vil_nitf2::enum_field_type vil_nitf2_field::type() const 
{ 
  if (m_definition && m_definition->formatter) {
    return m_definition->formatter->field_type;
  } else { 
    return vil_nitf2::type_undefined;
  }
}

vcl_string vil_nitf2_field::tag() const 
{
  return m_definition->tag;
}

vcl_string vil_nitf2_field::pretty_name() const 
{
  return m_definition->pretty_name;
}

vcl_string vil_nitf2_field::description() const 
{ 
  return m_definition->description; 
}

vil_nitf2_scalar_field* vil_nitf2_field::scalar_field()
{
  if (num_dimensions()==0) {
    return (vil_nitf2_scalar_field*) this;
  } else {
    return 0;
  }
}

vil_nitf2_array_field* vil_nitf2_field::array_field()
{
  if (num_dimensions()>0) {
    return (vil_nitf2_array_field*) this;
  } else {
    return 0;
  }
}

vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_field& field) 
{
  return field.output(os);
}

