# ./oxl/osl/CMakeLists.txt

ADD_LIBRARY(osl osl_sources)

SOURCE_FILES(osl_sources
  osl_topology.cxx
  osl_vertex.cxx
  osl_edge.cxx
  osl_edgel_chain.cxx
  osl_OrthogRegress.cxx
  osl_break_edge.cxx
  osl_canny_base.cxx
  osl_canny_gradient.cxx
  osl_canny_nms.cxx
  osl_canny_ox.cxx
  osl_canny_ox_params.cxx
  osl_canny_port.cxx
  osl_canny_rothwell.cxx
  osl_canny_rothwell_params.cxx
  osl_canny_smooth.cxx
  osl_chamfer.cxx
  osl_convolve.cxx
  osl_easy_canny.cxx
  osl_edge_detector.cxx
  osl_edge_detector_params.cxx
  osl_fit_circle.cxx
  osl_fit_lines.cxx
  osl_fit_lines_params.cxx
  osl_harris.cxx
  osl_harris_params.cxx
  osl_kernel.cxx
  osl_load_topology.cxx
  osl_ortho_regress.cxx
  osl_roi_window.cxx
  osl_save_topology.cxx
  internals/droid.cxx
  internals/osl_reorder_chain.cxx
)

AUX_SOURCE_DIRECTORY(Templates osl_sources)

IF (WIN32)
  IF (NOT CYGWIN)
    INCLUDE(${allvxl_SOURCE_DIR}/oxl/osl/CMakeListsHeaders.txt)
  ENDIF (NOT CYGWIN)
ENDIF(WIN32)

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
