#ifndef vgui_DLLDATA_H
#define vgui_DLLDATA_H
#define vgui_DLLDATA 

#include <vcl_compiler.h>

#if defined(VCL_WIN32) && !defined(BUILDING_VGUI_DLL)

// if win32 and not BUILDING_VGUI_DLL then you need a dllimport 
// Only if you are building a DLL linked application.
#ifdef BUILD_DLL
#undef vgui_DLLDATA
#define vgui_DLLDATA _declspec(dllimport)
#endif // BUILD_DLL
#endif // VCL_WIN32 && !BUILDING_VGUI_DLL

#endif // vgui_DLLDATA_H
