// This is brl/bbas/bxml/dll.h
#ifndef bxml_dll_h_
#define bxml_dll_h_

#include <vcl_compiler.h>

#define BXML_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_BXML_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BXML_DLL_DATA
#  define BXML_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // bxml_dll_h_
