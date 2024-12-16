#include "vxl_config.h"

#if VXL_INT_64_IS_LONGLONG
#  include "vsl/vsl_vector_io.hxx"
VSL_VECTOR_IO_INSTANTIATE(vxl_int_64);
#else
/* Avoiding linker warnings like the following
/usr/bin/ranlib: file: lib/libvsl.a(vsl_vector_io+int_64-.cxx.o) has no symbols
/usr/bin/ranlib: file: lib/libvsl.a(vsl_vector_io+uint_64-.cxx.o) has no symbols
/usr/bin/ranlib: file: lib/libvsl.a(vsl_vector_io+int_64-.cxx.o) has no symbols
/usr/bin/ranlib: file: lib/libvsl.a(vsl_vector_io+uint_64-.cxx.o) has no symbols
*/
extern bool vxl_int_64_dummy_symbol_never_used_to_avoid_ranlib_no_symbols_warning;
bool vxl_int_64_dummy_symbol_never_used_to_avoid_ranlib_no_symbols_warning{ false };
#endif // VXL_HAS_INT_64
