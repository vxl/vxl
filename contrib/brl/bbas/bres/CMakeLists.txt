# brl/bbas/bres/CMakeLists.txt
# Resource locating utilities for Brown University Libraries

# Configure bres_where.h
vxl_configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/bres_where.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/bres_where.h
    include/vxl/contrib/brl/bbas/bres
)
add_definitions(-DBRES_WHERE_H_EXISTS)

set(bres_sources
  bres_find.h bres_find.cxx
)

vxl_add_library(
    LIBRARY_NAME bres
    HEADER_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}"
    LIBRARY_SOURCES  ${bres_sources}
)

target_link_libraries(bres)
