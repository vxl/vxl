# brl/bpro/CMakeLists.txt
# main package for processing operations

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

# Basic process classes
add_subdirectory(bprb)

# Brown core processing
add_subdirectory(core)

# Brown batch processing (Python)
add_subdirectory(bpro_batch)

ENDIF(EXPAT_FOUND)
