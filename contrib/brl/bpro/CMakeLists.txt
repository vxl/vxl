# brl/bpro/CMakeLists.txt
# main package for processing operations

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

# Basic process classes
SUBDIRS(bprb)

# Brown core processing
SUBDIRS(core)

# Brown batch processing (Python)
SUBDIRS(bpro_batch)

ENDIF(EXPAT_FOUND)
