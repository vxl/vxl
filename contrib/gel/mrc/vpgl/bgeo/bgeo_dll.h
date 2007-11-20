// This is brl/bbas/bgeo/bgeo_dll.h
#ifndef geo_dll_h_
#define geo_dll_h_

#include <vcl_compiler.h>

#define GEO_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_GEO_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef GEO_DLL_DATA
#  define GEO_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // geo_dll_h_
