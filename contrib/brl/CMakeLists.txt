# brl/CMakeLists.txt
# Mutual dependencies:  bbas --> bseg --> vvid  and  bbas --> bmvl

PROJECT( brl )

SET( BRL_INCLUDE_DIR ${brl_SOURCE_DIR} CACHE STATIC "Brown University package include files" )

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

# basic services package
SUBDIRS(bbas)

# segmentation package
SUBDIRS(bseg)

# video view library
SUBDIRS(vvid)

# multiview library
SUBDIRS(bmvl)

# standalone executables
SUBDIRS(bexe)


# brown third part library
SUBDIRS(b3p)
