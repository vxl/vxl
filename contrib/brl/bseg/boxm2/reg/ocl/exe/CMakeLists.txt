# contrib/brl/bseg/boxm2_multi/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/reg/ocl )

ADD_EXECUTABLE( boxm2_ocl_reg_mutual_info_exe
                boxm2_ocl_reg_mutual_info_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_ocl_reg_mutual_info_exe boxm2_ocl_reg  vbl vgl)

ADD_EXECUTABLE( boxm2_ocl_depth_map_to_vol_exe
                boxm2_ocl_depth_map_to_vol_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_ocl_depth_map_to_vol_exe boxm2_ocl_reg  vbl vgl)

ADD_EXECUTABLE( boxm2_ocl_depth_map_to_vol_xyz_image
                boxm2_ocl_depth_map_to_vol_xyz_image.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_ocl_depth_map_to_vol_xyz_image boxm2_ocl_reg  vbl vgl)

ADD_EXECUTABLE( boxm2_ocl_depth_map_to_vol_xyz_image2
                boxm2_ocl_depth_map_to_vol_xyz_image2.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_ocl_depth_map_to_vol_xyz_image2 boxm2_ocl_reg  vbl vgl)

ADD_EXECUTABLE( boxm2_ocl_compare_xforms
                boxm2_ocl_compare_xforms.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_ocl_compare_xforms  vbl vgl vul vnl)
