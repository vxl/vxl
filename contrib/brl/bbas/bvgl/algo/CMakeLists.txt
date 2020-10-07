# bbas/bvgl/algo/CMakeLists.txt

project(bvgl_algo)

# Install resource file
brl_resource_file("${CMAKE_CURRENT_SOURCE_DIR}/bvgl_eulerspiral_lookup_table.bvl"
                  contrib/brl/bbas/bvgl/algo)

set( bvgl_algo_sources
  bvgl_2d_geo_index_node_base.h     bvgl_2d_geo_index_node_base.cxx
  bvgl_2d_geo_index.h               bvgl_2d_geo_index.cxx
  bvgl_2d_geo_index_sptr.h
  bvgl_2d_geo_index.hxx

  bvgl_biarc.cxx               bvgl_biarc.h
  bvgl_eulerspiral.cxx         bvgl_eulerspiral.h
  bvgl_eulerspiral_base.hxx    bvgl_eulerspiral_base.h
  bvgl_register_ptsets_3d_rigid.hxx bvgl_register_ptsets_3d_rigid.h
)
aux_source_directory(Templates bvgl_algo_sources)

vxl_add_library(LIBRARY_NAME bvgl_algo LIBRARY_SOURCES ${bvgl_algo_sources})
target_link_libraries(bvgl_algo bres bvgl bkml ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl bnl bnl_algo ${VXL_LIB_PREFIX}vnl_algo)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
