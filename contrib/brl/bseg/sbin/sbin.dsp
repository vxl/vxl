# Microsoft Developer Studio Project File - Name="sbin" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
#
# /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\brl\bbas" /I "$(VXLROOT)\contrib\brl\bseg" /I "$(VXLROOT)\contrib\oxl" /I "$(VXLROOT)\contrib\gel" == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL == compiler defines
# sbin  == name of output library
#  /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\  segv.lib bgui.lib vgui.lib sdet.lib bdgl.lib gevd.lib btol.lib vtol.lib vdgl.lib vsol.lib vgl_algo.lib vul.lib vpl.lib vnl_algo.lib vnl.lib netlib.lib vil.lib ws2_32.lib tiff.lib png.lib zlib.lib jpeg.lib vgl.lib vbl.lib opengl32.lib glu32.lib Qv.lib vcl.lib /STACK:10000000  == libraries linked in
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=sbin - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "sbin.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "sbin.mak" CFG="sbin - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "sbin - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "sbin - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "sbin - Win32 Release"

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
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\brl\bbas" /I "$(VXLROOT)\contrib\brl\bseg" /I "$(VXLROOT)\contrib\oxl" /I "$(VXLROOT)\contrib\gel"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "sbin_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2 -DCMAKE_INTDIR=\"Release\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\brl\bbas" /I "$(VXLROOT)\contrib\brl\bseg" /I "$(VXLROOT)\contrib\oxl" /I "$(VXLROOT)\contrib\gel" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\
# ADD LINK32 segv.lib
# ADD LINK32 bgui.lib
# ADD LINK32 vgui.lib
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 gevd.lib
# ADD LINK32 btol.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl_algo.lib
# ADD LINK32 vul.lib
# ADD LINK32 vpl.lib
# ADD LINK32 vnl_algo.lib
# ADD LINK32 vnl.lib
# ADD LINK32 netlib.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vbl.lib
# ADD LINK32 opengl32.lib
# ADD LINK32 glu32.lib
# ADD LINK32 Qv.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000

!ELSEIF  "$(CFG)" == "sbin - Win32 Debug"

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
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\brl\bbas" /I "$(VXLROOT)\contrib\brl\bseg" /I "$(VXLROOT)\contrib\oxl" /I "$(VXLROOT)\contrib\gel"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_OPENGL /D "sbin_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ -DCMAKE_INTDIR=\"Debug\"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\brl" /I "$(VXLROOT)\contrib\brl\bbas" /I "$(VXLROOT)\contrib\brl\bseg" /I "$(VXLROOT)\contrib\oxl" /I "$(VXLROOT)\contrib\gel" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\
# ADD LINK32 segv.lib
# ADD LINK32 bgui.lib
# ADD LINK32 vgui.lib
# ADD LINK32 sdet.lib
# ADD LINK32 bdgl.lib
# ADD LINK32 gevd.lib
# ADD LINK32 btol.lib
# ADD LINK32 vtol.lib
# ADD LINK32 vdgl.lib
# ADD LINK32 vsol.lib
# ADD LINK32 vgl_algo.lib
# ADD LINK32 vul.lib
# ADD LINK32 vpl.lib
# ADD LINK32 vnl_algo.lib
# ADD LINK32 vnl.lib
# ADD LINK32 netlib.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vgl.lib
# ADD LINK32 vbl.lib
# ADD LINK32 opengl32.lib
# ADD LINK32 glu32.lib
# ADD LINK32 Qv.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000
!ENDIF

# Begin Target

# Name "sbin - Win32 Release"
# Name "sbin - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\seg.cxx
# End Source File
# Begin Source File
SOURCE=.\seg_vil.cxx
# End Source File
# Begin Source File
SOURCE=.\negate.cxx
# End Source File
# Begin Source File
SOURCE=.\vil_convert.cxx
# End Source File
# Begin Source File
SOURCE=.\generate_tiff_pyramids.cxx
# End Source File
# Begin Source File
SOURCE=.\generate_tiff_rset.cxx
# End Source File
# End Group
# End Target
# End Project
