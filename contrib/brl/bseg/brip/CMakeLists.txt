# Brown image processing library
# brl/bseg/brip/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/brip
  DEPENDS contrib/brl/bbas/bsol core/vil1 core/vil core/vbl
  PACKAGE contrib/brl
  DESCRIPTION "Brown Image Processing Library"
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bseg )
 include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories(${GEL_INCLUDE_DIR})
include_directories( ${GEL_INCLUDE_DIR}/mrc )

set(brip_sources
   brip_histogram.h          brip_histogram.hxx
   brip_mutual_info.h        brip_mutual_info.hxx
   brip_vil1_float_ops.h     brip_vil1_float_ops.cxx
   brip_vil_float_ops.h      brip_vil_float_ops.cxx
   brip_vil_nitf_ops.h       brip_vil_nitf_ops.cxx
   brip_vil_ops.h
   brip_para_cvrg_params.h   brip_para_cvrg_params.cxx
   brip_para_cvrg.h          brip_para_cvrg.cxx
   brip_region_pixel.h       brip_region_pixel.cxx     brip_region_pixel_sptr.h
   brip_watershed_params.h   brip_watershed_params.cxx
   brip_watershed.h          brip_watershed.cxx
   brip_roi.h                brip_roi.cxx              brip_roi_sptr.h
   brip_quadtree_node.h      brip_quadtree_node_base_sptr.h
   brip_max_scale_response.h brip_max_scale_response.hxx
   brip_quadtree_utils.h     brip_quadtree_utils.hxx
   brip_rect_mask.h          brip_rect_mask.cxx
   brip_line_generator.h     brip_line_generator.cxx
   brip_label_equivalence.h  brip_label_equivalence.cxx
   brip_kernel.h
   brip_interp_kernel.h
   brip_gaussian_kernel.h
   brip_blobwise_mutual_info.h brip_blobwise_mutual_info.hxx
   brip_filter_bank.h        brip_filter_bank.cxx
   brip_gain_offset_solver.h    brip_gain_offset_solver.cxx
   brip_phase_correlation.h    brip_phase_correlation.cxx
   brip_ms_blob_detection.h    brip_ms_blob_detection.cxx
)
aux_source_directory(Templates brip_sources)

vxl_add_library(LIBRARY_NAME brip LIBRARY_SOURCES ${brip_sources})

target_link_libraries(brip brad gevd bsta bsol vsol ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vpgl bil_algo)

add_subdirectory(app)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
