# Microsoft Developer Studio Project File - Name="vcl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"  == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL == compiler defines
#  == override in output directory
# vcl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vcl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vcl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vcl.mak" CFG="vcl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vcl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "vcl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vcl - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "vcl - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB"  /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB"  /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vxl" /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/vcl/config.win32-VC60" /I "config.win32-vc60"    -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE -DBUILDING_VCL_DLL /D "vcl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
!ENDIF 

# Begin Target

# Name "vcl - Win32 Release"
# Name "vcl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Templates/alloc-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/complex-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/fstream-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/stream-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/string-instances.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+unsigned-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+ushort-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vcl_pair+double.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_algorithm+vcl_pair+float.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_deque+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+float~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_pair+void~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_pair+void~.void~--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_list+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+double.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+int.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+long.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+ulong.vcl_pair+void~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.unsigned_int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+unsigned_int.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+int.int-.vcl_pair+float.float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+int.int-.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+uint.uint-.double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_pair+uint.uint-.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+vcl_string.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.vcl_pair+ulong.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_map+void~.void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_ostream_iterator+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_pair+float.float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_pair+void~.int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_queue+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+unsigned-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_set+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_unique+double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+bool-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+bool~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+char-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+char~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+const_char~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double_const~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+float-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+float~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+int-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+int~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long_double-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long_double~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+long~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+schar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+schar~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uchar-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uchar~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uint-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+uint~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+ulong-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+ulong~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+float-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+long_double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_complex+long_double-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_map+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_map+uint.uint--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+char~.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+double.double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+double.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+float.float--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+float.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.double~--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.int-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+int.vcl_string--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+uint.uint--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_pair+vcl_string.vcl_string--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_string-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+double--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+int--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+int-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+unsigned--.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+unsigned-~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+void~-.cxx

# End Source File
# Begin Source File

SOURCE=.\Templates/vcl_vector+vcl_vector+vcl_pair+uint.uint---.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_alloc.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hashtable.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_rbtree_instances.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_straits.cxx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_string_instances.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cassert.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cmath.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_cstdlib.cxx

# End Source File
# Begin Source File

SOURCE=.\vcl_deprecated.cxx

# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File

SOURCE=.\egcs/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\egcs/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_algorithm.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_alloc.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_deque.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_functional.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hash.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hash_map.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hash_map.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hashtable.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_iterator.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_list.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_map.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_multimap.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_multimap.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_rbtree.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_set.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_straits.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_utility.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_vector.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\gcc-libstdcxx-v3/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_algorithm.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_deque.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_functional.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_iterator.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_list.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_map.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_set.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_utility.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_vector.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_iterator.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_map.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_vector.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_algorithm.h

# End Source File
# Begin Source File

SOURCE=.\vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_cassert.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\vcl_deprecated.h

# End Source File
# Begin Source File

SOURCE=.\vcl_deque.h

# End Source File
# Begin Source File

SOURCE=.\vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_functional.h

# End Source File
# Begin Source File

SOURCE=.\vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_iterator.h

# End Source File
# Begin Source File

SOURCE=.\vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_list.h

# End Source File
# Begin Source File

SOURCE=.\vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_map.h

# End Source File
# Begin Source File

SOURCE=.\vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_set.h

# End Source File
# Begin Source File

SOURCE=.\vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_stack.h

# End Source File
# Begin Source File

SOURCE=.\vcl_stack.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_utility.h

# End Source File
# Begin Source File

SOURCE=.\vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_vector.h

# End Source File
# Begin Source File

SOURCE=.\vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_algorithm.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_complex.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_deque.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_functional.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_iterator.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_list.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_map.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_set.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_string.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_utility.txx

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_vector.txx

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_algobase.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_bool.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_bvector.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_complex_fwd.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_defalloc.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_functionx.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_hash_set.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_heap.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_multiset.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_new.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_pair.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_rel_ops.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_stack.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_stlconf.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_stlfwd.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_tempbuf.h

# End Source File
# Begin Source File

SOURCE=.\emulation/vcl_tree.h

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_iomanip.h

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_ios.h

# End Source File
# Begin Source File

SOURCE=.\gcc-295/vcl_queue.txx

# End Source File
# Begin Source File

