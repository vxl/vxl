# prip/vdtop/CMakeLists.txt

SET(vdtop_sources
    vdtop_4_lower_homotopic_kernel.h
    vdtop_4_lower_leveling_kernel.h
    vdtop_4_upper_homotopic_kernel.h
    vdtop_4_upper_leveling_kernel.h
    vdtop_8_lower_homotopic_kernel.h
    vdtop_8_lower_leveling_kernel.h
    vdtop_8_neighborhood_mask.cxx vdtop_8_neighborhood_mask.h
    vdtop_8_upper_homotopic_kernel.h
    vdtop_8_upper_leveling_kernel.h
    vdtop_construct_well_composed.h
    vdtop_draw_digital_graph.h
    vdtop_freeman_code.cxx vdtop_freeman_code.h
    vdtop_image_view.h
    vdtop_kernel.h
    vdtop_misc.txx
    vdtop_neighborhood.h
    vdtop_pixel.h vdtop_pixel.txx
    vdtop_replace_quasi_8_minima.h
    vdtop_set_4_veinerization_structure.h vdtop_set_4_veinerization_structure.txx
    vdtop_set_8_veinerization_structure.h vdtop_set_8_veinerization_structure.txx
    vdtop_set_structure_from_digital_graph.h vdtop_set_structure_from_digital_graph.txx
    vdtop_well_composed_lower_leveling_kernel.h
    vil_canny_deriche_grad_filter.h
    vil_clamp.h
)

ADD_LIBRARY(vdtop ${vdtop_sources})

LINK_LIBRARIES(vdtop vbl vcl)

#IF (BUILD_TESTING)
  SUBDIRS(tests)
#ENDIF (BUILD_TESTING)
