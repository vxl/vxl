// This is brl/bbas/btol/dll.h
#ifndef btol_dll_h_
#define btol_dll_h_

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define BTOL_DLL_DATA

#if defined(_WIN32) && !defined(BUILDING_BTOL_DLL)

// if win32 and not building the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BTOL_DLL_DATA
#  define BTOL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // _WIN32 and !Building_*_dll

#endif // btol_dll_h_
