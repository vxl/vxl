#include <utility>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_pair_io.h"
#include "vsl/vsl_vector_io.hxx"
using vector_pair_uf = std::vector<std::pair<unsigned int, float>>;
VSL_VECTOR_IO_INSTANTIATE(vector_pair_uf);
