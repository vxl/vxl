# Executable for SGM
# brl/bseg/baml/app/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/baml )

add_executable( baml_app baml_app.cxx)
target_link_libraries( baml_app baml ${VXL_LIB_PREFIX}vul ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vpgl)
