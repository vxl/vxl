#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_vector_io.hxx>
typedef std::vector<std::pair<unsigned int,unsigned int> > vector_pair_uu;
VSL_VECTOR_IO_INSTANTIATE(vector_pair_uu);
