#include <vcl_utility.h>
#include <vsl/vsl_pair_io.hxx>
#include <vcl_vector.h>
#include <vsl/vsl_vector_io.hxx>
#include <vbl/io/vbl_io_array_2d.hxx>
typedef vcl_vector<vcl_pair<int,float> > vector_pair_id;
VBL_IO_ARRAY_2D_INSTANTIATE(vector_pair_id);
