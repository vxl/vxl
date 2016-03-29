# contrib/brl/bbas/bxml/bsvg/CMakeLists.txt
# Brown SVG Library, create an XML document with SVG primitives and display them using any browser

include_directories( ${BRL_INCLUDE_DIR}/bbas )

set( bsvg_sources
  bsvg_document.cxx bsvg_document.h
  bsvg_element.cxx bsvg_element.h
  bsvg_plot.h bsvg_plot.cxx
)

aux_source_directory(Templates bsvg_sources)

vxl_add_library(LIBRARY_NAME bsvg LIBRARY_SOURCES ${bsvg_sources})

target_link_libraries( bsvg bxml )

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

add_subdirectory(pro)