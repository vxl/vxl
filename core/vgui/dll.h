#ifndef vgui_DLLDATA_H
#define vgui_DLLDATA_H
#define vgui_DLLDATA

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#if defined(_WIN32) && !defined(BUILDING_VGUI_DLL)

// if win32 and not BUILDING_VGUI_DLL then you need a dllimport
// Only if you are building a DLL linked application.
#ifdef BUILD_DLL
#undef vgui_DLLDATA
#define vgui_DLLDATA _declspec(dllimport)
#endif // BUILD_DLL
#endif // _WIN32 && !BUILDING_VGUI_DLL

#endif // vgui_DLLDATA_H
