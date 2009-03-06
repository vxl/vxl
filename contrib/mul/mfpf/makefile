USES += contrib/mul core

LIBRARY := mfpf
VERSION := 1.0

SOURCES += mfpf_add_all_loaders.cxx
SOURCES += mfpf_edge_finder.cxx
SOURCES += mfpf_edge_finder_builder.cxx
SOURCES += mfpf_norm_corr1d.cxx
SOURCES += mfpf_norm_corr1d_builder.cxx
SOURCES += mfpf_norm_corr2d.cxx
SOURCES += mfpf_norm_corr2d_builder.cxx
SOURCES += mfpf_point_finder.cxx
SOURCES += mfpf_point_finder_builder.cxx
SOURCES += mfpf_profile_pdf.cxx
SOURCES += mfpf_profile_pdf_builder.cxx
SOURCES += mfpf_region_pdf.cxx
SOURCES += mfpf_region_pdf_builder.cxx
MANPAGE_SOURCES += mfpf_norm_vec.h
MANPAGE_SOURCES += mfpf_sample_region.h
SOURCES += mfpf_searcher.cxx
SOURCES += mfpf_draw_pose_cross.cxx
MANPAGE_SOURCES += mfpf_pose.h
SOURCES += mfpf_draw_pose_lines.cxx
SOURCES += mfpf_draw_matches.cxx
MANPAGE_SOURCES += mfpf_pose_set.h
MANPAGE_SOURCES += mfpf_region_form.h
SOURCES += mfpf_mr_point_finder.cxx
SOURCES += mfpf_mr_point_finder_builder.cxx
SOURCES += mfpf_prune_overlaps.cxx
SOURCES += mfpf_sort_matches.cxx
SOURCES += mfpf_region_finder.cxx
SOURCES += mfpf_region_finder_builder.cxx
SOURCES += mfpf_sad_vec_cost.cxx
SOURCES += mfpf_sad_vec_cost_builder.cxx
SOURCES += mfpf_vec_cost.cxx
SOURCES += mfpf_vec_cost_builder.cxx
MANPAGE_SOURCES += mfpf_feature_vec.h
SOURCES += mfpf_dp_snake.cxx
SOURCES += mfpf_max_finder.cxx
SOURCES += mfpf_ssd_vec_cost.cxx
SOURCES += mfpf_ssd_vec_cost_builder.cxx
SOURCES += mfpf_hog_box_finder.cxx
SOURCES += mfpf_hog_box_finder_builder.cxx
SOURCES += mfpf_region_about_lineseg.cxx
SOURCES += mfpf_region_definer.cxx
SOURCES += mfpf_patch_data.cxx
SOURCES += mfpf_region_about_pt.cxx

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
