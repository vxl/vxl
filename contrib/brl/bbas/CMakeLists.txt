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
SUBDIRS(bvnl)

# Brown Graph Library
SUBDIRS(bgrl)
