# brl/bbas/CMakeLists.txt
# main package for basic operations common to all libraries
# All libraries are mutually independent, except for bxml <-- bdgl

# spatial object algorithms
SUBDIRS(bsol)

# topology algorithms
SUBDIRS(btol)

# digital library algorithms
SUBDIRS(bdgl)

# xml<->vxl conversion 
SUBDIRS(bxml)

# basic vgui classes
SUBDIRS(bgui)

# staged vgl development area
SUBDIRS(bvgl)

# uncertainty geometry library
SUBDIRS(bugl)

# Additions to vnl
SUBDIRS(bnl)

# Brown Graph Library
SUBDIRS(bgrl)

# Brown statistics library
SUBDIRS(bsta)

# Brown dynamic programming library
SUBDIRS(bdpg)

OPTION( BUILD_CONTRIB_VIDL2 "Build Experimental Video Streaming Library" YES )
IF(BUILD_CONTRIB_VIDL2)
# Video Library (rewrite)
SUBDIRS(vidl2)
ENDIF(BUILD_CONTRIB_VIDL2)
