# Microsoft Developer Studio Project File - Name="vbl_io" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vbl_io - Win32 StaticDebug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vbl_io.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vbl_io.mak" CFG="vbl_io - Win32 StaticDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vbl_io - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl_io - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl_io - Win32 StaticDebug" (based on "Win32 (x86) Static Library")
!MESSAGE "vbl_io - Win32 StaticRelease" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vbl_io - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Release\vbl_io.lib"

!ELSEIF  "$(CFG)" == "vbl_io - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\Debug\vbl_io.lib"

!ELSEIF  "$(CFG)" == "vbl_io - Win32 StaticDebug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "_DEBUG" /D "_LIB" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\StaticDebug\vbl_io.lib"

!ELSEIF  "$(CFG)" == "vbl_io - Win32 StaticRelease"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /Ob2 /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\mul" /D "WIN32" /D "NDEBUG" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32
# ADD LIB32 /nologo /out:"..\..\StaticRelease\vbl_io.lib"

!ENDIF 

# Begin Target

# Name "vbl_io - Win32 Release"
# Name "vbl_io - Win32 Debug"
# Name "vbl_io - Win32 StaticDebug"
# Name "vbl_io - Win32 StaticRelease"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;txx"
# Begin Source File

SOURCE=".\Templates\vbl_io_array_1d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_1d+float-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_2d+double-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_2d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_2d+unsigned-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_2d+unsignedchar-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_array_3d+int-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_bounding_box+double.2-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_bounding_box+float.2-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_bounding_box+int.2-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_sparse_array+double.uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_sparse_array+double.vcl_pair+uint.uint--.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_sparse_array+int.uint-.cxx"
# End Source File
# Begin Source File

SOURCE=".\Templates\vbl_io_sparse_array+int.vcl_pair+uint.uint--.cxx"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dll.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_1d.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_2d.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_3d.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_bounding_box.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_smart_ptr.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_sparse_array_base.txx
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_1d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_2d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_array_3d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_bounding_box.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_smart_ptr.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_sparse_array_1d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_sparse_array_2d.h
# End Source File
# Begin Source File

SOURCE=.\vbl_io_sparse_array_base.h
# End Source File
# End Group
# End Target
# End Project
