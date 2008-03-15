# brl/bseg/bbgm/pro/CMakeLists.txt

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bseg )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bpro )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
SET(bbgm_bpro_sources
    bbgm_load_image_of_process.h      bbgm_load_image_of_process.cxx 
    bbgm_save_image_of_process.h      bbgm_save_image_of_process.cxx 
    bbgm_update_dist_image_process.h  bbgm_update_dist_image_process.cxx
    bbgm_update_dist_image_stream_process.h
    bbgm_update_dist_image_stream_process.cxx
    bbgm_display_dist_image_process.h
    bbgm_display_dist_image_process.cxx
    bbgm_measure_process.h            bbgm_measure_process.cxx
)

AUX_SOURCE_DIRECTORY(Templates bbgm_bpro_sources)

ADD_LIBRARY(bbgm_bpro ${bbgm_bpro_sources})
TARGET_LINK_LIBRARIES(bbgm_bpro bbgm bprb brdb bsta_algo bsta brip vidl2 vil vsl)

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

ENDIF(EXPAT_FOUND)
