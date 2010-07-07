#include <vbl/io/vbl_io_array_1d.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<int, 2> int2;
VBL_IO_ARRAY_1D_INSTANTIATE(int2);
