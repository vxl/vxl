# brl/bbas/CMakeLists.txt
# main package for basic operations common to all libraries
# All libraries are mutually independent, except for bxml <-- bdgl

# xml<->vxl conversion
add_subdirectory(bxml)

# Uncertainty geometry library
add_subdirectory(bugl)

# Additions to vnl
add_subdirectory(bnl)

# Brown Graph Library
add_subdirectory(bgrl)

# Brown Templated Graph Library
add_subdirectory(bgrl2)

# Brown statistics library
add_subdirectory(bsta)

# Brown dynamic programming library
add_subdirectory(bdpg)

# 3D mesh library
add_subdirectory(bmsh3d)

# In memory relational database
add_subdirectory(brdb)

# Brown Imaging Library
add_subdirectory(bil)

# Brown VRML utilities library
add_subdirectory(bvrml)

# Brown opencl library
add_subdirectory(bocl)

# Brown asychronous IO library
add_subdirectory(baio)

# Brown radiometry library
add_subdirectory(brad)

# viewsphere Library
  add_subdirectory(vsph)

# camera library
  add_subdirectory (bpgl)

if( VXL_BUILD_GEL )
  # spatial object algorithms
  add_subdirectory(bsol)

  # topology algorithms
  add_subdirectory(btol)

  # digital geometry algorithms
  add_subdirectory(bdgl)

  # basic vgui classes
  add_subdirectory(bgui)

  # coin3D incorporated vgui classes
  add_subdirectory(bgui3d)

  # staged vgl development area
  add_subdirectory(bvgl)

  # 3D modeling library
  add_subdirectory(bwm)

  # Indexed Mesh Library
  add_subdirectory(imesh)

endif()

# Brown hadoop library
add_subdirectory(bhdfs)

# Brown subjective logic library
add_subdirectory(bsl)

# Brown elastic curve matching library
add_subdirectory(bcvr)

# Brown KML utilities library
add_subdirectory(bkml)

# volumetric matching tools
add_subdirectory(volm)

# kd nearest neighbor
add_subdirectory(bnabo)

# parse json files
add_subdirectory(bjson)
