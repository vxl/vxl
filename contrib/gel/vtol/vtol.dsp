# Microsoft Developer Studio Project File - Name="vtol" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=vtol - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "vtol.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "vtol.mak" CFG="vtol - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "vtol - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "vtol - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "vtol - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\gel" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Release\vtol.lib"

!ELSEIF  "$(CFG)" == "vtol - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "$(IUEROOT)\vcl\config.win32-vc60" /I "$(IUEROOT)\vcl" /I "$(IUEROOT)\vxl" /I "$(IUEROOT)\gel" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Debug\vtol.lib"

!ENDIF 

# Begin Target

# Name "vtol - Win32 Release"
# Name "vtol - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_block-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_chain-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_edge_2d-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_face_2d-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_face-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_one_chain-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_topology_object-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_two_chain-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_vertex_2d-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_vertex-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_zero_chain-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_list+vtol_chain_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_list+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_block_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_block~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_chain_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_edge_2d_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_edge_2d~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_edge~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_face_2d_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_face_2d~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_face~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_one_chain_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_one_chain~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_topology_object_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_topology_object~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_two_chain_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_two_chain~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_vertex_2d_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_vertex_2d~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_vertex~-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_zero_chain_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_zero_chain~-.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_block.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_edge.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_edge_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_face_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_face.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_list_functions.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_one_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_topology_cache.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_topology_io.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_topology_object.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_two_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_vertex.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_vertex_2d.cxx
# End Source File
# Begin Source File

SOURCE=.\vtol_zero_chain.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vbl_smart_ptr+vtol_edge-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_edge_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_face_sptr-.cxx
# End Source File
# Begin Source File

SOURCE=.\Templates\vcl_vector+vtol_vertex_sptr-.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
