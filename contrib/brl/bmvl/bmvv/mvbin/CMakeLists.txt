# brl/bmvv/mvbin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR}/bmvl )

SOURCE_FILES(mviewer_sources mviewer.cxx bmvv_menus.cxx bmvv_menus.h)
ADD_EXECUTABLE (mviewer mviewer_sources)
TARGET_LINK_LIBRARIES(mviewer bmvv)

