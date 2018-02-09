# Executable for SGM
# brl/bseg/betr/app/CMakeLists.txt

include_directories( ${BRL_INCLUDE_DIR}/betr)

add_executable( betr_pixelwise_app betr_pixelwise_app.cxx)
target_link_libraries( betr_pixelwise_app betr ${VXL_LIB_PREFIX}vsph ${VXL_LIB_PREFIX}vsol ${VXL_LIB_PREFIX}bmsh3d ${VXL_LIB_PREFIX}bmsh3d_algo ${VXL_LIB_PREFIX}sdet ${VXL_LIB_PREFIX}brip ${VXL_LIB_PREFIX}vdgl ${VXL_LIB_PREFIX}vpgl ${VXL_LIB_PREFIX}vpgl_algo ${VXL_LIB_PREFIX}vcsl ${VXL_LIB_PREFIX}vnl_algo ${VXL_LIB_PREFIX}vnl ${VXL_LIB_PREFIX}vil_algo ${VXL_LIB_PREFIX}vil_io ${VXL_LIB_PREFIX}vil ${VXL_LIB_PREFIX}vgl_algo ${VXL_LIB_PREFIX}vgl ${VXL_LIB_PREFIX}vbl_io ${VXL_LIB_PREFIX}vbl ${VXL_LIB_PREFIX}baml ${VXL_LIB_PREFIX}bjson ${VXL_LIB_PREFIX}vpl)
