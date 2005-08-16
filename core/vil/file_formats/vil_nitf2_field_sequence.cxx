// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_field_sequence.h"
#include "vil_nitf2_index_vector.h"
#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_scalar_field.h"
#include "vil_nitf2_array_field.h"

bool vil_nitf2_field_sequence::
create_array_fields(const vil_nitf2_field_definitions* field_defs,
                     int num_dimensions)
{
  for (vil_nitf2_field_definitions::const_iterator node = field_defs->begin();
       node != field_defs->end(); ++node)
  {
    if ((*node) && (*node)->is_field_definition()) { 
      vil_nitf2_field_definition* field_def = (*node)->field_definition();
      vil_nitf2_array_field* field = field_def->formatter->create_array_field(num_dimensions, field_def);
      if (field) {
        fields.insert(make_pair(field_def->tag, field));
      } else {
        vcl_cerr << "vil_nitf2_field_sequence:create_array_fields(): Error created required vcl_vector field " 
          << field_def->tag << "; bailing." << vcl_endl;
        return false;
      }
    } else if ((*node) && (*node)->is_repeat_node()) {
      // recursively create nested vector fields
      vil_nitf2_field_definition_repeat_node* repeat_node = (*node)->repeat_node();
      if (!create_array_fields(repeat_node->field_definitions, num_dimensions+1)) {
        return false;
      }
    } else {
      vcl_cerr << "vil_nitf2_field_sequence::create_array_fields(): unsupported node type!" << vcl_endl;
      return false;
    }
  }
  return true;
}

void vil_nitf2_field_sequence::set_array_fields_dimension(
  const vil_nitf2_field_definitions* field_defs,
  const vil_nitf2_index_vector& index, int repeat_count)
{
  for (vil_nitf2_field_definitions::const_iterator node = field_defs->begin();
       node != field_defs->end(); ++node)
  {
    if ((*node) && (*node)->is_field_definition()) { 
      vil_nitf2_field_definition* field_def = (*node)->field_definition();
      vil_nitf2_array_field* field = get_field(field_def->tag)->array_field();
      if (field) {
        VIL_NITF2_LOG(log_debug) << "  (Setting tag " << field_def->tag << " dimension " 
          << index << " to " << repeat_count << ".)" << vcl_endl;
        field->set_next_dimension(index, repeat_count);
      } else {
        vcl_cerr << "vil_nitf2_field_sequence:set_array_field_dimension(): array field " 
          << field_def->tag << " not found!" << vcl_endl;
      }
    } else if ((*node) && (*node)->is_repeat_node()) {
      // recursively set dimension vector fields
      vil_nitf2_field_definition_repeat_node* repeat_node = (*node)->repeat_node();
      set_array_fields_dimension(repeat_node->field_definitions, index, repeat_count);
    } else {
      vcl_cerr << "vil_nitf2_field_sequence::set_array_fields_dimension(): unsupported node type!" << vcl_endl;
    }
  }
}

