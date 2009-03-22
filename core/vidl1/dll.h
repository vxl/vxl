// This is core/vidl/dll.h
#ifndef vidl_dll_h_
#define vidl_dll_h_

#include <vcl_compiler.h>

#define VIDL_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_VIDL_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef VIDL_DLL_DATA
#  define VIDL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // vidl_dll_h_
