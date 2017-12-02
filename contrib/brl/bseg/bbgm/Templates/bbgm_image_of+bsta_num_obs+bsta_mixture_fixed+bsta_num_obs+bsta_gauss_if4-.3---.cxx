#include <bbgm/bbgm_image_of.hxx>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/io/bsta_io_attributes.h>
#include <bsta/io/bsta_io_mixture_fixed.h>
#include <bsta/io/bsta_io_gaussian_indep.h>
#include <bsta/bsta_gauss_if4.h>

typedef bsta_num_obs<bsta_gauss_if4> gauss_type;
typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;
BBGM_IMAGE_INSTANTIATE(mix_gauss_type);
