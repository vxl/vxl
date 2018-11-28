#include <vxl_config.h>

#if VXL_HAS_INT_64 && !( VXL_INT_64_IS_LONG || VXL_INT_IS_LONG_LONG )
#include <vsl/vsl_vector_io.hxx>
VSL_VECTOR_IO_INSTANTIATE(vxl_uint_64);


#endif // VXL_HAS_INT_64
