#ifndef vcl_iso_cstdio_h_
#define vcl_iso_cstdio_h_

#include <cstdio>

// FILE
#ifndef vcl_FILE
#define vcl_FILE std::FILE
#endif
// fpos_t
#ifndef vcl_fpos_t
#define vcl_fpos_t std::fpos_t
#endif
// size_t
#ifndef vcl_size_t
#define vcl_size_t std::size_t
#endif
// clearerr
#ifndef vcl_clearerr
#define vcl_clearerr std::clearerr
#endif
// fclose
#ifndef vcl_fclose
#define vcl_fclose std::fclose
#endif
// feof
#ifndef vcl_feof
#define vcl_feof std::feof
#endif
// ferror
#ifndef vcl_ferror
#define vcl_ferror std::ferror
#endif
// fflush
#ifndef vcl_fflush
#define vcl_fflush std::fflush
#endif
// fgetc
#ifndef vcl_fgetc
#define vcl_fgetc std::fgetc
#endif
// fgetpos
#ifndef vcl_fgetpos
#define vcl_fgetpos std::fgetpos
#endif
// fgets
#ifndef vcl_fgets
#define vcl_fgets std::fgets
#endif
// ...
// printf
#ifndef vcl_printf
#define vcl_printf std::printf
#endif
// sprintf
#ifndef vcl_sprintf
#define vcl_sprintf std::sprintf
#endif
// fprintf
#ifndef vcl_fprintf
#define vcl_fprintf std::fprintf
#endif
// vprintf
#ifndef vcl_vprintf
#define vcl_vprintf std::vprintf
#endif
// vsprintf
#ifndef vcl_vsprintf
#define vcl_vsprintf std::vsprintf
#endif
// vfprintf
#ifndef vcl_vfprintf
#define vcl_vfprintf std::vfprintf
#endif

using std::FILE;
using std::fprintf;
using std::fflush;

#endif
