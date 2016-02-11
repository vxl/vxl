#PUBLIC RELEASE APPROVAL FROM AFRL
#Case Number: RY-14-0126
#PA Approval Number: 88ABW-2014-1143
# contrib/brl/bseg/boxm2_multi/exe/CMakeLists.txt
include_directories( ${BRL_INCLUDE_DIR} )
include_directories( ${BRL_INCLUDE_DIR}/bbas )
include_directories( ${BRL_INCLUDE_DIR}/bseg/boxm2/reg/ocl )

add_executable( boxm2_ocl_reg_mutual_info_exe
                boxm2_ocl_reg_mutual_info_exe.cxx)
target_link_libraries( boxm2_ocl_reg_mutual_info_exe boxm2_ocl_reg ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl)


add_executable( boxm2_ocl_reg_points_to_volume_exe
                boxm2_ocl_reg_points_to_volume_exe.cxx)
target_link_libraries( boxm2_ocl_reg_points_to_volume_exe boxm2_ocl_reg ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}vgl)
