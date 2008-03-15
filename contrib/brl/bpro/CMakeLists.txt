# brl/bpro/CMakeLists.txt
# main package for processing operations

INCLUDE( ${BRL_MODULE_PATH}/FindEXPAT.cmake )
IF(EXPAT_FOUND)

# Basic process classes
SUBDIRS(bprb)

# Python wrappers
SUBDIRS(bppy)

# Brown core processing
SUBDIRS(core)

ENDIF(EXPAT_FOUND)
