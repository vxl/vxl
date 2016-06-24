# This is brl/seg/bmdl/CMakeLists.txt

if(HAS_GEOTIFF)
  include_directories( ${GEOTIFF_INCLUDE_DIR} )

doxygen_add_library(contrib/brl/bseg/bmdl
  DEPENDS contrib/brl/bbas/imesh core/vil
  PACKAGE contrib/brl
  DESCRIPTION "Brown Lidar Mesh Modeling Library"
  )

add_definitions(-DDEBUG_MEMORY=1)

set( bmdl_sources
  bmdl_classify.hxx    bmdl_classify.h
  bmdl_mesh.cxx        bmdl_mesh.h
)

aux_source_directory(Templates bmdl_sources)

vxl_add_library(LIBRARY_NAME bmdl LIBRARY_SOURCES  ${bmdl_sources})

target_link_libraries(bmdl ${VXL_LIB_PREFIX}vil_algo imesh ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil)

#process lib
add_subdirectory(pro)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()
else()
  message(STATUS "Skipping contrib/brl/bseg/bmdl: requires geotiff")
endif()
