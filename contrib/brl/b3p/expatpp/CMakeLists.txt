#This is brl/b3p/expatpp

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if( EXPAT_FOUND )

  if( VXL_USING_NATIVE_EXPAT )
    include_directories( ${EXPAT_INCLUDE_DIR} )
  else( VXL_USING_NATIVE_EXPAT )
    include_directories( ${BRL_BINARY_DIR}/b3p/expat )
  endif( VXL_USING_NATIVE_EXPAT )

  set( expatpp_sources expatpp.h expatpp.cpp expatpplib.h )

  add_library( expatpp ${expatpp_sources} )
  target_link_libraries( expatpp ${EXPAT_LIBRARIES} )

  if( BUILD_TESTING )
    add_subdirectory(tests)
  endif( BUILD_TESTING )

  install_targets(/lib expatpp)
  INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/b3p/expatpp/ ${expatpp_sources})

endif( EXPAT_FOUND )
