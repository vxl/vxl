#include "baml_birchfield_tomasi.cxx"
#include <vil/algo/vil_greyscale_erode.hxx>
#include <vil/algo/vil_greyscale_dilate.hxx>
VIL_GREYSCALE_ERODE_INSTANTIATE( vxl_uint_16 );
VIL_GREYSCALE_DILATE_INSTANTIATE( vxl_uint_16 );
BAML_COMPUTE_BIRCHFIELD_TOMASI_INSTANTIATE(vxl_uint_16);
