# oxl/osl/CMakeLists.txt

SET(osl_sources
    osl_topology.cxx                osl_topology.h
    osl_vertex.cxx                  osl_vertex.h
    osl_edge.cxx                    osl_edge.h
    osl_edgel_chain.cxx             osl_edgel_chain.h
    osl_break_edge.cxx              osl_break_edge.h
    osl_canny_port.h
    osl_canny_base.cxx              osl_canny_base.h
    osl_canny_gradient.cxx          osl_canny_gradient.h
    osl_canny_nms.cxx               osl_canny_nms.h
    osl_canny_ox.cxx                osl_canny_ox.h
    osl_canny_ox_params.cxx         osl_canny_ox_params.h
    osl_canny_rothwell.cxx          osl_canny_rothwell.h
    osl_canny_rothwell_params.h
    osl_chamfer.cxx                 osl_chamfer.h
    osl_convolve.cxx                osl_convolve.h
    osl_easy_canny.cxx              osl_easy_canny.h
    osl_edge_detector.cxx           osl_edge_detector.h
    osl_edge_detector_params.cxx    osl_edge_detector_params.h
    osl_fit_circle.cxx              osl_fit_circle.h
    osl_fit_lines.cxx               osl_fit_lines.h
    osl_fit_lines_params.cxx        osl_fit_lines_params.h
    osl_harris.cxx                  osl_harris.h
    osl_harris_params.cxx           osl_harris_params.h
    osl_kernel.cxx                  osl_kernel.h
    osl_load_topology.cxx           osl_load_topology.h
    osl_ortho_regress.cxx           osl_ortho_regress.h osl_OrthogRegress.h
    osl_roi_window.h
    osl_save_topology.cxx           osl_save_topology.h
    internals/droid.cxx             internals/droid.h
    internals/osl_reorder_chain.cxx internals/osl_reorder_chain.h

    osl_1d_half_kernel.txx          osl_1d_half_kernel.h
    osl_canny_smooth.txx            osl_canny_smooth.h osl_canny_smooth.cxx
    osl_hacks.h
)
AUX_SOURCE_DIRECTORY(Templates osl_sources)

ADD_LIBRARY(osl ${osl_sources})
TARGET_LINK_LIBRARIES(osl vnl_algo vnl vil1 vbl)

IF (BUILD_EXAMPLES)
  SUBDIRS(examples)
ENDIF (BUILD_EXAMPLES)

IF (BUILD_TESTING)
  SUBDIRS(tests)
ENDIF (BUILD_TESTING)
