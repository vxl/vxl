#ifndef vgui_DLLDATA_H
#define vgui_DLLDATA_H
#define vgui_DLLDATA 

#include <vcl_compiler.h>

#ifdef VCL_WIN32
#ifndef BUILDING_vgui_DLL
// if win32 and not BUILDING_COOL_DLL then you need a dllimport 
// Only if you are building a DLL linked application.
#ifdef BUILD_DLL
#undef vgui_DLLDATA
#define vgui_DLLDATA _declspec(dllimport)
#endif // BUILD_DLL
#endif // BUILDING_SPATIAL_OBJECT_DLL
#endif // VCL_WIN32

#endif // vgui_DLLDATA_H