SOURCE=.\gcc/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\gcc/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_algorithm.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_bitset.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_blah.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cassert.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cctype.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cerrno.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cfloat.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_ciso646.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_climits.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_clocale.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_complex.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_csetjmp.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_csignal.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cstdarg.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cstddef.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cstdio.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cstring.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_ctime.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cwchar.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_cwctype.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_deque.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_exception.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_fstream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_functional.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_iomanip.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_ios.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_iosfwd.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_iostream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_istream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_iterator.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_limits.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_list.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_locale.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_map.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_memory.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_numeric.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_ostream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_queue.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_set.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_sstream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_stack.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_stdexcept.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_streambuf.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_string.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_strstream.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_typeinfo.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_utility.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_valarray.h

# End Source File
# Begin Source File

SOURCE=.\generic/vcl_vector.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_bitset.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cassert.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cctype.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cerrno.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cfloat.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_ciso646.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_climits.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_clocale.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_csetjmp.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_csignal.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cstdarg.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cstddef.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cstdio.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cstring.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_ctime.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cwchar.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_cwctype.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_exception.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_fstream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_iomanip.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_ios.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_iosfwd.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_iostream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_istream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_limits.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_locale.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_memory.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_new.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_numeric.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_ostream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_queue.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_queue.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_sstream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_stack.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_stack.txx

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_stdexcept.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_streambuf.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_strstream.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_typeinfo.h

# End Source File
# Begin Source File

SOURCE=.\iso/vcl_valarray.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_iomanip.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_ios.h

# End Source File
# Begin Source File

SOURCE=.\sgi/vcl_iostream.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_complex_fwd.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\sunpro/vcl_stack.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_bitset.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cctype.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cerrno.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cfloat.h

# End Source File
# Begin Source File

SOURCE=.\vcl_ciso646.h

# End Source File
# Begin Source File

SOURCE=.\vcl_climits.h

# End Source File
# Begin Source File

SOURCE=.\vcl_clocale.h

# End Source File
# Begin Source File

SOURCE=.\vcl_compiler.h

# End Source File
# Begin Source File

SOURCE=.\vcl_complex_fwd.h

# End Source File
# Begin Source File

SOURCE=.\vcl_csetjmp.h

# End Source File
# Begin Source File

SOURCE=.\vcl_csignal.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cstdarg.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cstddef.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cstdio.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cstring.h

# End Source File
# Begin Source File

SOURCE=.\vcl_ctime.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cwchar.h

# End Source File
# Begin Source File

SOURCE=.\vcl_cwctype.h

# End Source File
# Begin Source File

SOURCE=.\vcl_deprecated_header.h

# End Source File
# Begin Source File

SOURCE=.\vcl_exception.h

# End Source File
# Begin Source File

SOURCE=.\vcl_fstream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_iomanip.h

# End Source File
# Begin Source File

SOURCE=.\vcl_ios.h

# End Source File
# Begin Source File

SOURCE=.\vcl_iosfwd.h

# End Source File
# Begin Source File

SOURCE=.\vcl_iostream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_istream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_limits.h

# End Source File
# Begin Source File

SOURCE=.\vcl_locale.h

# End Source File
# Begin Source File

SOURCE=.\vcl_memory.h

# End Source File
# Begin Source File

SOURCE=.\vcl_new.h

# End Source File
# Begin Source File

SOURCE=.\vcl_numeric.h

# End Source File
# Begin Source File

SOURCE=.\vcl_ostream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_queue.h

# End Source File
# Begin Source File

SOURCE=.\vcl_queue.txx

# End Source File
# Begin Source File

SOURCE=.\vcl_rel_ops.h

# End Source File
# Begin Source File

SOURCE=.\vcl_sstream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_stdexcept.h

# End Source File
# Begin Source File

SOURCE=.\vcl_stlfwd.h

# End Source File
# Begin Source File

SOURCE=.\vcl_streambuf.h

# End Source File
# Begin Source File

SOURCE=.\vcl_strstream.h

# End Source File
# Begin Source File

SOURCE=.\vcl_sys/time.h

# End Source File
# Begin Source File

SOURCE=.\vcl_sys/types.h

# End Source File
# Begin Source File

SOURCE=.\vcl_typeinfo.h

# End Source File
# Begin Source File

SOURCE=.\vcl_valarray.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_cmath.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_complex_fwd.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_cstdio.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_cstdlib.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_cstring.h

# End Source File
# Begin Source File

SOURCE=.\win32-vc60/vcl_ctime.h

# End Source File
# Begin Source File

SOURCE=.\vcl_where_root_dir.h.in

# End Source File
# Begin Source File

SOURCE=.\vcl_config_compiler.h.in

# End Source File
# Begin Source File

SOURCE=.\vcl_config_headers.h.in

# End Source File
# Begin Source File

SOURCE=.\vcl_config_manual.h.in

# End Source File
# End Group
# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project

