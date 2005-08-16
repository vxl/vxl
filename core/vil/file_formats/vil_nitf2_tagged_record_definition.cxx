// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_tagged_record_definition.h"
#include "vil_nitf2_field_definition.h"
#include <vcl_iostream.h>
#include <vcl_utility.h>

vil_nitf2_tagged_record_definition::tagged_record_definition_map 
vil_nitf2_tagged_record_definition::all_definitions;

vil_nitf2_tagged_record_definition::vil_nitf2_tagged_record_definition(vcl_string name) 
  : m_name(name), 
    m_field_definitions(new vil_nitf2_field_definitions),
    m_definition_completed(false)
{
}

vil_nitf2_tagged_record_definition&
vil_nitf2_tagged_record_definition::define(vcl_string name)
{
  vil_nitf2_tagged_record_definition* definition = new vil_nitf2_tagged_record_definition(name);
  if (all_definitions.find(name) != all_definitions.end()) {
    throw("vil_nitf2_tagged_record_definition already defined.");
  }
  all_definitions.insert(vcl_make_pair(name, definition));
  return *definition;
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::field(
    vcl_string tag,
    vcl_string pretty_name,
    vil_nitf2_field_formatter* formatter,
    bool blanks_ok,
    vil_nitf2_field_functor<int>* width_functor,
    vil_nitf2_field_functor<bool>* condition_functor,
    vcl_string units,
    vcl_string description)
{
  if (m_definition_completed) {
    vcl_cerr << "vil_nitf2_tagged_record_definition:field() failed; definition already complete.";
  } else {
    vil_nitf2_field_definition* field_definition = new vil_nitf2_field_definition(
      tag, pretty_name, formatter, blanks_ok, 
      width_functor, condition_functor, units, description);
    m_field_definitions->push_back(field_definition);
  }
  return *this;
}

vil_nitf2_tagged_record_definition& 
vil_nitf2_tagged_record_definition::repeat(vil_nitf2_field_functor<int>* repeat_functor,
                                   vil_nitf2_field_definitions& field_definitions)
{
  if (m_definition_completed) {
    vcl_cerr << "vil_nitf2_tagged_record_definition:repeat() failed; definition already complete.";
  } else {
    m_field_definitions->push_back(
      new vil_nitf2_field_definition_repeat_node(
        repeat_functor, 
        new vil_nitf2_field_definitions(field_definitions)));
  }
  return *this;
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::end() 
{ 
  m_definition_completed = true; 
  return *this; 
}

vil_nitf2_tagged_record_definition* vil_nitf2_tagged_record_definition::find(vcl_string name)
{
  tagged_record_definition_map::iterator definition = all_definitions.find(name);
  if (definition == all_definitions.end()) return 0;
  return definition->second;
}
