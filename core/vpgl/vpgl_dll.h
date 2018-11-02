// This is vpgl/vpgl_dll.h
#ifndef vpgl_dll_h_
#define vpgl_dll_h_

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define VPGL_DLL_DATA

#if defined(_WIN32) && !defined(BUILDING_VPGL_DLL)

// if win32 and not building the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef VPGL_DLL_DATA
#  define VPGL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // _WIN32 and !Building_*_dll

#endif // vpgl_dll_h_
