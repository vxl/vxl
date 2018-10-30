# contrib/brl/bseg/betr_batch/CMakeLists.txt
project( betr_batch )

include( ${VXL_CMAKE_DIR}/NewCMake/FindPython.cmake )

if( PYTHON_FOUND )
  include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
  if( EXPAT_FOUND )
    include_directories( ${PYTHON_INCLUDE_DIRS} )
    include_directories( ${BRL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bpro )
    include_directories( ${BRL_INCLUDE_DIR}/bpro/core )

    include_directories( ${VXLCORE_INCLUDE_DIR} )
    include_directories( ${MUL_INCLUDE_DIR} )
    include_directories( ${BRL_INCLUDE_DIR}/bseg )
    include_directories( ${BRL_INCLUDE_DIR}/bseg/betr )
    include_directories( ${BRL_INCLUDE_DIR}/bbas)

     set(betr_batch_sources
         reg_betr.h   reg_betr.cxx
        )

    if(NOT VXL_BUILD_POSITION_DEPENDENT_CODE)
      vxl_add_library(LIBRARY_NAME betr_batch LIBRARY_SOURCES SHARED ${betr_batch_sources})

      #library has to have different name depending on debug or release version.
      if(WIN32)
        if(NOT CYGWIN)
          set_target_properties(betr_batch PROPERTIES OUTPUT_NAME betr_batch DEBUG_POSTFIX _d SUFFIX .pyd)
        endif()
      endif()

      set_target_properties(betr_batch PROPERTIES PREFIX "")

      target_link_libraries(betr_batch betr_pro vpgl_pro vil_pro brip_pro sdet_pro brad_pro brad_io bprb brdb bpro_batch)

      if(PYTHON_DEBUG_LIBRARIES)
        target_link_libraries( betr_batch debug ${PYTHON_DEBUG_LIBRARY})
      endif()

      if(PYTHON_LIBRARIES)
        target_link_libraries( betr_batch optimized ${PYTHON_LIBRARY})
      endif()


      #install the .h .hxx and libs

 #     if( BUILD_TESTING )
 #       add_subdirectory(tests)
 #     endif()
    endif()
  endif()
endif()


