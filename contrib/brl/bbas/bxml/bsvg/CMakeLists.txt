# contrib/brl/bbas/bxml/bsvg/CMakeLists.txt
# Brown SVG Library, create an XML document with SVG primitives and display them using any browser

INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bbas )

SET( bsvg_sources
  bsvg_document.cxx bsvg_document.h
  bsvg_element.cxx bsvg_element.h
  bsvg_plot.h bsvg_plot.cxx
)

AUX_SOURCE_DIRECTORY(Templates bsvg_sources)

ADD_LIBRARY(bsvg ${bsvg_sources})

TARGET_LINK_LIBRARIES( bsvg bxml )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )

SUBDIRS( pro )


INSTALL_TARGETS(/lib bsvg)
INSTALL_NOBASE_HEADER_FILES(/include/vxl/contrib/brl/bbas/bxml/bsvg ${bsvg_sources})