bool vil_nitf2_field_sequence::read(vil_nitf2_istream& input, 
                                    const vil_nitf2_field_definitions* field_defs, 
                                    vil_nitf2_index_vector indexes) 
{
  if (!field_defs) field_defs = m_field_definitions;
  if (!field_defs) vcl_cerr << "vil_nitf2_field_sequence::read() missing field definitions!" << vcl_endl;
  bool error = false;
  for (vil_nitf2_field_definitions::const_iterator node = field_defs->begin();
       node != field_defs->end(); ++node)
  {
    if ((*node) && (*node)->is_field_definition()) {
      vil_nitf2_field_definition* field_def = (*node)->field_definition();
      // The field exists if it is required, or if it is conditional and
      // the condition is true.
      bool fieldExists;
      if (field_def->is_required()) {
        fieldExists = true;
      } else {
        bool condition;
        bool conditionValid = (*(field_def->condition_functor))(this, indexes, condition);
        if (conditionValid) {
          fieldExists = condition;
        } else {
          // Cannot evaluate condition; therefore I don't know whether this
          // field exists and cannot reliably parse the rest of the record
          vcl_cerr << "vil_nitf2_field_sequence::read(): Cannot evaluate condition for tag " << field_def->tag << vcl_endl;
          error = true;
          break;
        }
      }
      if (fieldExists)
      {
        // Evaluate its width functor, if any.
        int variable_width = -1;
        if (field_def->width_functor != 0) {
          bool computed_width = (*(field_def->width_functor))(this, indexes, variable_width);
          if (!computed_width) {
            // Cannot evaluate width functor; therefore I don't know the length
            // of this field and cannot reliably parse the rest of the record
            vcl_cerr << "vil_nitf2_field_sequence::read(): Cannot evaluate width functor for tag " << field_def->tag << vcl_endl;
            error = true;
            break;
          }
        }
        if (variable_width == 0) {
          VIL_NITF2_LOG(log_debug) << "Skipping field " << field_def->tag << ", whose length = 0." << vcl_endl;
        } else {
          // Either there is no width functor, in which case variable_width = -1 and will be ignored,
          // or there is a width functor, and the resulting positive variable_width will be applied.
          if (indexes.size()==0) {
            // read scalar field
            bool fieldReadError;
            vil_nitf2_scalar_field* field = vil_nitf2_scalar_field::read(input, field_def, variable_width, &fieldReadError);
            if (field) {
              fields.insert(make_pair(field_def->tag, field));
            } 
            if ( fieldReadError ){
              error = true;
              break;
            }

          } else {
            // read vector field element
            bool read_error = true;
            vil_nitf2_field_definition* field_def = (*node)->field_definition();
            if (field_def) {
              vil_nitf2_array_field* field = get_field(field_def->tag)->array_field();
              if (field) {
                if (field->read_vector_element(input, indexes, variable_width)) {
                  read_error = false;
                }
              }
            }
            if (read_error) {
              vcl_cerr << "vil_nitf2_field_sequence::read(): Couldn't find vcl_vector field!" << vcl_endl;
              return false;
            }
          }
        } 
        // TO DO: Check that the expected amount of data was read; if not,
        // try to recover.
      }
    } else if ((*node) && (*node)->is_repeat_node()) {

      vil_nitf2_field_definition_repeat_node* repeat_node = (*node)->repeat_node();

      // Compute how many times it repeats
      int repeat_count = 0;
      bool computed_repeat = false;
      if (repeat_node->repeat_functor != 0) {
        computed_repeat = (*(repeat_node->repeat_functor))(this, indexes, repeat_count);
      }
      if (!computed_repeat) {
        // Cannot evaluate repeat count; therefore I don't know the length
        // of this field and cannot reliably parse the rest of the record
        vcl_cerr << "Cannot evaluate repeat count for repeat node" << vcl_endl;
        error = true;
        break;
      }
      // On the first call to this method, call create_array_fields to loop
      // recursively over the field definitions to create all vector fields.
      // All nested fields need to be defined before any values are read. This is
      // is so that we can start setting the bounds on outer dimensions of 
      // nested field at the top of each repeat loop. 
      //
      // For example, for this field sequence:
      //   REPEAT i=1..N
      //     FIELD A(i)
      //     REPEAT j=1..A(i)
      //       FIELD B(i,j)
      // the following call to create_array_fields sets up fields
      // A (with 1 dimension) and B (with 2 dimensions).
      if (indexes.size() == 0) {
        if (!create_array_fields(repeat_node->field_definitions, 1)) {
          return false;
        }
      }
      // Loop repeat_count times over fields to read the elements
      vcl_string nesting_level_indicator((indexes.size()+1)*2, '-');
      VIL_NITF2_LOG(log_debug) << nesting_level_indicator 
        << "Repeating fields " << repeat_count << " times:" << vcl_endl;
      for (int i=0; i<repeat_count; ++i) {

        // The first time through the repeat loop, set the dimension 
        // bounds of all fields, including repeated fields. So, for the 
        // example above, during the first call to this method read(),
        // the invocation of set_fields_bounds() will set:
        //   A.dimension(vector()) = N
        //   B.dimension(vector()) = N
        // During the recursive calls to read(), the two invocations of
        // set_field_bounds() will set:
        //   B.dimension(vector(1)) = A(1)
        //   B.dimension(vector(2)) = A(2)
        // Actually, the indexes are zero-based, but this gives the general
        // idea.
        if (i==0) {
          set_array_fields_dimension(repeat_node->field_definitions, indexes, repeat_count);
        }

        // Now call myself recursively to read the vector field elements
        vil_nitf2_index_vector nested_indexes(indexes);
        nested_indexes.push_back(i);
        if (!read(input, repeat_node->field_definitions, nested_indexes)) {
          return false;
        }
      }
      VIL_NITF2_LOG(log_debug) << nesting_level_indicator 
        << "End repeating fields." << vcl_endl;

    } else {
      vcl_cerr << "vil_nitf2_tagged_record::read(): unsupported node." << vcl_endl;
    }
  }
  return !error;
}

