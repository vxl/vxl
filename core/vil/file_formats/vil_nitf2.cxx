// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2.h"

// (To enable debug logging, set value to vil_nitf2::log_debug)
vil_nitf2::enum_log_level vil_nitf2::s_log_level = vil_nitf2::log_none;

#include "vil_nitf2_header.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_image_subheader.h"

void vil_nitf2::cleanup_static_members() 
{
  if( vil_nitf2_header::s_field_definitions_1 ) {
    delete vil_nitf2_header::s_field_definitions_1;
    vil_nitf2_header::s_field_definitions_1 = 0;
  }

  if( vil_nitf2_header::s_field_definitions_20 ) {
    delete vil_nitf2_header::s_field_definitions_20;
    vil_nitf2_header::s_field_definitions_20 = 0;
  }

  if( vil_nitf2_header::s_field_definitions_21 ) {
    delete vil_nitf2_header::s_field_definitions_21;
    vil_nitf2_header::s_field_definitions_21 = 0;
  }

  if( vil_nitf2_image_subheader::s_field_definitions_20 ) {
    delete vil_nitf2_image_subheader::s_field_definitions_20;
    vil_nitf2_image_subheader::s_field_definitions_20 = 0;
  }

  if( vil_nitf2_image_subheader::s_field_definitions_21 ) {
    delete vil_nitf2_image_subheader::s_field_definitions_21;
    vil_nitf2_image_subheader::s_field_definitions_21 = 0;
  }

  for( vil_nitf2_classification::type_field_defs_map::iterator it = vil_nitf2_classification::s_field_definitions.begin() ;
       it != vil_nitf2_classification::s_field_definitions.end() ;
       it++ )
  {
    delete it->second;
  }
  vil_nitf2_classification::s_field_definitions.clear();
}
