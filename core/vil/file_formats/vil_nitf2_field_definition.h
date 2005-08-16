// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_FIELD_DEFINITION_H
#define VIL_NITF2_FIELD_DEFINITION_H

// Do not #include this file within another header file (to avoid
// potential conflicts with the macro definitions below).

// These macros make definitions more concise
#define NITF_LOC  new vil_nitf2_location_formatter
#define NITF_INT  new vil_nitf2_integer_formatter
#define NITF_DBL  new vil_nitf2_double_formatter
#define NITF_LONG new vil_nitf2_long_long_formatter
#define NITF_CHAR new vil_nitf2_char_formatter
#define NITF_BIN  new vil_nitf2_binary_formatter
#define NITF_STR  new vil_nitf2_string_formatter
#define NITF_ENUM new vil_nitf2_enum_string_formatter
#define NITF_DAT  new vil_nitf2_date_time_formatter

#define NITF_STR_ECS(LEN)  NITF_STR(LEN, vil_nitf2_string_formatter::ECS)
#define NITF_STR_ECSA(LEN) NITF_STR(LEN, vil_nitf2_string_formatter::ECSA)
#define NITF_STR_BCS(LEN)  NITF_STR(LEN, vil_nitf2_string_formatter::BCS)
#define NITF_STR_BCSA(LEN) NITF_STR(LEN, vil_nitf2_string_formatter::BCSA)

#include <vcl_list.h>

#include "vil_nitf2.h" // vil_nitf2_istream, vil_nitf2_ostream
#include "vil_nitf2_field_functor.h"

class vil_nitf2_field_definition;
class vil_nitf2_field_definition_repeat_node;
class vil_nitf2_field_formatter;

//-----------------------------------------------------------------------------
// An abstract base class that represents either
// a field definition, condition, or repeat control.
//
class vil_nitf2_field_definition_node
{
public:
  enum node_type { type_field, type_repeat };
  vil_nitf2_field_definition_node(node_type type) : type(type) {}
  virtual ~vil_nitf2_field_definition_node() {}

  // Downcast test methods (for convenience)
  bool is_field_definition() const { return type==type_field; }
  bool is_repeat_node() const { return type==type_repeat; }

  // Downcast methods. Return 0 if conversion fails.
  vil_nitf2_field_definition* field_definition();
  vil_nitf2_field_definition_repeat_node* repeat_node();

  // Member variables
  node_type type;
};

//-----------------------------------------------------------------------------
// Represents the definition of a particular field 
// (including a contiguous set of repeating fields) of a tagged record 
// extension, including the name, format, repeat count, or other condition.
//
class vil_nitf2_field_definition : public vil_nitf2_field_definition_node
{
public:
  // These members basically correspond to columns within a row of
  // a NITF tagged record spec. An instance of this class corresponds
  // to a row (or a sequence of continguous repeating rows).
  vcl_string tag;
  vcl_string pretty_name;
  bool required;
  vil_nitf2_field_formatter* formatter;
  bool blanks_ok;
  vil_nitf2_field_functor<int>* width_functor;
  vil_nitf2_field_functor<bool>* condition_functor;
  vcl_string units;
  vcl_string description;

  bool is_required() const;
  bool is_variable_width() const;
  
  // Constructor
  vil_nitf2_field_definition(
    // field identifier, generally < 10 characters long
    vcl_string tag, 
    // the field name, typically a few words long
    vcl_string pretty_name, 
    // field type and format
    vil_nitf2_field_formatter* formatter,
    // whether this field may be unspecified (all blanks)
    bool blanks_ok = false,
    // function, when specified, that overrides formatter's field width.
    vil_nitf2_field_functor<int>* width_functor = 0,
    // conditional field predicate; 0 for required fields
    vil_nitf2_field_functor<bool>* condition_functor = 0,
    // additional documentation fields
    vcl_string units = "",
    vcl_string description = "");
};


//-----------------------------------------------------------------------------
// Represents the definition of a contiguous sequence of fields or nodes 
// containing other such sequences of fields. Methods field(), repeat(), 
// and cond() provide "syntactic sugar" for assembling the sequence.
//
class vil_nitf2_field_definitions : public vcl_list<vil_nitf2_field_definition_node*>
{
public:
  // Define a field and add it to this list of definitions, returning
  // the current list
  vil_nitf2_field_definitions& field(
    vcl_string tag,
    vcl_string pretty_name,
    vil_nitf2_field_formatter* formatter,
    // whether this field may be unspecified (all blank)
    bool blanks_ok = false,
    // function, when specified, that overrides formatter's field width
    vil_nitf2_field_functor<int>* width_functor = 0,
    // predicate that returns whether this conditional field is present;
    // 0 for required fields
    vil_nitf2_field_functor<bool>* condition_functor = 0,
    vcl_string units = "",
    vcl_string description = "");

  // Define a repeat node, with repeat count determined by repeat_functor,
  // and add it to this list of definitions, returning the current list.
  vil_nitf2_field_definitions& repeat(vil_nitf2_field_functor<int>* repeat_functor,
                                      vil_nitf2_field_definitions& field_definitions);

  // Same as above where the repeat count is simply the value of a tag.
  vil_nitf2_field_definitions& repeat(vcl_string intTag, 
                                      vil_nitf2_field_definitions& field_definitions) 
  { return repeat(new vil_nitf2_field_value<int>(intTag), field_definitions); }

  // Destructor
  virtual ~vil_nitf2_field_definitions() {}
};

//-----------------------------------------------------------------------------
// Represents a sequence of fields that is repeated. The repeat count
// is determined by a functor that evaluates previously processed tag(s).
//
class vil_nitf2_field_definition_repeat_node : public vil_nitf2_field_definition_node
{
public:
  // Construct a repeat node
  vil_nitf2_field_definition_repeat_node(vil_nitf2_field_functor<int>* repeat_functor,
                                         vil_nitf2_field_definitions* field_definitions) 
    : vil_nitf2_field_definition_node(type_repeat), 
      repeat_functor(repeat_functor),
      field_definitions(field_definitions) {}

  // Member variables
  vil_nitf2_field_functor<int>* repeat_functor;
  vil_nitf2_field_definitions* field_definitions;
};

#endif // VIL_NITF2_FIELD_DEFINITION_H
