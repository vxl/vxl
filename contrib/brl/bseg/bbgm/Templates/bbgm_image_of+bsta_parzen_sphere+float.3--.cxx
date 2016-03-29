#include <bbgm/bbgm_image_of.hxx>
#include <bsta/bsta_parzen_sphere.hxx>
#include <bsta/io/bsta_io_parzen_sphere.h>

typedef bsta_parzen_sphere<float,3> parzen;

BBGM_IMAGE_INSTANTIATE(parzen);
