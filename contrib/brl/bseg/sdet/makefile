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
SOURCES += sdet_fit_lines.cxx
SOURCES += sdet_fit_lines_params.cxx
SOURCES += sdet_grid_finder.cxx
SOURCES += sdet_grid_finder_params.cxx
SOURCES += sdet_vehicle_finder.cxx
SOURCES += sdet_vehicle_finder_params.cxx
SOURCES += sdet_watershed_region_proc.cxx
SOURCES += sdet_watershed_region_proc_params.cxx

#######################################
include ${IUEROOT}/config/top-params.mk
#######################################

ifndef NOTEST
 SUBDIRS += tests
endif

######################################
include ${IUEROOT}/config/top-rules.mk
######################################
