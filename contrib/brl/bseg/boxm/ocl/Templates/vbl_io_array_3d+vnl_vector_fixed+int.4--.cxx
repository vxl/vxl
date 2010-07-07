#include <vbl/io/vbl_io_array_3d.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<int, 4> int4;
VBL_IO_ARRAY_3D_INSTANTIATE(int4);
