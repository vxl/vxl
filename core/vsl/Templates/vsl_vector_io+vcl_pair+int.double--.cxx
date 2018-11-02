#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_vector_io.hxx>
typedef std::pair<int,double> pair_id;
VSL_VECTOR_IO_INSTANTIATE(pair_id);
