#include <rsdl/rsdl_bins_2d.txx>
#include <vcl_compiler.h>
#include <iostream>
#include <vector>
#include <vbl/vbl_array_2d.hxx>

RSDL_BINS_2D_INSTANTIATE(double, float);

typedef rsdl_bins_2d_entry<double, float> bin_T;
typedef std::vector<bin_T> vector_T;
VBL_ARRAY_2D_INSTANTIATE(vector_T);
