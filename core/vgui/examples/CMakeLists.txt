# ./oxl/vgui/examples/CMakeLists.txt
#

SUBDIRS( kym fsm capes geoff) 
IF(MFC_FOUND)
  SUBDIRS( vgui_example_mfc )
ENDIF(MFC_FOUND)
#SUBDIRS(awf) 
