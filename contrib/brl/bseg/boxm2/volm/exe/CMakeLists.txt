# contrib/brl/bseg/boxm2_volm/exe/CMakeLists.txt
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg/boxm2/volm )

ADD_EXECUTABLE( boxm2_volumetric_matcher_exe
                boxm2_volumetric_matcher_exe.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_exe boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volm_prepare_location_hypotheses
                boxm2_volm_prepare_location_hypotheses.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volm_prepare_location_hypotheses boxm2_volm vbl vgl  )

ADD_EXECUTABLE( boxm2_volumetric_matcher_exe_per_gpu
                boxm2_volumetric_matcher_exe_per_gpu.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_exe_per_gpu boxm2_volm vbl vgl volm depth_map bvrml)

ADD_EXECUTABLE( boxm2_volumetric_matcher_compose
                boxm2_volumetric_matcher_compose.cxx
              )
TARGET_LINK_LIBRARIES( boxm2_volumetric_matcher_compose boxm2_volm vbl vgl volm depth_map bvrml)





