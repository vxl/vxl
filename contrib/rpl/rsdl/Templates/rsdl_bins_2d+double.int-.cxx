#include <iostream>
#include <vector>
#include <rsdl/rsdl_bins_2d.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_array_2d.hxx>

RSDL_BINS_2D_INSTANTIATE( double, int );

typedef rsdl_bins_2d_entry< double, int > bin_T;
typedef std::vector< bin_T > vector_T;
VBL_ARRAY_2D_INSTANTIATE( vector_T );
