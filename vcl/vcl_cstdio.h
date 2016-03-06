#ifndef vcl_cstdio_h_
#define vcl_cstdio_h_

#include "vcl_compiler.h"
#include <cstdio>
/* The following includes are needed to preserve backwards
   compatilibility for external applications.  Previously
   definitions were defined in multiple headers with conditional
   ifndef guards, but we now include a reference header
   instead */
#include "vcl_cstddef.h"
//vcl alias names to std names
// [27.8.2.1]
//
// macros:
//   BUFSIZ        FOPEN_MAX SEEK_CUR TMP_MAX _IONBF stdout
//   EOF           L_tmpnam  SEEK_END _IOFBF  stderr
//   FILENAME_MAX  NULL      SEEK_SET _IOLBF  stdin

#define vcl_FILE std::FILE
#define vcl_fpos_t std::fpos_t
// NB: size_t is declared in <cstddef>, not <cstdio>
#define vcl_fopen std::fopen
#define vcl_fclose std::fclose
#define vcl_feof std::feof
#define vcl_ferror std::ferror
#define vcl_fflush std::fflush
#define vcl_fgetc std::fgetc
#define vcl_fgetpos std::fgetpos
#define vcl_fgets std::fgets
#define vcl_fwrite std::fwrite
#define vcl_fread std::fread
#define vcl_fseek std::fseek
#define vcl_ftell std::ftell
#define vcl_perror std::perror
#define vcl_clearerr std::clearerr
#define vcl_rename std::rename
#define vcl_fputc std::fputc
#define vcl_fputs std::fputs
#define vcl_freopen std::freopen
#define vcl_fsetpos std::fsetpos
#define vcl_getc std::getc
#define vcl_getchar std::getchar
#define vcl_gets std::gets
#define vcl_putc std::putc
#define vcl_putchar std::putchar
#define vcl_puts std::puts
#define vcl_remove std::remove
#define vcl_rewind std::rewind
#define vcl_setbuf std::setbuf
#define vcl_setvbuf std::setvbuf
#define vcl_tmpfile std::tmpfile
#define vcl_tmpnam std::tmpnam
#define vcl_ungetc std::ungetc

// printf() family
#define vcl_printf std::printf
#define vcl_sprintf std::sprintf
#define vcl_snprintf std::snprintf
#define vcl_fprintf std::fprintf
#define vcl_vprintf std::vprintf
#define vcl_vsprintf std::vsprintf
#define vcl_vfprintf std::vfprintf

// scanf() family
#define vcl_scanf std::scanf
#define vcl_sscanf std::sscanf
#define vcl_fscanf std::fscanf
#define vcl_vscanf std::vscanf
#define vcl_vsscanf std::vsscanf
#define vcl_vfscanf std::vfscanf

#endif // vcl_cstdio_h_
