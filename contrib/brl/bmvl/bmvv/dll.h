// This is brl/bmvl/bmvv/dll.h
#ifndef bmvv_dll_h_
#define bmvv_dll_h_

#include <vcl_compiler.h>

#define BMVV_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_BMVV_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BMVV_DLL_DATA
#  define BMVV_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // bmvv_dll_h_
