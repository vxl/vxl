# Executable for SGM
# brl/bseg/bsgm/app/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/bseg )

add_executable( bsgm_app bsgm_app.cxx)
target_link_libraries( bsgm_app bsgm ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vcl )
