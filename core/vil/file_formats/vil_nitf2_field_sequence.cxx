// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <utility>
#include "vil_nitf2_field_sequence.h"
#include "vil_nitf2_index_vector.h"
#include "vil_nitf2_field_formatter.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_scalar_field.h"
#include "vil_nitf2_array_field.h"
#include "vil_nitf2_compound_field_value.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

vil_nitf2_field::field_tree *
vil_nitf2_field_sequence::get_tree(vil_nitf2_field::field_tree * tr) const
{
  vil_nitf2_field::field_tree * t = tr ? tr : new vil_nitf2_field::field_tree;
  for (auto i : fields_vector)
  {
    t->children.push_back(i->get_tree());
  }
  return t;
}

void
vil_nitf2_field_sequence::insert_field(const std::string & str, vil_nitf2_field * field)
{
  fields.insert(std::make_pair(str, field));
  fields_vector.push_back(field);
}

bool
vil_nitf2_field_sequence::create_array_fields(const vil_nitf2_field_definitions * field_defs, int num_dimensions)
{
  for (auto node : *field_defs)
  {
    if (node && node->is_field_definition())
    {
      vil_nitf2_field_definition * field_def = node->field_definition();
      vil_nitf2_array_field *      field = field_def->formatter->create_array_field(num_dimensions, field_def);
      if (field)
      {
        insert_field(field_def->tag, field);
      }
      else
      {
        std::cerr << "vil_nitf2_field_sequence:create_array_fields(): Error created required std::vector field "
                  << field_def->tag << "; bailing out.\n";
        return false;
      }
    }
    else if (node && node->is_repeat_node())
    {
      // recursively create nested vector fields
      vil_nitf2_field_definition_repeat_node * repeat_node = node->repeat_node();
      if (!create_array_fields(repeat_node->field_definitions, num_dimensions + 1))
      {
        return false;
      }
    }
    else
    {
      std::cerr << "vil_nitf2_field_sequence::create_array_fields(): unsupported node type!\n";
      return false;
    }
  }
  return true;
}

void
vil_nitf2_field_sequence::set_array_fields_dimension(const vil_nitf2_field_definitions * field_defs,
                                                     const vil_nitf2_index_vector &      index,
                                                     int                                 repeat_count)
{
  for (auto node : *field_defs)
  {
    if (node && node->is_field_definition())
    {
      vil_nitf2_field_definition * field_def = node->field_definition();
      vil_nitf2_array_field *      field = get_field(field_def->tag)->array_field();
      if (field)
      {
        VIL_NITF2_LOG(log_debug) << "  (Setting tag " << field_def->tag << " dimension " << index << " to "
                                 << repeat_count << ".)" << std::endl;
        field->set_next_dimension(index, repeat_count);
      }
      else
      {
        std::cerr << "vil_nitf2_field_sequence:set_array_field_dimension(): array field " << field_def->tag
                  << " not found!\n";
      }
    }
    else if (node && node->is_repeat_node())
    {
      // recursively set dimension vector fields
      vil_nitf2_field_definition_repeat_node * repeat_node = node->repeat_node();
      set_array_fields_dimension(repeat_node->field_definitions, index, repeat_count);
    }
    else
    {
      std::cerr << "vil_nitf2_field_sequence::set_array_fields_dimension(): unsupported node type!\n";
    }
  }
}

