# oxl/osl/CMakeLists.txt

doxygen_add_library(contrib/oxl/osl
  DEPENDS core/vnl core/vil1 core/vbl
  PACKAGE contrib/oxl
  DESCRIPTION "Oxford's segmentation library"
  )

set(osl_sources
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

    osl_1d_half_kernel.hxx          osl_1d_half_kernel.h
    osl_canny_smooth.hxx            osl_canny_smooth.h osl_canny_smooth.cxx
    osl_hacks.h
)
aux_source_directory(Templates osl_sources)

vxl_add_library(LIBRARY_NAME osl LIBRARY_SOURCES ${osl_sources})
target_link_libraries(osl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vbl)

if(VXL_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
