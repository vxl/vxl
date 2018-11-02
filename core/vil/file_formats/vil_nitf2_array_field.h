//:
// \file
// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_ARRAY_FIELD_H
#define VIL_NITF2_ARRAY_FIELD_H

#include <map>
#include <string>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vil_nitf2_field.h"
#include "vil_nitf2_index_vector.h"

#include "vil_nitf2_field_sequence.h"

class vil_nitf2_field_definition;
class vil_nitf2_location;
class vil_nitf2_date_time;

//: Abstract class for array fields, i.e., fields that occur within a repeat loop.
// Since repeat loops can be nested, this field can be multi-dimensional.
// Its row dimensions don't even need to be same. One instance of this
// class "collects" all the values associated with a tag, even though they
// may be interleaved among other values in the NITF file. This class
// provides dimensional information and implements methods common to the
// type-specific subclasses.

class vil_nitf2_array_field : public vil_nitf2_field
{
 public:
  // Constructor
  vil_nitf2_array_field(vil_nitf2_field_definition* definition, int num_dimensions)
    : vil_nitf2_field(definition), m_num_dimensions(num_dimensions) {}

  // Destructor
  ~vil_nitf2_array_field() override = default;

  //: Number of dimensions.
  // \returns this vector's number of dimensions, which equals
  // its "repeat" nesting level.
  int num_dimensions() const override;

  //: Given a partial index vector, set the value of the next dimension.
  // Length of indexes must be less than num_dimensions(). See comment
  // for member m_dimensions_map, below; indexes is its key. For example,
  // if indexes is empty, the first dimension is set.
  void set_next_dimension(const vil_nitf2_index_vector& indexes, int bound);

  //: Given a partial index vector, return value of next dimension (or zero if none).
  // Length of indexes must be less than num_dimensions(). See comment
  // for member m_dimensions_map, below; indexes is its key. For
  // example, if indexes is empty, the first dimension is retrieved.
  int next_dimension(const vil_nitf2_index_vector& indexes) const;

  //: Compares index vector against value dimensions.
  bool check_index(const vil_nitf2_index_vector& indexes) const;

  //: Reads from input stream the scalar value at specified index.
  // check_index(indexes) must be true, or this will emit an error.
  // \returns success.
  virtual bool read_vector_element(vil_nitf2_istream& input,
                                   const vil_nitf2_index_vector& indexes,
                                   int variable_width) = 0;

  //: Writes to output stream the scalar value at specified index.
  // check_index(indexes) must be true, of this will emit an error.
  // Returns success.  Arg variable_width, if non-negative, overrides
  // formatter's field_width.
  virtual bool write_vector_element(vil_nitf2_ostream& output,
                                    const vil_nitf2_index_vector& indexes,
                                    int variable_width) const = 0;

  field_tree* get_tree() const override;

  //:
  // Sets out_value to the value of the element selected by specified
  // index vector, which must satisfy check_index(). Returns true iff the
  // value is defined. Note that this may return false because the value
  // is unspecified (i.e., blank), even if the index is valid.
  //
  // Subclasses overload the appropriate method to set out parameter and
  // return true. The implementation here return false. These methods are
  // defined here for the convenience of my callers, so they don't have to
  // downcast to the specific field type.
#if VXL_HAS_INT_64
  virtual bool value(const vil_nitf2_index_vector&, vil_nitf2_long& ) const { return false; }
#endif
  virtual bool value(const vil_nitf2_index_vector&, int& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, double& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, char& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, void*& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, std::string& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, vil_nitf2_location*& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, vil_nitf2_date_time& ) const { return false; }
  virtual bool value(const vil_nitf2_index_vector&, vil_nitf2_tagged_record_sequence& ) const { return false; }

 protected:
  void do_dimension( const vil_nitf2_index_vector& index,
                     vil_nitf2_field::field_tree* tr ) const;
  std::string get_value_string(const vil_nitf2_index_vector& in_indices) const;


  //: Dimensionality of vector field
  int m_num_dimensions;

  //:
  // Because a repeating field's dimension can depend on the value of
  // another repeating field, slices of a multi-dimensional vector field
  // can have varying dimensions (for an example test case, see method
  // vil_nitf2_tagged_record::test()). Dimensions are therefore stored
  // here as follows:
  //   - m_dimensions_map[vector()] holds the first dimension;
  //   - m_dimensions_map[vector(i)] holds the second dimension of
  //     row i of a 2-or-more-dimensional vector;
  //   - m_dimensions_map[vector(i,j)] holds the third dimension of
  //     plane (i,j) of a 3-or-more-dimensional vector;
  // and so on, according dimensionality of the field.
  std::map<vil_nitf2_index_vector, int> m_dimensions_map;
};

#endif // VIL_NITF2_ARRAY_FIELD_H
