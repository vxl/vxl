// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TYPED_ARRAY_FIELD_H
#define VIL_NITF2_TYPED_ARRAY_FIELD_H

#include <vcl_map.h>

#include "vil_nitf2_array_field.h"
#include "vil_nitf2.h"

class vil_nitf2_index_vector;

// Typed concrete class for array fields. Stores values and implements I/O of 
// values.
//
template<class T>
class vil_nitf2_typed_array_field : public vil_nitf2_array_field
{
public:
  // Constructor
  vil_nitf2_typed_array_field(int num_dimensions, vil_nitf2_field_definition* field_definition) 
    : vil_nitf2_array_field(field_definition, num_dimensions) {};

  // Set out_value to the scalar value at the specified index vector,
  // and returns whether specified element was defined.
  // The length of the index vector must equal num_dimensions(),
  // and check_index(indexes) must return true to indicate that
  // the indexes are within bounds. Even so, this method may return
  // false if the value is undefined at the specified index.
  bool value(const vil_nitf2_index_vector& indexes, T& out_value) const;

  // Reads from input stream the scalar value at specified index.
  // check_index(indexes) must be true, or this will emit an error.
  // Returns success.
  bool read_vector_element(vil_nitf2_istream& input, 
    const vil_nitf2_index_vector& indexes, int variable_width);

  // Writes to output stream the scalar value at specified index.
  // check_index(indexes) must be true, of this will emit an error.
  // Returns success. Arg variable_width, if non-negative, overrides
  // formatter's field_width.
  bool write_vector_element(vil_nitf2_ostream& output, 
    const vil_nitf2_index_vector& indexes, int variable_width);

  // Output in human-readable form. Implementation provides an example of
  // how to iterate over all elements.
  virtual vcl_ostream& output(vcl_ostream& os) const;

protected:
  // Helper method for output() method above. Iterates over one
  // dimension of vector field, recursively printing all defined elements 
  // to output stream.
  void output_dimension_iterate(vcl_ostream& os, vil_nitf2_index_vector indexes,
                                bool& output_yet) const;

private:
  // Value (i,j,...) is stored in m_value_map[(i,j,...)], where the
  // length of the index vector (i,j,...) equals member m_num_dimensions.
  // A map is used here instead of simple vector for several reasons:
  // (1) this single class can represent a vector of any dimensionality; 
  // (2) due to conditional and blank nodes, a vector may be sparsely 
  // populated; and (3) a multi-dimensional vector's dimensions may vary.
  vcl_map<vil_nitf2_index_vector, T> m_value_map;
};

//==============================================================================
// implementation
//==============================================================================

#include "vil_nitf2_index_vector.h"
#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_typed_field_formatter.h"
#include "vil_nitf2_field_definition.h"

template<class T>
bool vil_nitf2_typed_array_field<T>::value(
  const vil_nitf2_index_vector& indexes, T& out_value) const 
{
  if ((int)indexes.size() != m_num_dimensions) {
    vcl_cerr << "vil_nitf2_typed_array_field index vector wrong length" << vcl_endl;
    return false;
  }
  typename vcl_map<vil_nitf2_index_vector, T>::const_iterator element = m_value_map.find(indexes);
  if (element != m_value_map.end()) {
    out_value = element->second;
    return true;
  } else return false;
}
  
template<class T>
bool vil_nitf2_typed_array_field<T>::
read_vector_element(vil_nitf2_istream& input, const vil_nitf2_index_vector& indexes,
                    int variable_width)
{
  VIL_NITF2_LOG(log_debug) << "Reading " << tag() << indexes << ": ";
  bool is_blank;
  if (!check_index(indexes)) {
    VIL_NITF2_LOG(log_debug) << "invalid index!" << vcl_endl;
    return false;
  }
  vil_nitf2_field_formatter* formatter = m_definition->formatter;
  // To do: make this cast safe!
  vil_nitf2_typed_field_formatter<T>* typed_formatter = (vil_nitf2_typed_field_formatter<T>*)formatter;
  // if non-negative variable_width is specified, it overrides the field_width
  int saved_field_width = typed_formatter->field_width;
  if (variable_width > 0) {
    typed_formatter->field_width = variable_width;
  }
  T val;
  bool value_read = typed_formatter->read(input, val, is_blank);
  typed_formatter->field_width = saved_field_width;
  if (value_read) {
    VIL_NITF2_LOG(log_debug) << val << vcl_endl;
    m_value_map[indexes] = val;
  } else if (is_blank && !m_definition->blanks_ok) {
    VIL_NITF2_LOG(log_debug) << "not specified, but required!" << vcl_endl;
  } else if (is_blank) {
    VIL_NITF2_LOG(log_debug) << "(unspecified)" << vcl_endl;
  } else {
    VIL_NITF2_LOG(log_debug) << "failed!" << vcl_endl;
    return false;
  }
  return true;
}

template<class T>
bool vil_nitf2_typed_array_field<T>::
write_vector_element(vil_nitf2_ostream& output, const vil_nitf2_index_vector& indexes,
                     int variable_width)
{
  VIL_NITF2_LOG(log_debug) << "Writing tag " << tag() << indexes << " ";
  if (!check_index(indexes)) {
    VIL_NITF2_LOG(log_debug) << ": invalid index!" << vcl_endl;
    return false;
  }
  T val;
  // To do: make this cast safe!
  vil_nitf2_typed_field_formatter<T>* typed_formatter = 
    (vil_nitf2_typed_field_formatter<T>*)m_definition->formatter;
  if (variable_width > 0) typed_formatter->field_width = variable_width;
  bool value_defined = value(indexes, val);
  if (value_defined) {
    VIL_NITF2_LOG(log_debug) << vcl_endl;
    return typed_formatter->write(output, val);
  } else {
    if (!m_definition->blanks_ok) {
      VIL_NITF2_LOG(log_debug) << ": required value undefined at this index; writing blanks." << vcl_endl;
    }
    return typed_formatter->write_blank(output);
  }
}

template<class T>
vcl_ostream& vil_nitf2_typed_array_field<T>::output(vcl_ostream& os) const
{
  bool output_yet = false;
  output_dimension_iterate(os, vil_nitf2_index_vector(), output_yet);
  return os;
}

template<class T>
void vil_nitf2_typed_array_field<T>::output_dimension_iterate(
  vcl_ostream& os, vil_nitf2_index_vector indexes, bool& output_yet) const
{
  if ((int)indexes.size()==m_num_dimensions) {
    T val;
    if (value(indexes, val)) {
      // flag output_yet is simply used to print a separator between 
      // elements within a line.
      if (output_yet) {
        os << ", ";
      } else {
        output_yet = true;
      }
      os << indexes << " " << val;
    }
  } else {
    int dim = next_dimension(indexes);
    for (int i=0; i < dim; ++i) {
      vil_nitf2_index_vector next_indexes(indexes);
      next_indexes.push_back(i);
      output_dimension_iterate(os, next_indexes, output_yet);
    }
    os << vcl_endl;
    output_yet = false;
  }
}

template<class T>
vcl_ostream& operator << (vcl_ostream& os, const vil_nitf2_typed_array_field<T>& field)
{
  return field->output(os);
};

#endif // VIL_NITF2_TYPED_ARRAY_FIELD_H
