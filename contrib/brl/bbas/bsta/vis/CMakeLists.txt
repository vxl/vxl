# This is contrib/brl/bbas/bsta/vis/CMakeLists.txt
# The Brown Statistics Algorithms Library Visualization Tools

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET( bsta_vis_sources
  bsta_svg_tools.h bsta_svg_tools.cxx
)

ADD_LIBRARY( bsta_vis  ${bsta_vis_sources} )

TARGET_LINK_LIBRARIES( bsta_vis bsta bsvg vnl )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
