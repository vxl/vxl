// This is brl/bseg/segv/dll.h
#ifndef segv_dll_h_
#define segv_dll_h_

#include <vcl_compiler.h>

#define SEGV_VIL_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_SEGV_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef SEGV_VIL_DLL_DATA
#  define SEGV_VIL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // segv_dll_h_
