#include <bbgm/bbgm_image_of.h>


void vsl_add_to_binary_loader(bbgm_image_base const& b)
{
  vsl_binary_loader<bbgm_image_base >::instance().add(b);
}

