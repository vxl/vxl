# gel/vifa/CMakeLists.txt

doxygen_add_library(contrib/gel/vifa
  DEPENDS contrib/gel/gevd
  PACKAGE contrib/gel
  )

set(vifa_sources
 vifa_typedefs.h
 vifa_bbox.h
 vifa_gaussian.cxx              vifa_gaussian.h
 vifa_histogram.cxx             vifa_histogram.h
 vifa_image_histogram.cxx       vifa_image_histogram.h
 vifa_incr_var.cxx              vifa_incr_var.h
 vifa_coll_lines_params.cxx     vifa_coll_lines_params.h
 vifa_line_cover.cxx            vifa_line_cover.h
 vifa_group_pgram.cxx           vifa_group_pgram.h
 vifa_group_pgram_params.cxx    vifa_group_pgram_params.h
 vifa_imp_line.h                vifa_imp_line.hxx
 vifa_norm_params.cxx           vifa_norm_params.h
 vifa_parallel_params.cxx       vifa_parallel_params.h
 vifa_coll_lines.cxx            vifa_coll_lines.h
 vifa_parallel.cxx              vifa_parallel.h
 vifa_int_face_attr.cxx         vifa_int_face_attr.h
 vifa_int_face_attr_factory.h
 vifa_int_face_attr_common.cxx  vifa_int_face_attr_common.h
 vifa_int_face_attr_common_params.cxx vifa_int_face_attr_common_params.h
 vifa_int_faces_attr.cxx        vifa_int_faces_attr.h
 vifa_int_faces_adj_attr.cxx    vifa_int_faces_adj_attr.h
)
aux_source_directory(Templates vifa_sources)

vxl_add_library(LIBRARY_NAME vifa LIBRARY_SOURCES ${vifa_sources})
target_link_libraries(vifa gevd vtol vsol vdgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl)
