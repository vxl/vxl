#include <brip/brip_region_pixel_sptr.h>
#include <vcl_vector.h>
#include <vcl_queue.txx>

VCL_PRIORITY_QUEUE_INSTANTIATE(vcl_vector<brip_region_pixel_sptr>, brip_region_pixel_sptr, brip_region_pixel::compare);
