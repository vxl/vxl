#include <vsl/vsl_vector_io.txx>
#include <vbl/vbl_triple.txx>
#include <vbl/io/vbl_io_triple.txx>
typedef vcl_vector<vbl_triple<double,int,int> > vector_triple_dii;
VSL_VECTOR_IO_INSTANTIATE(vector_triple_dii);