bool vil_nitf2_field_sequence::write(vil_nitf2_ostream& output, 
                                     const vil_nitf2_field_definitions* field_defs,
                                     vil_nitf2_index_vector indexes)
{
  if (!field_defs) field_defs = m_field_definitions;
  if (!field_defs) vcl_cerr << "vil_nitf2_field_sequence::write(): Missing field definitions!" << vcl_endl;
  for (vil_nitf2_field_definitions::const_iterator node = field_defs->begin();
       node != field_defs->end(); ++node)
  {
    if ((*node) && (*node)->is_field_definition()) {

      vil_nitf2_field_definition* field_def = (*node)->field_definition();
      if (!field_def) {
        vcl_cerr << "vil_nitf2_field_sequence::write(): Missing field definition!" << vcl_endl;
        return false;
      }
      vil_nitf2_field* field = get_field(field_def->tag);

      // Determine whether the field is required or is a conditional field
      // whose condition is satisifed
      bool expected = field_def->is_required();
      if (!expected) {
        bool condition;
        if ((*field_def->condition_functor)(this, indexes, condition)) {
          expected |= condition;
        } else {
          vcl_cerr << "vil_nitf2_field_sequence::write(): Cound not evaluate condition for field " 
            << field_def->tag << vcl_endl;
          // Cannot evaluate condition, therefore I can't tell whether this
          // field should exist.
          return false;
        }
      }
      if (field && !expected) {
        vcl_cerr << "vil_nitf2_field_sequence::write(): Field " << field_def->tag 
          << " is being ignored because its condition is not satisfied." << vcl_endl;
      } else {

        // Will emit field. Evaluate its width functor, if any.
        int variable_width = -1;
        if (field_def->width_functor != 0) {
          bool computed_width = (*(field_def->width_functor))(this, indexes, variable_width);
          if (!computed_width) {
            // Cannot evaluate width functor; therefore I don't know the length
            // of this field and cannot reliably parse the rest of the record
            vcl_cerr << "vil_nitf2_field_sequence::write(): Cannot evaluate width functor for tag " << field_def->tag << vcl_endl;
            return false;
          }
        }
        if (variable_width == 0) {
          VIL_NITF2_LOG(log_debug) << "Skipping field " << field_def->tag << ", whose length = 0." << vcl_endl;
        } else {
          // Either there is no width functor, in which case variable_width = -1 and will be ignored,
          // or there is a width functor, and the resulting positive variable_width will be applied.          
          if (!field && expected) {
            if (!field_def->blanks_ok) {
              vcl_cerr << "vil_nitf2_field_sequence::write(): Field " << field_def->tag 
                << " is unspecified; writing blanks." << vcl_endl;
            }
            if (variable_width > 0) field_def->formatter->field_width = variable_width;
            field_def->formatter->write_blank(output);
          } else if (field) {
            if (field->scalar_field()) {
              field->scalar_field()->write(output, variable_width);
            } else {
              field->array_field()->write_vector_element(output, indexes, variable_width);
            }
          }
        }
      }
    } else if ((*node) && (*node)->is_repeat_node()) {

      vil_nitf2_field_definition_repeat_node* repeat_node = (*node)->repeat_node();
      // Compute how many times it repeats
      int repeat_count = 0;
      bool computed_repeat = false;
      if (repeat_node->repeat_functor != 0) {
        computed_repeat = (*(repeat_node->repeat_functor))(this, indexes, repeat_count);
      }
      if (!computed_repeat) {
        // Cannot evaluate repeat count; therefore I don't know the length
        // of this field.
        vcl_cerr << "vil_nitf2_field_sequence::write(): Cannot evaluate repeat count for repeat node" << vcl_endl;
        return false;
      }
      if (repeat_node->field_definitions) {
        for (int i=0; i < repeat_count; ++i) {
          vil_nitf2_index_vector nested_indexes(indexes);
          nested_indexes.push_back(i);
          this->write(output, repeat_node->field_definitions, nested_indexes);
        }
      }
    } else {
      vcl_cerr << "vil_nitf2_field_sequence::write(): Ignoring unsupported node." << vcl_endl;
    }
  }
  return true;
}

