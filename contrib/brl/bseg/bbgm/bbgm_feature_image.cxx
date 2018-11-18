#include "bbgm_feature_image.h"

void vsl_add_to_binary_loader(bbgm_feature_image_base const& b)
{
  vsl_binary_loader<bbgm_feature_image_base >::instance().add(b);
}
