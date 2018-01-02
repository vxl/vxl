# segmentation detection library
# brl/bseg/sdet/CMakeLists.txt

doxygen_add_library(contrib/brl/bseg/sdet
  DEPENDS contrib/brl/bseg/brip contrib/brl/bbas/btol core/vil1
  PACKAGE contrib/brl
  )

include_directories(${BRL_INCLUDE_DIR} )
include_directories(${BRL_INCLUDE_DIR}/bseg )
include_directories(${VXL_MUL_INCLUDE_DIR} )
include_directories(${GEL_INCLUDE_DIR} )
include_directories(${VTHREEP_INCLUDE_DIR})

set(sdet_sources
    sdet_edge_champher.cxx               sdet_edge_champher.h
    sdet_region_edge.cxx                 sdet_region_edge.h  sdet_region_edge_sptr.h
    sdet_edgel_regions.cxx               sdet_edgel_regions.h
    sdet_contour.cxx                     sdet_contour.h
    sdet_detector_params.cxx             sdet_detector_params.h
    sdet_detector.cxx                    sdet_detector.h
    sdet_region_proc_params.cxx          sdet_region_proc_params.h
    sdet_region_proc.cxx                 sdet_region_proc.h
    sdet_harris_detector_params.cxx      sdet_harris_detector_params.h
    sdet_harris_detector.cxx             sdet_harris_detector.h
    sdet_fit_lines.cxx                   sdet_fit_lines.h
    sdet_fit_lines_params.cxx            sdet_fit_lines_params.h
    sdet_fit_conics.cxx                  sdet_fit_conics.h
    sdet_fit_conics_params.cxx           sdet_fit_conics_params.h
    sdet_grid_finder_params.cxx          sdet_grid_finder_params.h
    sdet_grid_finder.cxx                 sdet_grid_finder.h
    sdet_region.cxx                      sdet_region.h       sdet_region_sptr.h
    sdet_watershed_region_proc_params.cxx sdet_watershed_region_proc_params.h
    sdet_watershed_region_proc.cxx       sdet_watershed_region_proc.h
    sdet_nonmax_suppression_params.cxx   sdet_nonmax_suppression_params.h
    sdet_nonmax_suppression.cxx          sdet_nonmax_suppression.h
    sdet_gauss_fit.cxx                   sdet_gauss_fit.h
    sdet_img_edge.cxx                    sdet_img_edge.h
    sdet_nms.cxx                         sdet_nms.h
    sdet_third_order_edge_det_params.cxx sdet_third_order_edge_det_params.h
    sdet_third_order_edge_det.cxx        sdet_third_order_edge_det.h
    sdet_image_mesh.cxx                  sdet_image_mesh.h
    sdet_image_mesh_params.cxx           sdet_image_mesh_params.h
    sdet_denoise_mrf.cxx                 sdet_denoise_mrf.h
    sdet_denoise_mrf_params.cxx          sdet_denoise_mrf_params.h
    sdet_mrf_site_bp.cxx                 sdet_mrf_site_bp.h  sdet_mrf_site_bp_sptr.h
    sdet_mrf_bp.cxx                      sdet_mrf_bp.h       sdet_mrf_bp_sptr.h
    sdet_denoise_mrf_bp.cxx              sdet_denoise_mrf_bp.h
    sdet_denoise_mrf_bp_params.cxx       sdet_denoise_mrf_bp_params.h
    sdet_vrml_display.cxx                sdet_vrml_display.h
    sdet_texture_classifier.cxx          sdet_texture_classifier.h
    sdet_texture_classifier_params.cxx   sdet_texture_classifier_params.h
    sdet_atmospheric_image_classifier.cxx
    sdet_atmospheric_image_classifier.h
    sdet_graph_img_seg.cxx               sdet_graph_img_seg.h sdet_graph_img_seg_sptr.h
    sdet_selective_search.h              sdet_selective_search.cxx
    sdet_selective_search_params.h       sdet_selective_search_params.cxx
    sdet_region_classifier.h             sdet_region_classifier.cxx
    sdet_region_classifier_params.h      sdet_region_classifier_params.cxx
# SEL Class
    sdet_sel.cxx                         sdet_sel.h
    sdet_sel_base.cxx                    sdet_sel_base.h
# SEL Others
    sdet_appearance.cxx                  sdet_appearance.h
    sdet_curvelet.cxx                    sdet_curvelet.h
    sdet_curvelet_map.cxx                sdet_curvelet_map.h
    sdet_curve_model.cxx                 sdet_curve_model.h
    sdet_edgel.cxx                       sdet_edgel.h
    sdet_EMD.cxx                         sdet_EMD.h
# Symbolic Edge Linker
    sdet_symbolic_edge_linker_params.cxx sdet_symbolic_edge_linker_params.h
    sdet_symbolic_edge_linker.cxx        sdet_symbolic_edge_linker.h
)
aux_source_directory(Templates sdet_sources)

vxl_add_library(LIBRARY_NAME sdet LIBRARY_SOURCES ${sdet_sources})

target_link_libraries(sdet brip bsol btol bdgl bvgl_algo bnl gevd vdgl vtol vsol imesh_algo imesh ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl bvgl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vbl_io bil_algo ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vnl_io)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
