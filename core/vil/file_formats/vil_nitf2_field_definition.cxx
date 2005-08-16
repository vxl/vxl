// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_field_formatter.h"

#include <vcl_cassert.h>

//==============================================================================
// Class vil_nitf2_field_definition_node

vil_nitf2_field_definition* 
vil_nitf2_field_definition_node::field_definition() 
{ 
  return is_field_definition() ? (vil_nitf2_field_definition*)this : 0; 
}

vil_nitf2_field_definition_repeat_node* 
vil_nitf2_field_definition_node::repeat_node() 
{ 
  return is_repeat_node() ? (vil_nitf2_field_definition_repeat_node*)this : 0; 
}

//==============================================================================
// Class vil_nitf2_field_definition

vil_nitf2_field_definition::
vil_nitf2_field_definition(vcl_string tag,
                    vcl_string pretty_name,
                    vil_nitf2_field_formatter* formatter,
                    bool blanks_ok,
                    vil_nitf2_field_functor<int>* width_functor,
                    vil_nitf2_field_functor<bool>* condition_functor,
                    vcl_string units,
                    vcl_string description)
  : vil_nitf2_field_definition_node(type_field),
    tag(tag), 
    pretty_name(pretty_name),
    formatter(formatter),
    blanks_ok(blanks_ok),
    width_functor(width_functor),
    condition_functor(condition_functor),
    units(units), 
    description(description)
{
  assert(!tag.empty() && "vil_nitf2_field_definition:: null tag");
  assert(formatter != 0 && "vil_nitf2_field_definition:: null formatter");
}

bool vil_nitf2_field_definition::is_required() const {
  return condition_functor == 0;
}

bool vil_nitf2_field_definition::is_variable_width() const {
  return width_functor != 0;
}

//==============================================================================
// Class vil_nitf2_field_definitions

vil_nitf2_field_definitions& vil_nitf2_field_definitions::field(
    vcl_string tag,
    vcl_string pretty_name,
    vil_nitf2_field_formatter* formatter,
    bool blanks_ok,
    vil_nitf2_field_functor<int>* width_functor,
    vil_nitf2_field_functor<bool>* condition_functor,
    vcl_string units,
    vcl_string description)
{
  push_back(new vil_nitf2_field_definition(
                  tag, pretty_name, formatter, blanks_ok, 
                  width_functor, condition_functor, units, description));
  return *this;
}

vil_nitf2_field_definitions& 
vil_nitf2_field_definitions::repeat(vil_nitf2_field_functor<int>* repeat_functor, 
                                    vil_nitf2_field_definitions& field_definitions ) 
{
  push_back(new vil_nitf2_field_definition_repeat_node( 
                  repeat_functor, 
                  new vil_nitf2_field_definitions(field_definitions)));
  return *this;
}


