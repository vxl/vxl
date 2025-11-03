#include <utility>
#include <vector>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_pair_io.hxx"
#include "vsl/vsl_vector_io.hxx"
#include <vbl/io/vbl_io_array_2d.hxx>
using vector_pair_ld = std::vector<std::pair<long, double>>;
VBL_IO_ARRAY_2D_INSTANTIATE(vector_pair_ld);
