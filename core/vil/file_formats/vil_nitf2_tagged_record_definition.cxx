// vil_nitf2: Written by Harry Voorhees (hlv@) and Rob Radtke (rob@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include <iostream>
#include <utility>
#include "vil_nitf2_tagged_record_definition.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


vil_nitf2_tagged_record_definition::tagged_record_definition_map&
  vil_nitf2_tagged_record_definition::all_definitions()
{
  class tagged_record_definition_map_t: public vil_nitf2_tagged_record_definition::tagged_record_definition_map
  {
   public:
    ~tagged_record_definition_map_t()
    {
      for (auto & it : *this) {
        delete it.second;
      }
    }
  };

  static tagged_record_definition_map_t tagged_record_definitions;
  return tagged_record_definitions;
}


vil_nitf2_tagged_record_definition::vil_nitf2_tagged_record_definition(
  std::string name, std::string pretty_name, vil_nitf2_field_definitions* defs)
  : m_name(std::move(name)),
    m_pretty_name(std::move(pretty_name)),
    m_field_definitions(defs ? defs : new vil_nitf2_field_definitions()),
    m_definition_completed(false)
{
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::define(
  std::string name, std::string pretty_name)
{
  vil_nitf2_tagged_record_definition* definition =
    new vil_nitf2_tagged_record_definition(name, pretty_name);
  if (all_definitions().find(name) != all_definitions().end()) {
    throw("vil_nitf2_tagged_record_definition already defined.");
  }
  all_definitions().insert(std::make_pair(name, definition));
  return *definition;
}

bool vil_nitf2_tagged_record_definition::undefine(std::string name)
{
  auto definition = all_definitions().find(name);
  if (definition == all_definitions().end()) {
    return false;
  }
  delete definition->second;
  all_definitions().erase(definition);
  return true;
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::field(
    std::string tag,
    std::string pretty_name,
    vil_nitf2_field_formatter* formatter,
    bool blanks_ok,
    vil_nitf2_field_functor<int>* width_functor,
    vil_nitf2_field_functor<bool>* condition_functor,
    std::string units,
    std::string description)
{
  if (m_definition_completed) {
    std::cerr << "vil_nitf2_tagged_record_definition:field() failed; definition already complete.";
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
    std::cerr << "vil_nitf2_tagged_record_definition:repeat() failed; definition already complete.";
  } else {
    m_field_definitions->push_back(
      new vil_nitf2_field_definition_repeat_node(
        repeat_functor,
        new vil_nitf2_field_definitions(field_definitions)));
  }
  return *this;
}

void vil_nitf2_tagged_record_definition::end()
{
  m_definition_completed = true;
}

vil_nitf2_tagged_record_definition* vil_nitf2_tagged_record_definition::find(std::string name)
{
  auto definition = all_definitions().find(name);
  if (definition == all_definitions().end()) return nullptr;
  return definition->second;
}

vil_nitf2_tagged_record_definition::~vil_nitf2_tagged_record_definition()
{
  delete m_field_definitions;
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::repeat(
  std::string int_tag, vil_nitf2_field_definitions& field_definitions)
{
  return repeat(new vil_nitf2_field_value<int>(int_tag), field_definitions);
}

vil_nitf2_tagged_record_definition& vil_nitf2_tagged_record_definition::repeat(
  int repeat_count, vil_nitf2_field_definitions& field_definitions)
{
  return repeat(new vil_nitf2_constant_functor<int>(repeat_count), field_definitions);
}

void vil_nitf2_tagged_record_definition::register_test_tre()
{
  define("PIAIMB", "Profile for Imagery Archives Image" )
    .field("CLOUDCVR",   "Cloud Cover",            NITF_INT(3),  true)
    .field("SRP",        "Standard Radiometric Product", NITF_CHAR(), true)
    .field("SENSMODE",   "Sensor Mode",            NITF_STR(12), true)
    .field("SENSNAME",   "Sensor Name",            NITF_STR(18), true)
    .field("SOURCE",     "Source",                 NITF_STR(255), true)
    .field("COMGEN",     "Compression Generation", NITF_INT(2), true)
    .field("SUBQUAL",    "Subjective Quality",     NITF_CHAR(), true)
    .field("PIAMSNNUM",  "PIA Mission Number",     NITF_STR(7), true)
    .field("CAMSPECS",   "Camera Specs",           NITF_STR(32), true)
    .field("PROJID",     "Project ID Code",        NITF_STR(2), true)
    .field("GENERATION", "Generation",             NITF_INT(1), true)
    .field("ESD",        "Exploitation Support Data", NITF_CHAR(), true)
    .field("OTHERCOND",  "Other Conditions",       NITF_STR(2), true)
    .end();
}