bool
vil_nitf2_field_sequence::read(vil_nitf2_istream &                 input,
                               const vil_nitf2_field_definitions * field_defs,
                               const vil_nitf2_index_vector &      indexes)
{
  if (!field_defs)
    field_defs = m_field_definitions;
  if (!field_defs)
    std::cerr << "vil_nitf2_field_sequence::read() missing field definitions!\n";
  bool error = false;
  for (auto node : *field_defs)
  {
    if (node && node->is_field_definition())
    {
      vil_nitf2_field_definition * field_def = node->field_definition();
      // The field exists if it is required, or if it is conditional and
      // the condition is true.
      bool field_exists;
      if (field_def->is_required())
      {
        field_exists = true;
      }
      else
      {
        bool condition;
        bool conditionValid = (*(field_def->condition_functor))(this, indexes, condition);
        if (conditionValid)
        {
          field_exists = condition;
        }
        else
        {
          // Cannot evaluate condition; therefore I don't know whether this
          // field exists and cannot reliably parse the rest of the record
          std::cerr << "vil_nitf2_field_sequence::read(): Cannot evaluate condition for tag " << field_def->tag << '\n';
          error = true;
          break;
        }
      }
      if (field_exists)
      {
        // Evaluate its width functor, if any.
        int variable_width = -1;
        if (field_def->width_functor != nullptr)
        {
          bool computed_width = (*(field_def->width_functor))(this, indexes, variable_width);
          if (!computed_width)
          {
            // Cannot evaluate width functor; therefore I don't know the length
            // of this field and cannot reliably parse the rest of the record
            std::cerr << "vil_nitf2_field_sequence::read(): Cannot evaluate width functor for tag " << field_def->tag
                      << '\n';
            error = true;
            break;
          }
        }
        if (variable_width == 0)
        {
          VIL_NITF2_LOG(log_debug) << "Skipping field " << field_def->tag << ", whose length = 0." << std::endl;
        }
        else
        {
          // Either there is no width functor, in which case variable_width = -1 and will be ignored,
          // or there is a width functor, and the resulting positive variable_width will be applied.
          if (indexes.size() == 0)
          {
            // read scalar field
            bool                     fieldReadError;
            vil_nitf2_scalar_field * field =
              vil_nitf2_scalar_field::read(input, field_def, variable_width, &fieldReadError);
            if (field)
            {
              insert_field(field_def->tag, field);
            }
            if (fieldReadError)
            {
              error = true;
              break;
            }
          }
          else
          {
            // read vector field element
            bool                         read_error = true;
            vil_nitf2_field_definition * field_def = node->field_definition();
            if (field_def)
            {
              vil_nitf2_array_field * field = get_field(field_def->tag)->array_field();
              if (field)
              {
                if (field->read_vector_element(input, indexes, variable_width))
                {
                  read_error = false;
                }
              }
            }
            if (read_error)
            {
              std::cerr << "vil_nitf2_field_sequence::read(): Couldn't find std::vector field!\n";
              return false;
            }
          }
        }
        // TO DO: Check that the expected amount of data was read; if not,
        // try to recover.
      }
    }
    else if (node && node->is_repeat_node())
    {
      vil_nitf2_field_definition_repeat_node * repeat_node = node->repeat_node();

      // Compute how many times it repeats
      int  repeat_count = 0;
      bool computed_repeat = false;
      if (repeat_node->repeat_functor != nullptr)
      {
        computed_repeat = (*(repeat_node->repeat_functor))(this, indexes, repeat_count);
      }
      if (!computed_repeat)
      {
        // Cannot evaluate repeat count; therefore I don't know the length
        // of this field and cannot reliably parse the rest of the record
        std::cerr << "Cannot evaluate repeat count for repeat node\n";
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
      if (indexes.size() == 0)
      {
        if (!create_array_fields(repeat_node->field_definitions, 1))
        {
          return false;
        }
      }
      // Loop repeat_count times over fields to read the elements
      std::string nesting_level_indicator((indexes.size() + 1) * 2, '-');
      VIL_NITF2_LOG(log_debug) << nesting_level_indicator << "Repeating fields " << repeat_count
                               << " times:" << std::endl;
      for (int i = 0; i < repeat_count; ++i)
      {
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
        if (i == 0)
        {
          set_array_fields_dimension(repeat_node->field_definitions, indexes, repeat_count);
        }

        // Now call myself recursively to read the vector field elements
        vil_nitf2_index_vector nested_indexes(indexes);
        nested_indexes.push_back(i);
        if (!read(input, repeat_node->field_definitions, nested_indexes))
        {
          return false;
        }
      }
      VIL_NITF2_LOG(log_debug) << nesting_level_indicator << "End repeating fields." << std::endl;
    }
    else
    {
      std::cerr << "vil_nitf2_tagged_record::read(): unsupported node.\n";
    }
  }
  return !error;
}

bool
vil_nitf2_field_sequence::write(vil_nitf2_ostream &                 output,
                                const vil_nitf2_field_definitions * field_defs,
                                vil_nitf2_index_vector              indexes)
{
  if (!field_defs)
    field_defs = m_field_definitions;
  if (!field_defs)
    std::cerr << "vil_nitf2_field_sequence::write(): Missing field definitions!\n";
  for (auto node : *field_defs)
  {
    if (node && node->is_field_definition())
    {
      vil_nitf2_field_definition * field_def = node->field_definition();
      if (!field_def)
      {
        std::cerr << "vil_nitf2_field_sequence::write(): Missing field definition!\n";
        return false;
      }
      vil_nitf2_field * field = get_field(field_def->tag);

      // Determine whether the field is required or is a conditional field
      // whose condition is satisfied
      bool expected = field_def->is_required();
      if (!expected)
      {
        bool condition;
        if ((*field_def->condition_functor)(this, indexes, condition))
        {
          expected |= condition;
        }
        else
        {
          std::cerr << "vil_nitf2_field_sequence::write(): Cound not evaluate condition for field " << field_def->tag
                    << std::endl;
          // Cannot evaluate condition, therefore I can't tell whether this
          // field should exist.
          return false;
        }
      }
      if (field && !expected)
      {
        std::cerr << "vil_nitf2_field_sequence::write(): Field " << field_def->tag
                  << " is being ignored because its condition is not satisfied.\n";
      }
      else
      {
        // Will emit field. Evaluate its width functor, if any.
        int variable_width = -1;
        if (field_def->width_functor != nullptr)
        {
          bool computed_width = (*(field_def->width_functor))(this, indexes, variable_width);
          if (!computed_width)
          {
            // Cannot evaluate width functor; therefore I don't know the length
            // of this field and cannot reliably parse the rest of the record
            std::cerr << "vil_nitf2_field_sequence::write(): Cannot evaluate width functor for tag " << field_def->tag
                      << std::endl;
            return false;
          }
        }
        if (variable_width == 0)
        {
          VIL_NITF2_LOG(log_debug) << "Skipping field " << field_def->tag << ", whose length = 0." << std::endl;
        }
        else
        {
          // Either there is no width functor, in which case variable_width = -1 and will be ignored,
          // or there is a width functor, and the resulting positive variable_width will be applied.
          if (!field && expected)
          {
            if (!field_def->blanks_ok)
            {
              std::cerr << "vil_nitf2_field_sequence::write(): Field " << field_def->tag
                        << " is unspecified; writing blanks.\n";
            }
            if (variable_width > 0)
              field_def->formatter->field_width = variable_width;
            field_def->formatter->write_blank(output);
          }
          else if (field)
          {
            if (field->scalar_field())
            {
              field->scalar_field()->write(output, variable_width);
            }
            else
            {
              field->array_field()->write_vector_element(output, indexes, variable_width);
            }
          }
        }
      }
    }
    else if (node && node->is_repeat_node())
    {
      vil_nitf2_field_definition_repeat_node * repeat_node = node->repeat_node();
      // Compute how many times it repeats
      int  repeat_count = 0;
      bool computed_repeat = false;
      if (repeat_node->repeat_functor != nullptr)
      {
        computed_repeat = (*(repeat_node->repeat_functor))(this, indexes, repeat_count);
      }
      if (!computed_repeat)
      {
        // Cannot evaluate repeat count; therefore I don't know the length
        // of this field.
        std::cerr << "vil_nitf2_field_sequence::write(): Cannot evaluate repeat count for repeat node\n";
        return false;
      }
      if (repeat_node->field_definitions)
      {
        for (int i = 0; i < repeat_count; ++i)
        {
          vil_nitf2_index_vector nested_indexes(indexes);
          nested_indexes.push_back(i);
          this->write(output, repeat_node->field_definitions, nested_indexes);
        }
      }
    }
    else
    {
      std::cerr << "vil_nitf2_field_sequence::write(): Ignoring unsupported node.\n";
    }
  }
  return true;
}

vil_nitf2_field_sequence::~vil_nitf2_field_sequence()
{
  // Delete fields, which I own
  for (auto & field_map_entry : fields)
  {
    vil_nitf2_field * field = field_map_entry.second;
    delete field;
  }
}

vil_nitf2_field *
vil_nitf2_field_sequence::get_field(const std::string & tag) const
{
  auto field_map_entry = fields.find(tag);
  if (field_map_entry == fields.end())
    return nullptr;
  return field_map_entry->second;
}

// Who needs templated functions when we have macros!
#define NITF_FIELD_SEQ_GET_VALUE(T)                                                                                    \
  bool vil_nitf2_field_sequence::get_value(std::string tag, T & out_value) const                                       \
  {                                                                                                                    \
    vil_nitf2_field *        field = get_field(tag);                                                                   \
    vil_nitf2_scalar_field * scalar_field = field ? field->scalar_field() : 0;                                         \
    if (!scalar_field)                                                                                                 \
    {                                                                                                                  \
      /*std::cerr << "vil_nitf2_field_sequence::get_value(" << tag << "): scalar field not found.\n";*/                \
      return false;                                                                                                    \
    }                                                                                                                  \
    if (!scalar_field->value(out_value))                                                                               \
    {                                                                                                                  \
      std::cerr << "vil_nitf2_field_sequence::get_value(" << tag << ") called with wrong type.\n";                     \
      return false;                                                                                                    \
    }                                                                                                                  \
    return true;                                                                                                       \
  }

NITF_FIELD_SEQ_GET_VALUE(int)
NITF_FIELD_SEQ_GET_VALUE(double)
NITF_FIELD_SEQ_GET_VALUE(char)
NITF_FIELD_SEQ_GET_VALUE(void *)
NITF_FIELD_SEQ_GET_VALUE(std::string)
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_location *)
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_date_time)
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_tagged_record_sequence)

