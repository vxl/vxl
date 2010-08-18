#include <vbl/io/vbl_io_array_2d.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<unsigned char, 16> uchar16;
VBL_IO_ARRAY_2D_INSTANTIATE(uchar16);
