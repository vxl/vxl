// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_scalar_field.h"
#include "vil_nitf2.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_field_formatter.h"

vil_nitf2_scalar_field* 
vil_nitf2_scalar_field::read(vil_nitf2_istream& input, 
                             vil_nitf2_field_definition* definition, 
                             int variable_width, bool* error)
{
  if (error) (*error) = false;
  if (!definition || !definition->formatter) {
    vcl_cerr << "vil_nitf2_field::read(): Incomplete field definition!" << vcl_endl;
    return 0;
  }
  vil_nitf2_scalar_field* result = 0;
  vil_nitf2_field_formatter* formatter = definition->formatter;
  // variable_width, if positive, overrides formatter field_width
  if (variable_width > 0) formatter->field_width = variable_width;
  // Parse string
  VIL_NITF2_LOG(log_debug) << "Reading tag " << definition->tag << ": ";
  bool is_blank;
  result = formatter->read_field(input, is_blank);

  // Set result definition, if found, and output (for debugging):
  //   value if computed
  //   warning if required field missing (scalar field or vcl_vector element)
  //   failed message if neither of the above apply
  if (result!=0) {
    result->m_definition = definition;
    VIL_NITF2_LOG(log_debug) << *result;
  } else if (is_blank && !definition->blanks_ok) {
    VIL_NITF2_LOG(log_debug) << "required field not specified!";
    if (error) (*error) = true;
  } else if (is_blank) {
    VIL_NITF2_LOG(log_debug) << "(unspecified)";
  } else {
    VIL_NITF2_LOG(log_debug) << "failed!";
    if (error) (*error) = true;
  }
  VIL_NITF2_LOG(log_debug) << vcl_endl;
  return result;
}

bool vil_nitf2_scalar_field::write(vil_nitf2_ostream& output, int variable_width)
{
  if (!m_definition || !m_definition->formatter) {
    vcl_cerr << "vil_nitf2_scalar_field::write(): Incomplete field definition!" << vcl_endl;
    return 0;
  }
  VIL_NITF2_LOG(log_debug) << "Writing tag " << m_definition->tag << ":";
  vil_nitf2_field_formatter* formatter = m_definition->formatter;
  // variable_width, if non-negative, overrides formatter's field_width
  if (variable_width > 0) formatter->field_width = variable_width;
  formatter->write_field(output, this);
  VIL_NITF2_LOG(log_debug) << vcl_endl;
  return output.ok();
}
