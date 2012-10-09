# brl/bbas/CMakeLists.txt
# main package for basic operations common to all libraries
# All libraries are mutually independent, except for bxml <-- bdgl

# xml<->vxl conversion
SUBDIRS(bxml)

# Uncertainty geometry library
SUBDIRS(bugl)

# Additions to vnl
SUBDIRS(bnl)

# Brown Graph Library
SUBDIRS(bgrl)

# Brown Templated Graph Library
SUBDIRS(bgrl2)

# Brown statistics library
SUBDIRS(bsta)

# Brown dynamic programming library
SUBDIRS(bdpg)

# 3D mesh library
SUBDIRS(bmsh3d)

# In memory relational database
SUBDIRS(brdb)

# Brown Imaging Library
SUBDIRS(bil)

# Brown VRML utilities library
SUBDIRS(bvrml)

# Brown opencl library
SUBDIRS(bocl)

# Brown asychronous IO library
SUBDIRS(baio)

# Brown radiometry library
SUBDIRS(brad)

# viewsphere Library
  SUBDIRS(vsph)

# camera library
  SUBDIRS (bpgl)

IF( BUILD_GEL )
  # spatial object algorithms
  SUBDIRS(bsol)

  # topology algorithms
  SUBDIRS(btol)

  # digital geometry algorithms
  SUBDIRS(bdgl)

  # basic vgui classes
  SUBDIRS(bgui)

  # coin3D incorporated vgui classes
  SUBDIRS(bgui3d)

  # staged vgl development area
  SUBDIRS(bvgl)

  # 3D modeling library
  SUBDIRS(bwm)

  # Indexed Mesh Library
  SUBDIRS(imesh)

ENDIF( BUILD_GEL )

# Brown hadoop library
SUBDIRS(bhdfs)

# Brown subjective logic library
SUBDIRS(bsl)

# Brown elastic curve matching library
SUBDIRS(bcvr)

# Brown KML utilities library
SUBDIRS(bkml)

# volumetric matching tools
SUBDIRS(volm)

