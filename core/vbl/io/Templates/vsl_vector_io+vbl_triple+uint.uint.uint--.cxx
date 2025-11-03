#include "vsl/vsl_vector_io.hxx"
#include "vbl/vbl_triple.hxx"
#include <vbl/io/vbl_io_triple.hxx>
using vbl_triple_iii = vbl_triple<unsigned int, unsigned int, unsigned int>;
VSL_VECTOR_IO_INSTANTIATE(vbl_triple_iii);
using vector_triple_uuu = std::vector<vbl_triple_iii>;
VSL_VECTOR_IO_INSTANTIATE(vector_triple_uuu);
