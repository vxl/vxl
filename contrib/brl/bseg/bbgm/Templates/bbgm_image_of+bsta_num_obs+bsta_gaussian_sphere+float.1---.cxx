#include <bbgm/bbgm_image_of.txx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_sphere.h>

typedef bsta_num_obs<bsta_gaussian_sphere<float,1> > gauss_type;
BBGM_IMAGE_INSTANTIATE(gauss_type);