#define NITF_FIELD_SEQ_GET_ARRAY_VALUE(T)                                                                              \
  bool vil_nitf2_field_sequence::get_value(                                                                            \
    std::string tag, const vil_nitf2_index_vector & indexes, T & out_value, bool ignore_extra_indexes) const           \
  {                                                                                                                    \
    vil_nitf2_field * field = get_field(tag);                                                                          \
    if (!field)                                                                                                        \
    {                                                                                                                  \
      /*std::cerr << "vil_nitf2_field_sequence::get_value(" << tag << ", const vil_nitf2_index_vector&): tag not       \
       * found.\n"; */                                                                                                 \
      return false;                                                                                                    \
    }                                                                                                                  \
    vil_nitf2_index_vector trimmed_indexes(indexes);                                                                   \
    if (ignore_extra_indexes && (int)indexes.size() > field->num_dimensions())                                         \
    {                                                                                                                  \
      trimmed_indexes.resize(field->num_dimensions());                                                                 \
    }                                                                                                                  \
    if (trimmed_indexes.size() == 0)                                                                                   \
    {                                                                                                                  \
      return field->scalar_field() && field->scalar_field()->value(out_value);                                         \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
      return field->array_field()->value(trimmed_indexes, out_value);                                                  \
    }                                                                                                                  \
  }

