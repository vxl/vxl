// vil_nitf2: Written by Rob Radtke (rob@) and Harry Voorhees (hlv@) of
// Stellar Science Ltd. Co. (stellarscience.com) for
// Air Force Research Laboratory, 2005.

#include "vil_nitf2_header.h"
#include "vil_nitf2_field_definition.h"
#include "vil_nitf2_image_subheader.h"

void vil_nitf2::cleanup_static_members() {
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
}
