# ./oxl/vgui/examples/CMakeLists.txt
#

SUBDIRS( kym fsm capes geoff) 
IF(WIN32)
  SUBDIRS( vgui_example_mfc )
ENDIF(WIN32)
#SUBDIRS(awf) 
