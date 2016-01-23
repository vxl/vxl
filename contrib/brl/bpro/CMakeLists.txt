# brl/bpro/CMakeLists.txt
# main package for processing operations

include( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
if(EXPAT_FOUND)

# Basic process classes
add_subdirectory(bprb)

# Brown core processing
add_subdirectory(core)

# Brown batch processing (Python)
add_subdirectory(bpro_batch)

endif()
