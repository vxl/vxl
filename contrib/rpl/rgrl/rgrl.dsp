# Microsoft Developer Studio Project File - Name="rgrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# ITK DSP Header file
# This file is read by the build system of itk, and is used as the top part of
# a microsoft project dsp header file
# IF this is in a dsp file, then it is not the header, but has
# already been used, so do not edit here...

# variables to REPLACE
#
# /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" == include path
#   -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE == compiler defines
#  == override in output directory
# rgrl  == name of output library

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=rgrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE
!MESSAGE NMAKE /f "rgrl.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "rgrl.mak" CFG="rgrl - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "rgrl - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "rgrl - Win32 Release" (based on "Win32 (x86) Static Library")
# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "rgrl - Win32 Release"

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
# ADD BASE CPP /nologo /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "NDEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rgrl_EXPORTS"
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

!ELSEIF  "$(CFG)" == "rgrl - Win32 Debug"

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
# ADD BASE CPP /nologo /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /c
# ADD CPP /nologo /D "_DEBUG" /D "WIN32" /D "_MBCS" /D "_LIB" /FD /GZ /c
# ADD CPP /I "$(VXLROOT)/vcl" /I "$(VXLROOT)/core" /I "$(VXLROOT)/vcl/config.win32" /I "$(VXLROOT)/contrib/rpl" -DVXL_WARN_DEPRECATED -DVXL_WARN_DEPRECATED_ONCE /D "rgrl_EXPORTS"
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

# Name "rgrl - Win32 Release"
# Name "rgrl - Win32 Debug"

# Begin Group "Source Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\rgrl_converge_status.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_on_median_error.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_on_weighted_error.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_tester.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_data_manager.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_debug_util.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_affine.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_quadratic.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_reduced_quad2d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_rigid.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_similarity2d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_spline.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_translation.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_estimator.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_based_registration.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_face_pt.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_landmark.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_point.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_trace_pt.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_inv_indexing.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_prior.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_ran_sam.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_match.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_single_landmark.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_mask.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_match_set.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_fixed.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_boundary.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_object.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_all_weights.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_closest.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_null.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_estimator.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_spline.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_affine.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_mixed_spline.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_quadratic.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_reader.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_reduced_quad.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_rigid.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_similarity.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_spline.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_translation.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_transformation.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_util.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_view.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter_m_est.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter_unit.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_feature_set_location+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_feature_set_location+3-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_feature_set_location_masked+2-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_set_of+rgrl_match_set_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_set_of+rgrl_scale_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_command-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_converge_status-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_convergence_tester-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_data_manager-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_estimator-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_event-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_feature-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_feature_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_initializer-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_invariant-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_invariant_match-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_mask-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_match_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_matcher-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_object-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_scale-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_scale_estimator_unwgted-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_scale_estimator_wgted-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_spline-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_transformation-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_view-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_weighter-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo+vxl_byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo+vxl_uint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo_3d+float-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo_3d+vxl_byte-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo_3d+vxl_sint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_matcher_pseudo_int_3d+vxl_sint_16-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_evaluator-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rgrl_feature_caster+rgrl_feature_trace_pt-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_feature_face_pt-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/vbl_smart_ptr+rgrl_invariant_set-.cxx
# End Source File
# Begin Source File
SOURCE=.\Templates/rsdl_bins_2d+2.double.rgrl_feature_sptr-.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_homography2d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_homography2d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_homo2d_lm.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_evaluator_ssd.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_face_region.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_point_region.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_region.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_trace_region.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_reader.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_reader.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_dis_homo2d_lm.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_rad_dis_homo2d.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_couple.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_converge_status_nas.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_random.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_pick_one.cxx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_bins_2d.cxx
# End Source File
# End Group
# Begin Group "Header Files"
# PROP Default_Filter ""
# Begin Source File
SOURCE=.\rgrl_cast.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_command.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_command_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_converge_status.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_converge_status_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_on_median_error.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_on_weighted_error.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_tester.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_convergence_tester_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_data_manager.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_data_manager_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_debug_util.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_affine.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_quadratic.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_reduced_quad2d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_rigid.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_similarity2d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_spline.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_translation.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_estimator.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_estimator_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_event.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_event_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_based_registration.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_face_pt.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_face_pt_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_landmark.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_point.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_location.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_location.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_location_masked.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_location_masked.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_trace_pt.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_fwd.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_inv_indexing.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_prior.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_ran_sam.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_match.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_match_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_single_landmark.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_macros.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_mask.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_mask_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_match_set.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_match_set_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_fixed.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_boundary.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_object.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_object_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_all_weights.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_closest.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_est_null.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_estimator.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_set_of.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_set_of.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_spline.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_spline_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_affine.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_mixed_spline.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_quadratic.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_reader.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_reduced_quad.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_rigid.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_similarity.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_spline.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_translation.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_transformation.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_transformation_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_util.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_view.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_view_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter_m_est.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_weighter_unit.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_copyright.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_scale_estimator_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_homography2d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_homography2d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_homo2d_lm.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_evaluator_ssd.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_face_region.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_point_region.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_region.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_trace_region.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_evaluator.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_evaluator_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo_3d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo_3d.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo_int_3d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_pseudo_int_3d.txx
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_reader.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_initializer_reader.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_est_dis_homo2d_lm.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_rad_dis_homo2d.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_trans_couple.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_converge_status_nas.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_random.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_matcher_k_nearest_pick_one.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_set.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_invariant_set_sptr.h
# End Source File
# Begin Source File
SOURCE=.\rgrl_feature_set_bins_2d.h
# End Source File
# End Group
# End Target
# End Project
