USES += contrib/brl contrib/gel contrib/brl/bbas contrib/brl/bseg core

LIBRARY := sdet
VERSION := 1.0

SOURCES += sdet_contour.cxx
SOURCES += sdet_region.cxx
MANPAGE_SOURCES += sdet_region_sptr.h
SOURCES += sdet_region_edge.cxx
MANPAGE_SOURCES += sdet_region_edge_sptr.h
SOURCES += sdet_edgel_regions.cxx

SOURCES += sdet_detector.cxx
SOURCES += sdet_detector_params.cxx
SOURCES += sdet_harris_detector.cxx
SOURCES += sdet_harris_detector_params.cxx
SOURCES += sdet_edge_champher.cxx
SOURCES += sdet_region_proc.cxx
SOURCES += sdet_region_proc_params.cxx
SOURCES += sdet_fit_conics.cxx
SOURCES += sdet_fit_conics_params.cxx
SOURCES += sdet_fit_lines.cxx
SOURCES += sdet_fit_lines_params.cxx
SOURCES += sdet_grid_finder.cxx
SOURCES += sdet_grid_finder_params.cxx
SOURCES += sdet_watershed_region_proc.cxx
SOURCES += sdet_watershed_region_proc_params.cxx
SOURCES += sdet_nonmax_suppression.cxx
SOURCES += sdet_nonmax_suppression_params.cxx
SOURCES += sdet_gauss_fit.cxx
SOURCES += sdet_img_edge.cxx
SOURCES += sdet_third_order_edge_det.cxx
SOURCES += sdet_third_order_edge_det_params.cxx
SOURCES += sdet_nms.cxx
SOURCES += sdet_image_mesh.cxx
SOURCES += sdet_image_mesh_params.cxx
SOURCES += sdet_denoise_mrf.cxx
SOURCES += sdet_denoise_mrf_params.cxx
SOURCES += sdet_mrf_site_bp.cxx
MANPAGE_SOURCES += sdet_mrf_site_bp_sptr.h
SOURCES += sdet_denoise_mrf_bp.cxx
SOURCES += sdet_denoise_mrf_bp_params.cxx
SOURCES += sdet_mrf_bp.cxx
MANPAGE_SOURCES += sdet_mrf_bp_sptr.h

#######################################
include ${VXLROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${VXLROOT}/config/top-rules.mk
######################################
