#depends on OPENCL being found...
include_directories( ${OPENCL_INCLUDE_PATH} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR} )
set(bstm_ocl_algo_sources
    bstm_ocl_render_expected_image_function.h  bstm_ocl_render_expected_image_function.cxx
    bstm_ocl_minfo_function.h bstm_ocl_minfo_function.cxx
    bstm_ocl_particle_filter.h bstm_ocl_particle_filter.cxx
    bstm_ocl_annealed_particle_filter.h bstm_ocl_annealed_particle_filter.cxx
    bstm_ocl_update.h bstm_ocl_update.cxx
    bstm_ocl_update_color.h bstm_ocl_update_color.cxx
    bstm_ocl_update_viewdep.h bstm_ocl_update_viewdep.cxx
    bstm_ocl_change_detection.h bstm_ocl_change_detection.cxx
   )

add_library(bstm_ocl_algo  ${bstm_ocl_algo_sources})
target_link_libraries(bstm_ocl_algo bstm_ocl bstm_cpp_algo boxm2_ocl_algo vgl vil vul bsta)

install_targets(/lib bstm_ocl_algo)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bseg/bstm/ocl/algo ${bstm_ocl_algo_sources})

if( BUILD_TESTING )
  add_subdirectory(tests)
endif( BUILD_TESTING )
