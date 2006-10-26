// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#ifndef VIL_NITF2_TAGGED_RECORD_DEFINITION_H
#define VIL_NITF2_TAGGED_RECORD_DEFINITION_H

#include <vcl_map.h>
#include <vcl_string.h>

#include "vil_nitf2_field_functor.h"

class vil_nitf2_field_formatter;
class vil_nitf2_field_definition;
class vil_nitf2_field_definitions;
template<typename T> class vil_nitf2_field_functor;

//-----------------------------------------------------------------------------
// vil_nitf2_tagged_record_definition defines a particular tagged record extension 
// (TRE). It consists of its name and an ordered list of vil_nitf2_field_definitions.
// It also defines a static method to look up an TRE defintion by name.
// 
// The primary goal of this class design to provide a succinct way for the 
// programmer to specify a NITF tagged record definition, which can be used
// for both reading and writing the record. The definition must support 
// references to values of other fields in the record, which are needed to
// define conditional and repeating fields. Please see the example definition 
// in method test().

class vil_nitf2_tagged_record_definition
{
  friend class vil_nitf2_tagged_record;
public:

  // Factory method. Assumes ownership of optional pointer argument.
  static vil_nitf2_tagged_record_definition& define(
    vcl_string name, vcl_string pretty_name);

  // Define a field. Assumes ownership of pointer arguments.
  vil_nitf2_tagged_record_definition& field(
    vcl_string field_name,
    vcl_string pretty_name,
    vil_nitf2_field_formatter* formatter,
    // whether this field may be unspecified (all blank)
    bool blanks_ok = false,
    // function, when specified, that overrides formatter's width
    vil_nitf2_field_functor<int>* width_functor = 0,
    // predicate that returns whether this conditional field is present;
    // 0 for required fields
    vil_nitf2_field_functor<bool>* condition_functor = 0,
    vcl_string units = "",
    vcl_string description = "");
 
  // Define a repeat node. Assumes ownership of pointer argument.
  vil_nitf2_tagged_record_definition& repeat(
    vil_nitf2_field_functor<int>* repeat_functor,
    vil_nitf2_field_definitions& field_definitions);

  // Convenience overload where repeat count is simply the value of a tag.
  vil_nitf2_tagged_record_definition& repeat(
    vcl_string int_tag,
    vil_nitf2_field_definitions& field_definitions);

  // Convenience overload where repeat count is a fixed value.
  vil_nitf2_tagged_record_definition& repeat(
    int repeat_count,
    vil_nitf2_field_definitions& field_definitions);

  // Declares that definition is finished, preventing further invocations
  // of field() or repeat().
  void end();
    
  // Look up a record definition
  static vil_nitf2_tagged_record_definition* find(vcl_string name);

  // Look up a field definition
  vil_nitf2_field_definition* find_field(vcl_string name);

  // Destructor
  ~vil_nitf2_tagged_record_definition();

  // All tagged record definitions
  typedef vcl_map<vcl_string, vil_nitf2_tagged_record_definition*> 
    tagged_record_definition_map;
  static tagged_record_definition_map& all_definitions();

  // Return field definitions
  const vil_nitf2_field_definitions& field_definitions() const {
    return *m_field_definitions; }

  // Undefines a TRE. Returns whether TRE with specified name was found.
  static bool undefine(vcl_string name);

  // Registers some TREs for testing
  static void register_test_tre();

private:
  // No copy constructor
  vil_nitf2_tagged_record_definition(const vil_nitf2_tagged_record_definition&);

  // No assignment operator
  vil_nitf2_tagged_record_definition& operator=(const vil_nitf2_tagged_record_definition&);

  // Constructor
  vil_nitf2_tagged_record_definition(vcl_string name, vcl_string pretty_name, 
    vil_nitf2_field_definitions* defs = 0); 
  
  // to implement
  //virtual bool validate(const vil_nitf2_tagged_record*) const;

  vcl_string m_name;
  vcl_string m_pretty_name;
  vil_nitf2_field_definitions* m_field_definitions;
  bool m_definition_completed;
};

#endif // VIL_NITF2_TAGGED_RECORD_DEFINITION_H
