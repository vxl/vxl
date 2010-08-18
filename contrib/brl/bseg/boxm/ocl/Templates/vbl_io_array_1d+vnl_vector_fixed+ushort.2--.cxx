#include <vbl/io/vbl_io_array_1d.txx>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/io/vnl_io_vector_fixed.h>
typedef vnl_vector_fixed<unsigned short, 2> ushort2;
VBL_IO_ARRAY_1D_INSTANTIATE(ushort2);
