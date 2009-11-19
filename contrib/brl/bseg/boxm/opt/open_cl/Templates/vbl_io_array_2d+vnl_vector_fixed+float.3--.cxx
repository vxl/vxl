#include <vbl/io/vbl_io_array_2d.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<float, 3> v3d;
VBL_IO_ARRAY_2D_INSTANTIATE(v3d);
