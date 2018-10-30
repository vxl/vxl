# mul/mfpf/CMakeLists.txt
#
#   Tim Cootes
#

doxygen_add_library(contrib/mul/mfpf
  DEPENDS contrib/mul/clsfy contrib/mul/mipa contrib/mul/vimt contrib/mul/vpdfl contrib/mul/mbl
  PACKAGE contrib/mul
  DESCRIPTION "Manchester's Point Finder Library"
)

set(mfpf_sources
  mfpf_add_all_loaders.h         mfpf_add_all_loaders.cxx
  mfpf_point_finder_builder.h    mfpf_point_finder_builder.cxx
  mfpf_edge_finder_builder.h     mfpf_edge_finder_builder.cxx
  mfpf_point_finder.h            mfpf_point_finder.cxx
  mfpf_edge_finder.h             mfpf_edge_finder.cxx
  mfpf_max_finder.h              mfpf_max_finder.cxx
  mfpf_norm_corr1d.h             mfpf_norm_corr1d.cxx
  mfpf_norm_corr1d_builder.h     mfpf_norm_corr1d_builder.cxx
  mfpf_norm_corr2d.h             mfpf_norm_corr2d.cxx
  mfpf_norm_corr2d_builder.h     mfpf_norm_corr2d_builder.cxx
  mfpf_grad_corr2d.h             mfpf_grad_corr2d.cxx
  mfpf_grad_corr2d_builder.h     mfpf_grad_corr2d_builder.cxx
  mfpf_profile_pdf.h             mfpf_profile_pdf.cxx
  mfpf_profile_pdf_builder.h     mfpf_profile_pdf_builder.cxx
  mfpf_region_pdf.h              mfpf_region_pdf.cxx
  mfpf_region_pdf_builder.h      mfpf_region_pdf_builder.cxx
  mfpf_region_finder.h           mfpf_region_finder.cxx
  mfpf_region_finder_builder.h   mfpf_region_finder_builder.cxx
  mfpf_lin_clsfy_finder_builder.h mfpf_lin_clsfy_finder_builder.cxx
  mfpf_hog_box_finder.h          mfpf_hog_box_finder.cxx
  mfpf_hog_box_finder_builder.h  mfpf_hog_box_finder_builder.cxx

  mfpf_vec_cost.h                mfpf_vec_cost.cxx
  mfpf_sad_vec_cost.h            mfpf_sad_vec_cost.cxx
  mfpf_ssd_vec_cost.h            mfpf_ssd_vec_cost.cxx
  mfpf_log_lin_class_cost.h      mfpf_log_lin_class_cost.cxx
  mfpf_vec_cost_builder.h        mfpf_vec_cost_builder.cxx
  mfpf_sad_vec_cost_builder.h    mfpf_sad_vec_cost_builder.cxx
  mfpf_ssd_vec_cost_builder.h    mfpf_ssd_vec_cost_builder.cxx

  mfpf_sort_matches.h            mfpf_sort_matches.cxx
  mfpf_norm_vec.h
  mfpf_sample_region.h
  mfpf_region_form.h
  mfpf_pose.h
  mfpf_pose_set.h
  mfpf_feature_vec.h
  mfpf_searcher.h                mfpf_searcher.cxx
  mfpf_prune_overlaps.h          mfpf_prune_overlaps.cxx
  mfpf_patch_data.h              mfpf_patch_data.cxx

  mfpf_mr_point_finder.h         mfpf_mr_point_finder.cxx
  mfpf_mr_point_finder_builder.h mfpf_mr_point_finder_builder.cxx

  mfpf_draw_pose_cross.h         mfpf_draw_pose_cross.cxx
  mfpf_draw_pose_lines.h         mfpf_draw_pose_lines.cxx
  mfpf_draw_matches.h            mfpf_draw_matches.cxx

  mfpf_dp_snake.h                mfpf_dp_snake.cxx

  mfpf_region_definer.h          mfpf_region_definer.cxx
  mfpf_region_about_pt.h         mfpf_region_about_pt.cxx
  mfpf_region_about_lineseg.h    mfpf_region_about_lineseg.cxx

  mfpf_pose_predictor.h          mfpf_pose_predictor.cxx
  mfpf_pose_predictor_builder.h  mfpf_pose_predictor_builder.cxx
)

aux_source_directory(Templates mfpf_sources)
vxl_add_library(LIBRARY_NAME mfpf LIBRARY_SOURCES ${mfpf_sources} )
target_link_libraries(mfpf clsfy mipa)

if(VXL_BUILD_MUL_TOOLS)
  add_subdirectory(tools)
endif()

if( BUILD_TESTING)
  add_subdirectory(tests)
endif()
