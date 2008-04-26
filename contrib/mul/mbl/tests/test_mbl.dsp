# Microsoft Developer Studio Project File - Name="test_mbl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# variables to REPLACE
# 
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" == include path
# $(VXLROOT)/bin/ == override in output directory
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
# test_mbl  == name of output library
# /libpath:"$(VXLROOT)/contrib/mul/mbl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vnl/io/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)" /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)" /libpath:"$(VXLROOT)/vcl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vsl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vbl/$(OUTDIR)" /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)" "mbl.lib" "vnl_io.lib" "vnl_algo.lib" "vnl.lib" "netlib.lib" "vsl.lib" "vcl.lib" "vbl.lib" "vul.lib" /STACK:10000000  == libraries linked in 
# TARGTYPE "Win32 (x86) Application" 0x0103

CFG=test_mbl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "test_mbl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "test_mbl.mak" CFG="test_mbl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "test_mbl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "test_mbl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "test_mbl - Win32 Release"

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
# ADD BASE CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /nologo  /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "test_mbl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MD /O2
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/contrib/mul/mbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/io/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vsl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)"
# ADD LINK32 "mbl.lib" "vnl_algo.lib" "vnl_io.lib" "vnl.lib" "netlib.lib" "vsl.lib" "vcl.lib" "vbl.lib" "vul.lib" /STACK:10000000 

!ELSEIF  "$(CFG)" == "test_mbl - Win32 Debug"

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
# ADD BASE CPP   /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /nologo  /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/mul" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "test_mbl_EXPORTS"
# ADD CPP /W3 /Zm1000 /GX /GR
# ADD CPP /MDd /Zi /Od /GZ
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /IGNORE:4089

# ADD LINK32 /libpath:"$(VXLROOT)/contrib/mul/mbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/algo/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/io/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vnl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/v3p/netlib/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/vcl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vsl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vbl/$(OUTDIR)"
# ADD LINK32 /libpath:"$(VXLROOT)/core/vul/$(OUTDIR)"
# ADD LINK32 "mbl.lib" "vnl_algo.lib" "vnl_io.lib" "vnl.lib" "netlib.lib" "vsl.lib" "vcl.lib" "vbl.lib" "vul.lib" /STACK:10000000 

!ENDIF 

# Begin Target

# Name "test_mbl - Win32 Release"
# Name "test_mbl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter "cxx"
# Begin Source File
SOURCE=.\test_driver.cxx
# End Source File
# Begin Source File
SOURCE=.\test_ar_process.cxx
# End Source File
# Begin Source File
SOURCE=.\test_clamped_plate_spline_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_combination.cxx
# End Source File
# Begin Source File
SOURCE=.\test_correspond_points.cxx
# End Source File
# Begin Source File
SOURCE=.\test_data_wrapper_mixer.cxx
# End Source File
# Begin Source File
SOURCE=.\test_file_data_wrapper.cxx
# End Source File
# Begin Source File
SOURCE=.\test_gamma.cxx
# End Source File
# Begin Source File
SOURCE=.\test_histogram.cxx
# End Source File
# Begin Source File
SOURCE=.\test_index_sort.cxx
# End Source File
# Begin Source File
SOURCE=.\test_jarque_bera.cxx
# End Source File
# Begin Source File
SOURCE=.\test_k_means.cxx
# End Source File
# Begin Source File
SOURCE=.\test_lda.cxx
# End Source File
# Begin Source File
SOURCE=.\test_lru_cache.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matrix_products.cxx
# End Source File
# Begin Source File
SOURCE=.\test_matxvec.cxx
# End Source File
# Begin Source File
SOURCE=.\test_parse_block.cxx
# End Source File
# Begin Source File
SOURCE=.\test_priority_bounded_queue.cxx
# End Source File
# Begin Source File
SOURCE=.\test_random_n_from_m.cxx
# End Source File
# Begin Source File
SOURCE=.\test_rbf_network.cxx
# End Source File
# Begin Source File
SOURCE=.\test_read_props.cxx
# End Source File
# Begin Source File
SOURCE=.\test_read_multi_props.cxx
# End Source File
# Begin Source File
SOURCE=.\test_select_n_from_m.cxx
# End Source File
# Begin Source File
SOURCE=.\test_selected_data_wrapper.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stats_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stochastic_data_collector.cxx
# End Source File
# Begin Source File
SOURCE=.\test_sum_1d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_thin_plate_spline_2d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_thin_plate_spline_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_thin_plate_spline_weights_3d.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stats_nd.cxx
# End Source File
# Begin Source File
SOURCE=.\test_table.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cloneables_factory.cxx
# End Source File
# Begin Source File
SOURCE=.\test_rvm_regression_builder.cxx
# End Source File
# Begin Source File
SOURCE=.\test_test.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cloneable_ptr.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stl.cxx
# End Source File
# Begin Source File
SOURCE=.\test_exception.cxx
# End Source File
# Begin Source File
SOURCE=.\test_log.cxx
# End Source File
# Begin Source File
SOURCE=.\test_gram_schmidt.cxx
# End Source File
# Begin Source File
SOURCE=.\test_linear_interpolator.cxx
# End Source File
# Begin Source File
SOURCE=.\test_stepwise_regression.cxx
# End Source File
# Begin Source File
SOURCE=.\test_parse_sequence.cxx
# End Source File
# Begin Source File
SOURCE=.\test_cluster_tree.cxx
# End Source File
# Begin Source File
SOURCE=.\test_clusters.cxx
# End Source File
# Begin Source File
SOURCE=.\test_dyn_prog.cxx
# End Source File
# Begin Source File
SOURCE=.\test_parse_tuple.cxx
# End Source File
# Begin Source File
SOURCE=.\test_progress.cxx
# End Source File
# Begin Source File
SOURCE=.\test_text_file.cxx
# End Source File
# Begin Source File
SOURCE=.\test_include.cxx
# End Source File
# End Group
# End Target
# End Project
