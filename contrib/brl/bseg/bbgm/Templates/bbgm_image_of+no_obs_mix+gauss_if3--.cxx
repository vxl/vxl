#include <bbgm/bbgm_image_of.txx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_mixture.h>
#include <bsta/io/bsta_io_gaussian_indep.h>

typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
typedef bsta_mixture<gauss_type> mix_gauss_type;
BBGM_IMAGE_INSTANTIATE(mix_gauss_type);

