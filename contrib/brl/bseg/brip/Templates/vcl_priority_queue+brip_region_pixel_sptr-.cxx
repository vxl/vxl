#include <brip/brip_region_pixel.h>
#include <vcl_queue.txx>

VCL_QUEUE_INSTANTIATE(brip_region_pixel_sptr);

VCL_PRIORITY_QUEUE_INSTANTIATE(vcl_vector<brip_region_pixel_sptr>, brip_region_pixel_sptr, brip_region_pixel::compare);


