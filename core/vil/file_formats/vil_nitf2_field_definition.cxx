// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <utility>
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_field_formatter.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//==============================================================================
// Class vil_nitf2_field_definition_node

vil_nitf2_field_definition*
vil_nitf2_field_definition_node::field_definition()
{
  return is_field_definition() ? (vil_nitf2_field_definition*)this : nullptr;
}

vil_nitf2_field_definition_repeat_node*
vil_nitf2_field_definition_node::repeat_node()
{
  return is_repeat_node() ? (vil_nitf2_field_definition_repeat_node*)this : nullptr;
}

//==============================================================================
// Class vil_nitf2_field_definition

vil_nitf2_field_definition::
vil_nitf2_field_definition(std::string tag,
                           std::string pretty_name,
                           vil_nitf2_field_formatter* formatter,
                           bool blanks_ok,
                           vil_nitf2_field_functor<int>* width_functor,
                           vil_nitf2_field_functor<bool>* condition_functor,
                           std::string units,
                           std::string description)
  : vil_nitf2_field_definition_node(type_field),
    tag(std::move(tag)),
    pretty_name(std::move(pretty_name)),
    formatter(formatter),
    blanks_ok(blanks_ok),
    width_functor(width_functor),
    condition_functor(condition_functor),
    units(std::move(units)),
    description(std::move(description))
{
  assert(!tag.empty() && "vil_nitf2_field_definition:: null tag");
  assert(formatter != nullptr && "vil_nitf2_field_definition:: null formatter");
}

vil_nitf2_field_definition_node* vil_nitf2_field_definition::copy() const
{
  return new vil_nitf2_field_definition(
    tag,
    pretty_name,
    formatter->copy(),
    blanks_ok,
    width_functor ? width_functor->copy() : nullptr,
    condition_functor ? condition_functor->copy() : nullptr,
    units,
    description);
}

bool vil_nitf2_field_definition::is_required() const
{
  return condition_functor == nullptr;
}

bool vil_nitf2_field_definition::is_variable_width() const
{
  return width_functor != nullptr;
}

vil_nitf2_field_definition::~vil_nitf2_field_definition()
{
  delete formatter;
  delete width_functor;
  delete condition_functor;
}

//==============================================================================
// Class vil_nitf2_field_definitions

vil_nitf2_field_definitions::vil_nitf2_field_definitions(
  const vil_nitf2_field_definitions& that)
  : std::list<vil_nitf2_field_definition_node*>()
{
  for (auto it : that)
  {
    push_back(it->copy());
  }
}


vil_nitf2_field_definitions& vil_nitf2_field_definitions::field(
    std::string tag,
    std::string pretty_name,
    vil_nitf2_field_formatter* formatter,
    bool blanks_ok,
    vil_nitf2_field_functor<int>* width_functor,
    vil_nitf2_field_functor<bool>* condition_functor,
    std::string units,
    std::string description)
{
  push_back(new vil_nitf2_field_definition(
                  std::move(tag), std::move(pretty_name), formatter, blanks_ok,
                  width_functor, condition_functor, std::move(units), std::move(description)));
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

vil_nitf2_field_definitions::~vil_nitf2_field_definitions()
{
  for (auto & it : *this)
  {
    delete it;
  }
}

//==============================================================================
// Class vil_nitf2_field_definition_repeat_node

vil_nitf2_field_definition_node*
vil_nitf2_field_definition_repeat_node::copy() const
{
  return new vil_nitf2_field_definition_repeat_node(
    repeat_functor->copy(),
    new vil_nitf2_field_definitions(*field_definitions));
};

vil_nitf2_field_definition_repeat_node::~vil_nitf2_field_definition_repeat_node()
{
  delete repeat_functor;
  delete field_definitions;
}