NITF_FIELD_SEQ_GET_ARRAY_VALUE(int) // expanded below for debugging
NITF_FIELD_SEQ_GET_ARRAY_VALUE(double)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(char)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(void *)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(std::string)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_location *)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_date_time)

// Macro to generate overloads of get_values(), since VXL coding
// standards forbid using templated member functions.
//
#define NITF_FIELD_SEQ_GET_VALUES(T)                                                                                   \
  bool vil_nitf2_field_sequence::get_values(                                                                           \
    std::string tag, const vil_nitf2_index_vector & indexes, std::vector<T> & out_values, bool clear_out_values) const \
  {                                                                                                                    \
    vil_nitf2_field * field = get_field(tag);                                                                          \
    if (!field)                                                                                                        \
    {                                                                                                                  \
      /*std::cerr << "vil_nitf2_field_sequence::get_value(" << tag << ", const vil_nitf2_index_vector&): tag not       \
       * found.\n"; */                                                                                                 \
      return false;                                                                                                    \
    }                                                                                                                  \
    if (clear_out_values)                                                                                              \
    {                                                                                                                  \
      out_values.clear();                                                                                              \
    }                                                                                                                  \
    int num_dims = field->num_dimensions();                                                                            \
    if (num_dims == (int)indexes.size())                                                                               \
    {                                                                                                                  \
      /* get single value */                                                                                           \
      T value;                                                                                                         \
      if (get_value(tag, indexes, value, false))                                                                       \
      {                                                                                                                \
        out_values.push_back(value);                                                                                   \
        return true;                                                                                                   \
      }                                                                                                                \
      else                                                                                                             \
      {                                                                                                                \
        return false;                                                                                                  \
      }                                                                                                                \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
      vil_nitf2_array_field * array_field = field->array_field();                                                      \
      if (!array_field)                                                                                                \
      {                                                                                                                \
        /* indexes is too long */                                                                                      \
        return false;                                                                                                  \
      }                                                                                                                \
      /* traverse value tree depth-first, collecting values into out_values */                                         \
      int dimension = array_field->next_dimension(indexes);                                                            \
      for (int index = 0; index < dimension; ++index)                                                                  \
      {                                                                                                                \
        vil_nitf2_index_vector next_indexes = indexes;                                                                 \
        next_indexes.push_back(index);                                                                                 \
        if (!get_values(tag, next_indexes, out_values, false))                                                         \
        {                                                                                                              \
          return false;                                                                                                \
        }                                                                                                              \
      }                                                                                                                \
      return true;                                                                                                     \
    }                                                                                                                  \
  }                                                                                                                    \
                                                                                                                       \
  bool vil_nitf2_field_sequence::get_values(std::string tag, std::vector<T> & out_values) const                        \
  {                                                                                                                    \
    return get_values(tag, vil_nitf2_index_vector(), out_values, true);                                                \
  }

NITF_FIELD_SEQ_GET_VALUES(int)
NITF_FIELD_SEQ_GET_VALUES(double)
NITF_FIELD_SEQ_GET_VALUES(char)
NITF_FIELD_SEQ_GET_VALUES(void *)
NITF_FIELD_SEQ_GET_VALUES(std::string)
NITF_FIELD_SEQ_GET_VALUES(vil_nitf2_location *)
NITF_FIELD_SEQ_GET_VALUES(vil_nitf2_date_time)

#if VXL_HAS_INT_64
// if not VXL_HAS_INT_64 isn't defined the vil_nitf2_long is the same as just plain 'int'
// and this function will be a duplicate of that get_value
NITF_FIELD_SEQ_GET_VALUE(vil_nitf2_long)
NITF_FIELD_SEQ_GET_ARRAY_VALUE(vil_nitf2_long)
NITF_FIELD_SEQ_GET_VALUES(vil_nitf2_long)
#endif
