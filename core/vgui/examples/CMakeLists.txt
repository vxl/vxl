# ./oxl/vgui/examples/CMakeLists.txt
#

SUBDIRS( kym fsm capes geoff) 
IF(HAS_MFC)
  SUBDIRS( vgui_example_mfc )
ENDIF(HAS_MFC)
#SUBDIRS(awf) 
