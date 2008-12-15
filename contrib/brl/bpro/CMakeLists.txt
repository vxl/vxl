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

# Brown bvgl processes
SUBDIRS(bvgl_pro)

# Brown batch processing (Python)
SUBDIRS(bpro_batch)

ENDIF(EXPAT_FOUND)
