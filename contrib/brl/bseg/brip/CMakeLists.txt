# Brown image processing library
# brl/bseg/brip/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/brip
  DEPENDS contrib/brl/bbas/bsol core/vil1 core/vil core/vbl
  PACKAGE contrib/brl
  DESCRIPTION "Brown Image Processing Library"
  )

INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES(${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES(${GEL_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR}/mrc )

SET(brip_sources
   brip_histogram.h          brip_histogram.txx
   brip_mutual_info.h        brip_mutual_info.txx
   brip_vil1_float_ops.h     brip_vil1_float_ops.cxx
   brip_vil_float_ops.h      brip_vil_float_ops.cxx
   brip_vil_ops.h
   brip_para_cvrg_params.h   brip_para_cvrg_params.cxx
   brip_para_cvrg.h          brip_para_cvrg.cxx
   brip_region_pixel.h       brip_region_pixel.cxx     brip_region_pixel_sptr.h
   brip_watershed_params.h   brip_watershed_params.cxx
   brip_watershed.h          brip_watershed.cxx
   brip_roi.h                brip_roi.cxx              brip_roi_sptr.h
   brip_quadtree_node.h      brip_quadtree_node_base_sptr.h
   brip_max_scale_response.h brip_max_scale_response.txx
   brip_quadtree_utils.h     brip_quadtree_utils.txx
   brip_rect_mask.h          brip_rect_mask.cxx
   brip_line_generator.h     brip_line_generator.cxx
   brip_label_equivalence.h  brip_label_equivalence.cxx
   brip_kernel.h
   brip_interp_kernel.h
   brip_gaussian_kernel.h
   brip_blobwise_mutual_info.h brip_blobwise_mutual_info.txx
   brip_filter_bank.h        brip_filter_bank.cxx
   brip_gain_offset_solver.h    brip_gain_offset_solver.cxx
)
AUX_SOURCE_DIRECTORY(Templates brip_sources)

ADD_LIBRARY(brip ${brip_sources})
INSTALL_TARGETS(/lib brip)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/brip ${brip_sources})

TARGET_LINK_LIBRARIES(brip gevd bsta bsol vsol vil1 vil_algo vil vgl_algo vgl vnl_algo vnl vbl vul vpgl bil_algo)

IF(BUILD_TESTING)
  SUBDIRS(tests)
ENDIF(BUILD_TESTING)
