# ./oxl/vgui/examples/CMakeLists.txt
#

SUBDIRS( kym fsm capes ) 
IF(WIN32)
  SUBDIRS( vgui_example_mfc )
ENDIF(WIN32)
#SUBDIRS(awf) 
