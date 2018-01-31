# Executable for SGM
# brl/bseg/brip/app/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/brad )
include_directories(${GEL_INCLUDE_DIR})

add_executable( brip_find_blob_app brip_find_blob_app.cxx)
target_link_libraries( brip_find_blob_app brip ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}brad )


