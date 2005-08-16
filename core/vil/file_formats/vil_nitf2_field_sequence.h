// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_FIELD_SEQUENCE_H
#define VIL_NITF2_FIELD_SEQUENCE_H

#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include "vil_nitf2.h" // vil_nitf2_istream, vil_nitf2_ostream
#include "vil_nitf2_index_vector.h"

class vil_nitf2_field;
class vil_nitf2_field_definition;
class vil_nitf2_field_definitions;
class vil_nitf2_location;
class vil_nitf2_date_time;

//-----------------------------------------------------------------------------
// vil_nitf2_field_sequence represents the values of a continguous list of fields, 
// such as found in a NITF file header, image subheader, or tagged record extension.
//
class vil_nitf2_field_sequence
{
public:
  // Constructor
  vil_nitf2_field_sequence(const vil_nitf2_field_definitions& field_definitions) 
    : m_field_definitions(&field_definitions) {};
  
  // Destructor
  virtual ~vil_nitf2_field_sequence();
 
  // Sets out_value to the value of field specified by tag. 
  // Returns 0 if such a field is not found or is of the wrong type.
  bool get_value(vcl_string tag, long long& out_value) const;
  bool get_value(vcl_string tag, int& out_value) const;
  bool get_value(vcl_string tag, double& out_value) const;
  bool get_value(vcl_string tag, char& out_value) const;
  bool get_value(vcl_string tag, void*& out_value) const;
  bool get_value(vcl_string tag, vcl_string& out_value) const;
  bool get_value(vcl_string tag, vil_nitf2_location*& out_value) const;
  bool get_value(vcl_string tag, vil_nitf2_date_time& out_value) const;

  // Sets out_value to the value of the array field element specified by tag and index.
  // Returns 0 if such a field is not found or is of the wrong type. If ignore_extra_indexes
  // is true, then it trims indexes, if necessary to match the dimensionality of the vector.
  // This option is used by vil_nitf2_field_functor so that when in a repeat loop, the
  // value in the nearest enclosing scope will be used. For instance, if called with
  // indexes (i,j), and if tag dimensionality equals 1, then value(i) will be returned.
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, long long& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, int& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, double& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, char& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, void*& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vcl_string& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_location*& out_value, bool ignore_extra_indexes = false) const;
  bool get_value(vcl_string tag, const vil_nitf2_index_vector& indexes, vil_nitf2_date_time& out_value, bool ignore_extra_indexes = false) const;
 
  // Sets the value of the integer-valued field specified by tag to value.
  // Not yet implemented.
  //bool set_value(vcl_string tag, int value) { return false; }

  // Returns a field with specified tag, or 0 if not found.
  vil_nitf2_field* get_field(vcl_string tag) const;

  // Removes the field with the specified tag, returning whether successful.
  // Not yet implemented.
  //bool remove_field(vcl_string tag) { return false; }

  // Read field definition sequence from input stream. If this is called within
  // a repeat node, then indexes must equal the indexes must equal the 
  // current repeat iteration(s) and field_defs must equal the repeat node's
  // field definitions only.
  bool read(vil_nitf2_istream& input, 
            const vil_nitf2_field_definitions* field_defs = 0, 
            vil_nitf2_index_vector indexes = vil_nitf2_index_vector());

  // Attempts to write field sequence to the output stream. Arg
  // 'indexes' is used only during recursive calls to write nested sequences.
  virtual bool write(vil_nitf2_ostream&, 
                     const vil_nitf2_field_definitions* field_defs = 0,
                     vil_nitf2_index_vector indexes = vil_nitf2_index_vector());

  // Create vector fields for the specified field definitions with specified
  // number of dimensions. Return success.
  bool create_array_fields(const vil_nitf2_field_definitions* field_defs,
                            int num_dimensions);

  // Recursively set the specified dimension for vector fields to repeat_count.
  void set_array_fields_dimension(const vil_nitf2_field_definitions* field_defs,
                                   const vil_nitf2_index_vector& index, int repeat_count);

  // Returns field definition if found
  vil_nitf2_field_definition* find_field_definition(vcl_string tag);

private:
  typedef vcl_map<vcl_string, vil_nitf2_field*> field_map;
  field_map fields;
  const vil_nitf2_field_definitions* m_field_definitions;
};

#endif // VIL_NITF2_FIELD_SEQUENCE_H