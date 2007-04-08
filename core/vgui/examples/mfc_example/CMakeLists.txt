# oxl/vgui/examples/mfc_example

SET(mfc_example_sources
 example_mfc.cpp     example_mfc.h    example_mfc.rc
 example_mfcView.cpp example_mfcView.h
 example_mfcDoc.cpp  example_mfcDoc.h
 StdAfx.cpp          StdAfx.h
 MainFrm.cpp         MainFrm.h
)

ADD_EXECUTABLE(mfc_example WIN32 ${mfc_example_sources})

TARGET_LINK_LIBRARIES(mfc_example vgui vcl)
