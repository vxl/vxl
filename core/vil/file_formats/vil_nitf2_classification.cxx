// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for 
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_classification.h"

#include "vil_nitf2_tagged_record.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_typed_field_formatter.h"

#include <vcl_cassert.h>

vil_nitf2_classification::type_field_defs_map vil_nitf2_classification::s_field_definitions;

const vil_nitf2_field_definitions* vil_nitf2_classification::
get_field_definitions(const file_version& version, 
                      vcl_string tag_prefix, vcl_string pretty_name_prefix)
{
  vil_nitf2_field_definitions* field_defs = 0;
  if (version == V_NITF_20 || version == V_NITF_21) {
    type_field_defs_key key = 
      vcl_make_pair(version, vcl_make_pair(tag_prefix, pretty_name_prefix));
    type_field_defs_map::const_iterator map_entry = s_field_definitions.find(key);
    if (map_entry != s_field_definitions.end()) {
      field_defs = map_entry->second;
    } else {
      field_defs = new vil_nitf2_field_definitions();
      add_field_defs(field_defs, version, tag_prefix, pretty_name_prefix);
      s_field_definitions.insert(vcl_make_pair(key, field_defs));
    }
  } else {
    vcl_cerr << "vil_nitf2_classification::get_field_definitions() called with unsupported version!" << vcl_endl;
    assert(0);
  }
  return field_defs;
}
  
void vil_nitf2_classification::
add_field_defs(vil_nitf2_field_definitions* defs, const file_version& version,
               vcl_string tag_prefix, vcl_string pretty_name_prefix)
{
  const vcl_string& tp = tag_prefix;
  const vcl_string np = pretty_name_prefix + " ";
  switch (version) {
  case V_NITF_20:
    (*defs) 
    .field(tp+"SCLAS", np+"Security Classification", 
           NITF_ENUM(1, vil_nitf2_enum_values()
             .value("T", "Top Secret")
             .value("S", "Secret")
             .value("C", "Confidential")
             .value("R", "Restricted")
             .value("U", "Unclassified")),
            false, 0, 0)
    .field(tp+"SCODE", np+"Codewords",                NITF_STR_ECSA(40), true, 0, 0)   
    .field(tp+"SCTLH", np+"Control and Handling",     NITF_STR_ECSA(40), true, 0, 0)   
    .field(tp+"SREL",  np+"Releasing Instructions",   NITF_STR_ECSA(40), true, 0, 0)   
    .field(tp+"SCAUT", np+"Classification Authority", NITF_STR_ECSA(20), true, 0, 0)   
    .field(tp+"SCTLN", np+"Security Control Number",  NITF_STR_ECSA(20), true, 0, 0)   
    .field(tp+"SDWNG", np+"Security Downgrade",       NITF_STR_ECSA(6),  true, 0, 0)   
    .field(tp+"SDEVT", np+"Downgrading Event",        NITF_STR_ECSA(40), true, 0, 
           new vil_nitf2_field_value_one_of<vcl_string>(tp+"SDWNG", "999998"));
    break;
  case V_NITF_21:
    (*defs)
      .field(tp+"SCLAS", np+"Security Classification", 
             NITF_ENUM(1, vil_nitf2_enum_values()
               .value("T", "Top Secret")
               .value("S", "Secret")
               .value("C", "Confidential")
               .value("R", "Restricted")
               .value("U", "Unclassified")),
              false, 0, 0)
    .field(tp+"SCLSY", np+"Security Classification System", NITF_STR_ECSA(2),  true, 0, 0)
    .field(tp+"SCODE", np+"Codewords",                      NITF_STR_ECSA(11), true, 0, 0)
    .field(tp+"SCTLH", np+"Control and Handling",           NITF_STR_ECSA(2),  true, 0, 0)
    .field(tp+"SREL",  np+"Releasing Instructions",         NITF_STR_ECSA(20), true, 0, 0)
    //TODO: should this be an enum that accepts only DD, DE, GD, GE, 0, X and <blank>?  
    .field(tp+"SDCTP", np+"Declassification Type",          NITF_STR_ECSA(2),  true, 0, 0)
    //this field def is only used for NITF2.1, so we can use NitfDT
    .field(tp+"SDCDT", np+"Declassification Date",          NITF_DAT(8),       true, 0, 0)
    //TODO: Should this instead be a NitfEnum that accepts only [X1-X8] and [x251-X259]??
    .field(tp+"SDCXM", np+"Declassification Exemption",     NITF_STR_ECSA(4),  true, 0, 0)
    .field(tp+"SDG",   np+"Downgrade", 
           NITF_ENUM(1, vil_nitf2_enum_values()
             .value("S", "Secret")
             .value("C", "Confidential")
             .value("R", "Restricted")),
            true, 0, 0)
    //this field def is only used for NITF2.1, so we can use NitfDT
    .field(tp+"SDGDT", np+"Downgrade Date",                 NITF_DAT(8),       true, 0, 0)
    .field(tp+"SCLTX", np+"Classification Text",            NITF_STR_ECSA(43), true, 0, 0)
    .field(tp+"SCATP", np+"Classification Authority Type", 
           NITF_ENUM(1, vil_nitf2_enum_values()
             .value("O", "Original classification authority")
             .value("D", "Derivative from a single source")
             .value("M", "Derivative from multiple sources")),
            true, 0, 0)
    .field(tp+"SCAUT", np+"Classification Authority",       NITF_STR_ECSA(40), true, 0, 0)
    .field(tp+"SCRSN", np+"Classification Reason", 
           NITF_ENUM(1, vil_nitf2_enum_values()
             .value("A", "Military plans, weapons systems, or operations")
             .value("B", "Foreign government information")
             .value("C", "Intelligence activities (including special activities), intelligence sources or methods, or cryptology")
             .value("D", "Foreign relations or foreign activities of the United States, including confidential sources")
             .value("E", "Scientific, technological, or economic matters relating to national security")
             .value("F", "United States Government programs for safeguarding nuclear materials or facilities")
             .value("G", "Vulnerabilities or capabilities of systems, installations, projects or plans relating to the national security")),
            true, 0, 0)
    //order of data/time depends on NITF2.1 or NITF2.0, so just read in as string for now
    .field(tp+"SSRDT", np+"Security Source Date",           NITF_DAT(8),       true, 0, 0)
    .field(tp+"SCTLN", np+"Security Control Number",        NITF_STR_ECSA(15), true, 0, 0);
    break;
  default:
    assert(0);
  }
}