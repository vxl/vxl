#
# ./mul/mfpf/CMakeLists.txt
#
#   Tim Cootes
#

SET(mfpf_sources
  mfpf_add_all_loaders.h         mfpf_add_all_loaders.cxx
  mfpf_point_finder_builder.h    mfpf_point_finder_builder.cxx
  mfpf_edge_finder_builder.h     mfpf_edge_finder_builder.cxx
  mfpf_point_finder.h            mfpf_point_finder.cxx
  mfpf_edge_finder.h             mfpf_edge_finder.cxx
  mfpf_norm_corr1d.h             mfpf_norm_corr1d.cxx
  mfpf_norm_corr1d_builder.h     mfpf_norm_corr1d_builder.cxx
  mfpf_norm_corr2d.h             mfpf_norm_corr2d.cxx
  mfpf_norm_corr2d_builder.h     mfpf_norm_corr2d_builder.cxx
  mfpf_profile_pdf.h             mfpf_profile_pdf.cxx
  mfpf_profile_pdf_builder.h     mfpf_profile_pdf_builder.cxx
  mfpf_region_pdf.h              mfpf_region_pdf.cxx
  mfpf_region_pdf_builder.h      mfpf_region_pdf_builder.cxx
  mfpf_sort_matches.h            mfpf_sort_matches.cxx
  mfpf_norm_vec.h
  mfpf_sample_region.h
  mfpf_region_form.h
  mfpf_pose.h
  mfpf_pose_set.h
  mfpf_searcher.h                mfpf_searcher.cxx
  mfpf_prune_overlaps.h          mfpf_prune_overlaps.cxx

  mfpf_mr_point_finder.h         mfpf_mr_point_finder.cxx
  mfpf_mr_point_finder_builder.h mfpf_mr_point_finder_builder.cxx

  mfpf_draw_pose_cross.h         mfpf_draw_pose_cross.cxx
  mfpf_draw_pose_lines.h         mfpf_draw_pose_lines.cxx
  mfpf_draw_matches.h            mfpf_draw_matches.cxx
)

AUX_SOURCE_DIRECTORY(Templates mfpf_sources)
ADD_LIBRARY(mfpf ${mfpf_sources} )
TARGET_LINK_LIBRARIES(mfpf vimt vpdfl mbl)

#SUBDIRS(tools)

IF( BUILD_TESTING)
  SUBDIRS(tests)
ENDIF( BUILD_TESTING)
