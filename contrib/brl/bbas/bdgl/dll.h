// This is vxl/brl/bbas/bdgl/dll.h
#ifndef bdgl_dll_h_
#define bdgl_dll_h_

#include <vcl_compiler.h>

#define BDGL_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_BDGL_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BDGL_DLL_DATA
#  define BDGL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // bdgl_dll_h_
