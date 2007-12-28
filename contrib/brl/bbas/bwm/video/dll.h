// This is brl/bbas/bwm/video/dll.h
#ifndef bwm_video_dll_h_
#define bwm_video_dll_h_

#include <vcl_compiler.h>
//this stuff is necessary to export static symbols from a library
#define BWM_VIDEO_DLL_DATA

#if defined(VCL_WIN32) && !defined(BUILDING_BWM_VIDEO_DLL)

// if win32 and not buiding the DLL then you need a dllimport
// Only if you are building a DLL linked application.
# ifdef BUILD_DLL
#  undef BWM_VIDEO_DLL_DATA
#  define BWM_VIDEO_DLL_DATA _declspec(dllimport)
# endif // BUILD_DLL
#endif // VCL_WIN32 and !Building_*_dll

#endif // bwm_video_dll_h_
