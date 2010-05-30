// This is brl/bbas/bsol/dll.h
#ifndef bsol_dll_h_
#define bsol_dll_h_

#include <vcl_compiler.h>

#define BSOL_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_BSOL_DLL)

// if win32 and not building the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BSOL_DLL_DATA
#  define BSOL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // bsol_dll_h_
