#include <vcl_map.txx>
#include <vcl_vector.h>
#include <sdet/sdet_region.h>

VCL_MAP_INSTANTIATE(sdet_region_sptr, vcl_vector<sdet_region_sptr>*,
                    sdet_region::compare);
