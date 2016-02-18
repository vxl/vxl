# bbas/bvgl/algo/CMakeLists.txt

project(bvgl_algo)

# Set up bvgl_where_brl_lib_dir.h to have a macro set to $CMAKE_LIBRARY_OUTPUT_DIRECTORY
configure_file(
  ${brl_SOURCE_DIR}/bbas/bvgl/algo/bvgl_where_brl_lib_dir.h.in
  ${brl_BINARY_DIR}/bbas/bvgl/algo/bvgl_where_brl_lib_dir.h ESCAPE_QUOTES
)

# Copy the Eulerspiral lookup-table file to the BRL library folder
if(CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  configure_file(
    ${brl_SOURCE_DIR}/bbas/bvgl/algo/bvgl_eulerspiral_lookup_table.bvl
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/bvgl_eulerspiral_lookup_table.bvl COPYONLY
  )
endif()

add_definitions(-DBVGL_WHERE_BRL_LIB_DIR_H_EXISTS)
include_directories( ${brl_BINARY_DIR}/bbas/bvgl/algo/ )


set( bvgl_algo_sources
  bvgl_2d_geo_index_node_base.h     bvgl_2d_geo_index_node_base.cxx
  bvgl_2d_geo_index.h               bvgl_2d_geo_index.cxx
  bvgl_2d_geo_index_sptr.h
  bvgl_2d_geo_index.txx

  bvgl_where_brl_lib_dir.h.in
  bvgl_biarc.cxx               bvgl_biarc.h
  bvgl_eulerspiral.cxx         bvgl_eulerspiral.h
  bvgl_eulerspiral_base.cxx    bvgl_eulerspiral_base.h
)
aux_source_directory(Templates bvgl_algo_sources)

vxl_add_library( LIBNAME bvgl_algo LIBSRCS ${bvgl_algo_sources})
target_link_libraries(bvgl_algo bvgl bkml ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vsl ${VXL_LIB_PREFIX}vcl ${VXL_LIB_PREFIX}vnl bnl bnl_algo ${VXL_LIB_PREFIX}vnl_algo)


if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
