#ifndef vbl_dll_h_
#define vbl_dll_h_
// This is vxl/vbl/dll.h

#include <vcl_compiler.h>

#define VBL_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_VBL_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef VBL_DLL_DATA
#  define VBL_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // vbl_dll_h_
