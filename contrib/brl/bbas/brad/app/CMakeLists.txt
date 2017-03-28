# Executable for SGM
# brl/bbas/brad/app/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/brad )

add_executable( brad_ms_app brad_ms_app.cxx)
target_link_libraries( brad_ms_app brad ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl )

add_executable( brad_create_sam_app brad_create_sam_app.cxx)
target_link_libraries( brad_create_sam_app brad ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl )

