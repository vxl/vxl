#include <vcl_compiler.h>
#include <utility>
#include <vsl/vsl_pair_io.hxx>
#include <vector>
#include <vsl/vsl_vector_io.hxx>
#include <vbl/io/vbl_io_array_2d.hxx>
typedef std::vector<std::pair<int,double> > vector_pair_id;
VBL_IO_ARRAY_2D_INSTANTIATE(vector_pair_id);
