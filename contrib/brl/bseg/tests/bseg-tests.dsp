# Microsoft Developer Studio Project File - Name="bseg_tests" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **


# variables to REPLACE
# 
#  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas" == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# bseg_tests  == name of output library
#  /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\  sdet.lib bdgl.lib btol.lib gevd.lib vtol.lib vdgl.lib vsol.lib vgl-algo.lib vnl-algo.lib vbl.lib vgl.lib vil.lib vnl.lib vul.lib vcl.lib jpeg.lib netlib.lib png.lib tiff.lib /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=bseg_tests - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bseg_tests.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bseg_tests.mak" CFG="bseg_tests - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bseg_tests - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "bseg_tests - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "bseg_tests - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE "bseg_tests - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bseg_tests - Win32 Release"

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
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bdgl_test_curve_algs_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2 -DCMAKE_INTDIR=\"Release\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 btol.lib
# ADD LINK32 gevd.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl-algo.lib
# ADD LINK32 vnl-algo.lib
# ADD LINK32 vbl.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vil.lib
# ADD LINK32 vnl.lib
# ADD LINK32 vul.lib
# ADD LINK32 vcl.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 netlib.lib
# ADD LINK32 png.lib
# ADD LINK32 tiff.lib
# ADD LINK32 /STACK:10000000 

!ELSEIF  "$(CFG)" == "bseg_tests - Win32 Debug"

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
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP /nologo  /D "WIN32"  /D "_DEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /GZ /c
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bdgl_test_curve_algs_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ -DCMAKE_INTDIR=\"Debug\"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 btol.lib
# ADD LINK32 gevd.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl-algo.lib
# ADD LINK32 vnl-algo.lib
# ADD LINK32 vbl.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vil.lib
# ADD LINK32 vnl.lib
# ADD LINK32 vul.lib
# ADD LINK32 vcl.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 netlib.lib
# ADD LINK32 png.lib
# ADD LINK32 tiff.lib
# ADD LINK32 /STACK:10000000 

!ELSEIF  "$(CFG)" == "bseg_tests - Win32 MinSizeRel"
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MinSizeRel"
# PROP BASE Intermediate_Dir "MinSizeRel"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "MinSizeRel"
# PROP Intermediate_Dir "MinSizeRel"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bdgl_test_curve_algs_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 btol.lib
# ADD LINK32 gevd.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl-algo.lib
# ADD LINK32 vnl-algo.lib
# ADD LINK32 vbl.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vil.lib
# ADD LINK32 vnl.lib
# ADD LINK32 vul.lib
# ADD LINK32 vcl.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 netlib.lib
# ADD LINK32 png.lib
# ADD LINK32 tiff.lib
# ADD LINK32 /STACK:10000000 

!ELSEIF  "$(CFG)" == "bseg_tests - Win32 RelWithDebInfo"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "RelWithDebInfo"
# PROP BASE Intermediate_Dir "RelWithDebInfo"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "RelWithDebInfo"
# PROP Intermediate_Dir "RelWithDebInfo"
# PROP Target_Dir ""
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP /nologo  /D "WIN32"  /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "bdgl_test_curve_algs_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\gel" /I "$(VXLROOT)\contrib\brl\bbas" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 btol.lib
# ADD LINK32 gevd.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl-algo.lib
# ADD LINK32 vnl-algo.lib
# ADD LINK32 vbl.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vil.lib
# ADD LINK32 vnl.lib
# ADD LINK32 vul.lib
# ADD LINK32 vcl.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 netlib.lib
# ADD LINK32 png.lib
# ADD LINK32 tiff.lib
# ADD LINK32 /STACK:10000000 
!ENDIF 

# Begin Target

# Name "bseg_tests - Win32 Release"
# Name "bseg_tests - Win32 Debug"
# Name "bseg_tests - Win32 MinSizeRel"
# Name "bseg_tests - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_test_detector.cxx
# End Source File
# Begin Source File
SOURCE=.\sdet_test_region_proc.cxx
# End Source File
# Begin Source File
SOURCE=.\brip_test_lucas_kanade.cxx
# End Source File
# End Group
# End Target
# End Project
