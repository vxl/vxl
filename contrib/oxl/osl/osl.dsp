# Microsoft Developer Studio Project File - Name="osl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=osl - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "osl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "osl.mak" CFG="osl - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "osl - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "osl - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "osl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\osl.lib"

!ELSEIF  "$(CFG)" == "osl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=xilink6.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\osl.lib"

!ELSEIF  "$(CFG)" == "osl - Win32 StaticDebug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "StaticDebug"
# PROP BASE Intermediate_Dir "StaticDebug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "StaticDebug"
# PROP Intermediate_Dir "StaticDebug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /Ob2 /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\StaticDebug\osl.lib"

!ELSEIF  "$(CFG)" == "osl - Win32 StaticRelease"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "StaticRelease"
# PROP BASE Intermediate_Dir "StaticRelease"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "StaticRelease"
# PROP Intermediate_Dir "StaticRelease"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\oxl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32
# ADD LIB32 /nologo /out:"..\StaticRelease\osl.lib"

!ENDIF 

# Begin Target

# Name "osl - Win32 Release"
# Name "osl - Win32 Debug"
# Name "osl - Win32 StaticDebug"
# Name "osl - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\internals\droid.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+osl_edgel_chain~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+osl_edge~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_list+osl_vertex~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+osl_LINK~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+osl_edge~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vcl_vector+osl_vertex~-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\osl_1d_half_kernel+double-.cxx"
# End Source File
# Begin Source File

SOURCE=.\osl_break_edge.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_base.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_gradient.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_nms.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox_params.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_port.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell_params.cxx
# End Source File
# Begin Source File

SOURCE=".\Templates\osl_canny_smooth+vil_byte-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\osl_canny_smooth+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\osl_canny_smooth+vil_rgb+uchar--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\osl_canny_smooth+ushort-.cxx"
# End Source File
# Begin Source File

SOURCE=.\osl_canny_smooth.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_chamfer.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_convolve.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_easy_canny.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_edge.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector_params.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_edgel_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines_params.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_harris.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_harris_params.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_kernel.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_load_topology.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_ortho_regress.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_OrthogRegress.cxx
# End Source File
# Begin Source File

SOURCE=.\internals\osl_reorder_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_roi_window.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_save_topology.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_topology.cxx
# End Source File
# Begin Source File

SOURCE=.\osl_vertex.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\internals\droid.h
# End Source File
# Begin Source File

SOURCE=.\osl_1d_half_kernel.h
# End Source File
# Begin Source File

SOURCE=.\osl_break_edge.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_base.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_gradient.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_nms.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_ox_params.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_port.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_rothwell_params.h
# End Source File
# Begin Source File

SOURCE=.\osl_canny_smooth.h
# End Source File
# Begin Source File

SOURCE=.\osl_chamfer.h
# End Source File
# Begin Source File

SOURCE=.\osl_convolve.h
# End Source File
# Begin Source File

SOURCE=.\osl_easy_canny.h
# End Source File
# Begin Source File

SOURCE=.\osl_edge.h
# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector.h
# End Source File
# Begin Source File

SOURCE=.\osl_edge_detector_params.h
# End Source File
# Begin Source File

SOURCE=.\osl_edgel_chain.h
# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines.h
# End Source File
# Begin Source File

SOURCE=.\osl_fit_lines_params.h
# End Source File
# Begin Source File

SOURCE=.\osl_hacks.h
# End Source File
# Begin Source File

SOURCE=.\osl_harris.h
# End Source File
# Begin Source File

SOURCE=.\osl_harris_params.h
# End Source File
# Begin Source File

SOURCE=.\osl_kernel.h
# End Source File
# Begin Source File

SOURCE=.\osl_load_topology.h
# End Source File
# Begin Source File

SOURCE=.\osl_ortho_regress.h
# End Source File
# Begin Source File

SOURCE=.\osl_OrthogRegress.h
# End Source File
# Begin Source File

SOURCE=.\internals\osl_reorder_chain.h
# End Source File
# Begin Source File

SOURCE=.\osl_roi_window.h
# End Source File
# Begin Source File

SOURCE=.\osl_save_topology.h
# End Source File
# Begin Source File

SOURCE=.\osl_topology.h
# End Source File
# Begin Source File

SOURCE=.\osl_vertex.h
# End Source File
# End Group
# End Target
# End Project
