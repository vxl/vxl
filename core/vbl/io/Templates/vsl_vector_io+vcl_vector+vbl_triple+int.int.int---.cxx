#include <vsl/vsl_vector_io.hxx>
#include <vbl/vbl_triple.hxx>
#include <vbl/io/vbl_io_triple.hxx>
typedef std::vector<vbl_triple<int,int,int> > vector_triple_iii;
VSL_VECTOR_IO_INSTANTIATE(vector_triple_iii);
