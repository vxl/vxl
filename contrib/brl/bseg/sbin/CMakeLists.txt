#executables for segmentation processing
# brl/bseg/sbin/
INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${OXL_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${GEL_INCLUDE_DIR} )
SOURCE_FILES(sbin_sources segv_menus.cxx segv_menus.h)

ADD_EXECUTABLE (seg seg.cxx sbin_sources)

TARGET_LINK_LIBRARIES(seg segv vgui)


