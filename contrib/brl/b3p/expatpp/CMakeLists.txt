#This is brl/b3p/expatpp

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if( EXPAT_FOUND )

  if( VXL_USING_NATIVE_EXPAT )
    include_directories( ${EXPAT_INCLUDE_DIR} )
  else()
    include_directories( ${BRL_BINARY_DIR}/b3p/expat )
  endif()

  set( expatpp_sources expatpp.h expatpp.cpp expatpplib.h )

  vxl_add_library(LIBRARY_NAME expatpp LIBRARY_SOURCES ${expatpp_sources} )
  target_link_libraries( expatpp ${EXPAT_LIBRARIES} )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif()


endif()
