// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_field_functor.h"
#include "vil_nitf2_tagged_record.h"
#include "vil_nitf2_field.h"
#include "vil_nitf2_index_vector.h"

#include <vcl_string.h>

bool vil_nitf2_field_specified::
operator() (vil_nitf2_field_sequence* record, 
            const vil_nitf2_index_vector& indexes, bool& result) 
{
  if (!record->find_field_definition(tag)) {
    // Invalid tag
    return false;
  } 
  vil_nitf2_field* field = record->get_field(tag);
  if (field != 0) {
    vcl_string value;
    bool is_string_value = record->get_value(tag, indexes, value, true);
    if (is_string_value) {
      // a blank vcl_string field actually yields a valid field value (an empty 
      // vcl_string) so test the value
      result = !value.empty();
    } else {
      // other types of blank fields do not yield a field; since it
      // was found, it must not be blank
      result = false;
    }
  } else {
    // field not found; therefore the data was blank
    result = true;
  }
  return true;
}

bool vil_nitf2_max_field_value_plus_offset_and_threshold::
operator() (vil_nitf2_field_sequence* record, 
            const vil_nitf2_index_vector& indexes, int& value) 
{
  int value1 = 0;
  bool found = record->get_value(tag, indexes, value1, true);
  value1 += offset;
  value = (value1 < min_threshold) ? min_threshold : value1;
  return found;
}

bool vil_nitf2_multiply_field_values::
operator() (vil_nitf2_field_sequence* record, 
            const vil_nitf2_index_vector& indexes, int& value) {
  int value1, value2;
  bool found = record->get_value(tag_1, indexes, value1, true);
  found &= record->get_value(tag_2, indexes, value2, true);
  if (found) {
    value = value1 * value2;
    return true;
  } else {
    value = 0;
    return use_zero_if_tag_not_found;
  }
}