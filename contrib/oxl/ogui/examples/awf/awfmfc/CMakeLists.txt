# oxl/vgui/examples/awf/awfmfc/CMakeLists.txt

SET(awfmfc_sources
 awfmfcView.cpp awfmfcView.h
 awfmfcDoc.cpp  awfmfcDoc.h
 awfmfc.cpp     awfmfc.h    awfmfc.rc
 StdAfx.cpp     resource.h
 MainFrm.cpp    MainFrm.h
)

ADD_EXECUTABLE(awfmfc ${awfmfc_sources})
TARGET_LINK_LIBRARIES( awfmfc vgui )
