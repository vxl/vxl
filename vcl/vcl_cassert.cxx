/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_compiler.h>

#ifdef VCL_SUNPRO_CC
// fsm@robots: This is a silly hack to enable us to link code compiled with SunPro
// against a library (such as libMesaGL) compiled with gcc. The gcc assert macro
// uses a function call __eprintf() which is defined in gcc/libgcc2.c in the gcc
// sources.
#include <vcl/vcl_cstdio.h>
#include <vcl/vcl_cstdlib.h>
extern "C" void
__eprintf (char const *string, char const *expression,
	   unsigned int line, char const *filename)
{
  fprintf (stderr, string, expression, line, filename);
  fflush (stderr);
  vcl_abort ();
}
#endif
