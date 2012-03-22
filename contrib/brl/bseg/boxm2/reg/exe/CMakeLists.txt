# contrib/brl/bseg/boxm2_multi/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/reg/ )

#exec for render
ADD_EXECUTABLE( boxm2_reg_mutual_info_exe
                boxm2_reg_mutual_info_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_reg_mutual_info_exe boxm2_reg boxm2 boxm2_ocl_pro vil_pro vpgl_pro vbl vgl) #boxm2_basic boxm2_io boxm2_ocl bocl vsph vil vul

ADD_EXECUTABLE( boxm2_compute_path_uncertainty_exe
                boxm2_compute_path_uncertainty_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_compute_path_uncertainty_exe vil vul vnl vbl vgl) 