vil_nitf2_field_sequence::~vil_nitf2_field_sequence()
{
  // Delete fields, which I own
  for (field_map::iterator fieldMapEntry = fields.begin();
    fieldMapEntry != fields.end(); ++fieldMapEntry)
  {
    vil_nitf2_field* field = fieldMapEntry->second;
    delete field;
  }
}

vil_nitf2_field* vil_nitf2_field_sequence::get_field(vcl_string tag) const
{
  vcl_map<vcl_string, vil_nitf2_field*>::const_iterator fieldMapEntry = fields.find(tag);
  if (fieldMapEntry == fields.end()) return 0;
  return fieldMapEntry->second;
}     

// Who needs templated functions when we have macros!
#define NITF_FIELD_SEQ_GET_VALUE(T) \
bool vil_nitf2_field_sequence::get_value(vcl_string tag, T& out_value) const { \
  vil_nitf2_field* field = get_field(tag); \
  vil_nitf2_scalar_field* scalar_field = field ? field->scalar_field() : 0; \
  if (!scalar_field) { \
    /*vcl_cerr << "vil_nitf2_field_sequence::get_value(" << tag << "): scalar field not found." << vcl_endl;*/ \
    return false; \
  } \
  if (!scalar_field->value(out_value)) { \
    vcl_cerr << "vil_nitf2_field_sequence::get_value(" << tag << ") called with wrong type." << vcl_endl; \
    return false; \
  } \
  return true; \
}

NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_long)
NITF_FIELD_SEQ_GET_VALUE(int)
NITF_FIELD_SEQ_GET_VALUE(double)
NITF_FIELD_SEQ_GET_VALUE(char)
NITF_FIELD_SEQ_GET_VALUE(void*)
NITF_FIELD_SEQ_GET_VALUE(vcl_string)
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_location*)
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_date_time)

#define NITF_FIELD_SEQ_GET_ARRAY_VALUE(T) \
bool vil_nitf2_field_sequence::get_value(vcl_string tag, \
                                         const vil_nitf2_index_vector& indexes, \
                                         T& out_value, \
                                         bool ignore_extra_indexes) const { \
  vil_nitf2_field* field = get_field(tag); \
  if (!field) { \
    /*vcl_cerr << "vil_nitf2_field_sequence::get_value(" << tag << ", const vil_nitf2_index_vector&): tag not found." << vcl_endl; */\
    return false; \
  } \
  vil_nitf2_index_vector trimmed_indexes(indexes); \
  if (ignore_extra_indexes && (int)indexes.size() > field->num_dimensions()) { \
     trimmed_indexes.resize(field->num_dimensions()); \
  } \
  if (trimmed_indexes.size()==0) { \
    return field->scalar_field() && field->scalar_field()->value(out_value); \
  } else { \
    return field->array_field()->value(trimmed_indexes, out_value); \
  } \
} 
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_long)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(int) // expanded below for debugging
NITF_FIELD_SEQ_GET_ARRAY_VALUE(double)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(char)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(void*)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vcl_string)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_location*)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_date_time)
