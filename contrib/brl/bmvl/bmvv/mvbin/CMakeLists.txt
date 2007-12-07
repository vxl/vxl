# brl/bmvv/mvbin/CMakeLists.txt

INCLUDE_DIRECTORIES( ${VXLCORE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

#ADD_EXECUTABLE (modtom modtom.cxx)
#TARGET_LINK_LIBRARIES(modtom bmvv vgui)

#ADD_EXECUTABLE (mviewer mviewer.cxx)
#TARGET_LINK_LIBRARIES(mviewer bmvv vgui)

ADD_EXECUTABLE (recon recon.cxx)
TARGET_LINK_LIBRARIES(recon bmvv vgui)

ADD_EXECUTABLE (cal cal.cxx)
TARGET_LINK_LIBRARIES(cal bmvv vgui)

ADD_EXECUTABLE (ftool ftool.cxx)
TARGET_LINK_LIBRARIES(ftool bmvv vgui)
