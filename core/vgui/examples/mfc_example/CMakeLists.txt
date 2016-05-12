# core/vgui/examples/mfc_example

set(mfc_example_sources
 example_mfc.cpp     example_mfc.h    example_mfc.rc
 example_mfcView.cpp example_mfcView.h
 example_mfcDoc.cpp  example_mfcDoc.h
 MainFrm.cpp         MainFrm.h
)

add_executable(mfc_example WIN32 ${mfc_example_sources})

target_link_libraries(mfc_example ${VXL_LIB_PREFIX}vgui ${VXL_LIB_PREFIX}vcl)
