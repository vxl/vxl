# brl/bpro/bprb/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bpro )


set(bprb_sources
   bprb_parameters.cxx             bprb_parameters.h            bprb_parameters_sptr.h
   bprb_parameters.txx
   bprb_process.cxx                bprb_process.h               bprb_process_sptr.h
   bprb_process_ext.cxx            bprb_process_ext.h
   bprb_process_manager.txx        bprb_process_manager.h
   bprb_batch_process_manager.cxx  bprb_batch_process_manager.h
   bprb_null_process.cxx           bprb_null_process.h
   bprb_func_process.h
   bprb_macros.h
)

aux_source_directory(Templates bprb_sources)

add_library(bprb ${bprb_sources})

target_link_libraries(bprb brdb bxml vbl vsl)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

#install the .h .txx and libs
install_targets(/lib bprb)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bprb/ ${bprb_sources})
