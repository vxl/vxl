# Microsoft Developer Studio Project File - Name="pcbuilder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **


# variables to REPLACE
# 
#  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff" == include path
#  == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF == compiler defines
# vil2_io_test_driver  == name of output library
#  /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\  vil2_io.lib vpl.lib testlib.lib vsl.lib vil2.lib vil.lib ws2_32.lib tiff.lib png.lib zlib.lib jpeg.lib vcl.lib /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=vil2_io_test_driver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vil2_io_test_driver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vil2_io_test_driver.mak" CFG="vil2_io_test_driver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vil2_io_test_driver - Win32 MinSizeRel" (based on "Win32 (x86) Application")
!MESSAGE "vil2_io_test_driver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "vil2_io_test_driver - Win32 RelWithDebInfo" (based on "Win32 (x86) Application")
!MESSAGE "vil2_io_test_driver - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vil2_io_test_driver - Win32 Release"

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
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF /D "vil2_io_test_driver_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2 -DCMAKE_INTDIR=\"Release\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 vil2_io.lib
# ADD LINK32 vpl.lib
# ADD LINK32 testlib.lib
# ADD LINK32 vsl.lib
# ADD LINK32 vil2.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000 




!ELSEIF  "$(CFG)" == "vil2_io_test_driver - Win32 Debug"

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
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF /D "vil2_io_test_driver_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ -DCMAKE_INTDIR=\"Debug\"
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff" /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32   kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 vil2_io.lib
# ADD LINK32 vpl.lib
# ADD LINK32 testlib.lib
# ADD LINK32 vsl.lib
# ADD LINK32 vil2.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000 




!ELSEIF  "$(CFG)" == "vil2_io_test_driver - Win32 MinSizeRel"
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
# ADD CPP /nologo  /D "WIN32"  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF /D "NDEBUG" /D "_CONSOLE" /D "_MBCS"  /FD /c
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF /D "vil2_io_test_driver_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O1 -DCMAKE_INTDIR=\"MinSizeRel\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 vil2_io.lib
# ADD LINK32 vpl.lib
# ADD LINK32 testlib.lib
# ADD LINK32 vsl.lib
# ADD LINK32 vil2.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000 




!ELSEIF  "$(CFG)" == "vil2_io_test_driver - Win32 RelWithDebInfo"

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
# ADD CPP  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff"   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DHAS_PNG -DHAS_JPEG -DHAS_TIFF /D "vil2_io_test_driver_EXPORTS"
# ADD CPP /nologo /W3 /Zm1000 /GX /GR
# ADD CPP /MD /Zi /O2 -DCMAKE_INTDIR=\"RelWithDebInfo\"
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC  /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\core" /I "$(VXLROOT)\vcl" /I "$(VXLROOT)\vcl\config.win32" /I "$(VXLROOT)\core" /I "$(VXLROOT)\contrib\mul" /I "$(VXLROOT)\v3p\png" /I "$(VXLROOT)\v3p\zlib" /I "$(VXLROOT)\v3p\jpeg" /I "$(VXLROOT)\v3p\tiff" /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /IGNORE:4089

# ADD LINK32 /LIBPATH:$(VXLROOT)\lib\$(INTDIR)  /LIBPATH:$(VXLROOT)\lib\ 
# ADD LINK32 vil2_io.lib
# ADD LINK32 vpl.lib
# ADD LINK32 testlib.lib
# ADD LINK32 vsl.lib
# ADD LINK32 vil2.lib
# ADD LINK32 vil.lib
# ADD LINK32 ws2_32.lib
# ADD LINK32 tiff.lib
# ADD LINK32 png.lib
# ADD LINK32 zlib.lib
# ADD LINK32 jpeg.lib
# ADD LINK32 vcl.lib
# ADD LINK32 /STACK:10000000 




!ENDIF 

# Begin Target

# Name "vil2_io_test_driver - Win32 Release"
# Name "vil2_io_test_driver - Win32 Debug"
# Name "vil2_io_test_driver - Win32 MinSizeRel"
# Name "vil2_io_test_driver - Win32 RelWithDebInfo"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_image_view_io.cxx
# End Source File
# Begin Source File
SOURCE=.\test_memory_chunk_io.cxx
# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

