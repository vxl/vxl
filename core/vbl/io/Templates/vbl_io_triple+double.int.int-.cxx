// Instantiate io for vbl_triple(double,int,int)
#include <vsl/vsl_vector_io.hxx>
#include <vbl/vbl_triple.hxx>
#include <vbl/io/vbl_io_triple.hxx>
VBL_IO_TRIPLE_INSTANTIATE(double,int,int);
typedef vbl_triple<double, int, int> vbl_triple_dii;
VSL_VECTOR_IO_INSTANTIATE(vbl_triple_dii);
typedef std::vector<vbl_triple<double, int, int> > vector_triple_dii;
VSL_VECTOR_IO_INSTANTIATE(vector_triple_dii);
