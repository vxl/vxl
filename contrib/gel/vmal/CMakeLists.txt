# gel/vmal/CMakeLists.txt

doxygen_add_library(contrib/gel/vmal
  DEPENDS contrib/oxl/osl
  PACKAGE contrib/gel
  DESCRIPTION "Multi-View Line matching"
  )
if(VXL_BUILD_OXL)
set(vmal_sources
 vmal_dense_matching.cxx    vmal_dense_matching.h
 vmal_homog2d.cxx           vmal_homog2d.h
 vmal_lines_correlation.cxx vmal_lines_correlation.h
 vmal_operators.cxx         vmal_operators.h
 vmal_refine_lines.cxx      vmal_refine_lines.h
 vmal_track_lines.cxx       vmal_track_lines.h
 vmal_convert_vtol.cxx      vmal_convert_vtol.h
)
aux_source_directory(Templates vmal_sources)

 include_directories(${OXL_INCLUDE_DIR})
 set(vmal_sources ${vmal_sources}
 vmal_convert_osl.cxx       vmal_convert_osl.h
 vmal_rectifier.cxx         vmal_rectifier.h
 vmal_multi_view_data.cxx   vmal_multi_view_data.h
 vmal_multi_view_data_edge_sptr.h
 vmal_multi_view_data_vertex_sptr.h
)


vxl_add_library(LIBRARY_NAME vmal LIBRARY_SOURCES ${vmal_sources})

if(VXL_BUILD_OXL)
 target_link_libraries(vmal mvl osl ${VXL_LIB_PREFIX}vil)
endif()

target_link_libraries(vmal vtol ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vil1 ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vbl)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
endif()
