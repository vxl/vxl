#include <vsl/vsl_vector_io.hxx>
#include <vbl/vbl_triple.hxx>
#include <vbl/io/vbl_io_triple.hxx>
typedef vbl_triple<unsigned int, unsigned int, unsigned int> vbl_triple_iii;
VSL_VECTOR_IO_INSTANTIATE(vbl_triple_iii);
typedef std::vector<vbl_triple_iii>  vector_triple_uuu;
VSL_VECTOR_IO_INSTANTIATE(vector_triple_uuu);
