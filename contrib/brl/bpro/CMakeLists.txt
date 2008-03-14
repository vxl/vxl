# brl/bpro/CMakeLists.txt
# main package for processing operations

IF(HAS_BXML)

# Basic process classes
SUBDIRS(bprb)


# Python wrappers
SUBDIRS(bppy)

# Brown core processing
SUBDIRS(core)

ENDIF(HAS_BXML)
