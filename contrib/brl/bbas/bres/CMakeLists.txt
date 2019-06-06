# brl/bbas/bres/CMakeLists.txt
# Resource locating utilities for Brown University Libraries

# Configure bres_where.h
vxl_configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bres_where.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/bres_where.h include/vxl/bbas/bres
)

set(bres_sources
  bres_find.h bres_find.cxx
)

# include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

# if(EXPAT_FOUND)
#  if(EXPATPP_FOUND)
#   include_directories(${EXPAT_INCLUDE_DIRS} )
#   include_directories(${EXPATPP_INCLUDE_DIRS} )
#   set(bkml_sources ${bkml_sources}
#      bkml_parser.cxx    bkml_parser.h
#   )
#  endif()
# endif()

vxl_add_library(LIBRARY_NAME bres LIBRARY_SOURCES  ${bres_sources})

target_link_libraries(bres) #${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl)

# if(EXPAT_FOUND)
#   target_link_libraries(bkml ${EXPAT_LIBRARIES})
#   if(APPLE)
#     target_link_libraries(bkml expat)
#   endif()
#   if(EXPATPP_FOUND)
#     target_link_libraries(bkml ${EXPATPP_LIBRARIES})
#   endif()
# endif()

# if( BUILD_TESTING )
#   add_subdirectory(tests)
# endif()

#add_subdirectory(pro)
