#include <utility>
#include <vector>
#include <vcl_compiler.h>
#include <vsl/vsl_pair_io.hxx>
#include <vsl/vsl_vector_io.hxx>
#include <vbl/io/vbl_io_array_2d.hxx>
typedef std::vector<std::pair<long,double> > vector_pair_ld;
VBL_IO_ARRAY_2D_INSTANTIATE(vector_pair_ld);
