#include <bbgm/bbgm_image_of.txx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_indep.h>

typedef bsta_num_obs<bsta_gaussian_indep<float,3> > gauss_type;
BBGM_IMAGE_INSTANTIATE(gauss_type);
