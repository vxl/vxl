# vxl/contrib/brl/bbas/bxml/CMakeLists.txt
# The Development Brown XML Parser

doxygen_add_library(contrib/brl/bbas/bxml
  DEPENDS contrib/brl/bbas/bdgl core/vbl_io
  PACKAGE contrib/brl
  )

set( bxml_sources
    bxml_document.cxx            bxml_document.h
    bxml_write.cxx               bxml_write.h
    bxml_find.cxx                bxml_find.h
)

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )

if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  include_directories(${EXPAT_INCLUDE_DIRS} )
  include_directories(${EXPATPP_INCLUDE_DIRS} )
  set( bxml_sources ${bxml_sources}
      bxml_read.cxx                bxml_read.h
  )
 endif()
endif()

aux_source_directory(Templates bxml_sources)

vxl_add_library(LIBRARY_NAME bxml LIBRARY_SOURCES ${bxml_sources})

target_link_libraries( bxml ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vbl )
if(EXPAT_FOUND)
 if(EXPATPP_FOUND)
  target_link_libraries( bxml ${EXPATPP_LIBRARIES})
 endif()
endif()

add_subdirectory(bsvg)

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

#install the .h .hxx and libs
