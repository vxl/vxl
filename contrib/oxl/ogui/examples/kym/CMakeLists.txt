LINK_LIBRARIES( vgui vidl_vil1 vul vbl )

ADD_EXECUTABLE( display-image display-image.cxx )
ADD_EXECUTABLE( draw-line draw-line.cxx )
ADD_EXECUTABLE( 2d-example 2d-example.cxx )
ADD_EXECUTABLE( 3d-example 3d-example.cxx )
ADD_EXECUTABLE( menubar-example menubar-example.cxx )
ADD_EXECUTABLE( popup-example popup-example.cxx )
ADD_EXECUTABLE( mouse-position mouse-position.cxx )
ADD_EXECUTABLE( mouse-draw-line mouse-draw-line.cxx )
ADD_EXECUTABLE( dialog-example dialog-example.cxx )
ADD_EXECUTABLE( rubberband rubberband.cxx )
ADD_EXECUTABLE( printing_example printing_example.cxx )

INCLUDE(${MODULE_PATH}/NewCMake/FindGTK.cmake)
IF(GTK_FOUND)
 INCLUDE_DIRECTORIES( ${GTK_INCLUDE_DIR} )

 ADD_EXECUTABLE( gtk-display-image gtk-display-image.cxx )
 TARGET_LINK_LIBRARIES( gtk-display-image ${GTK_LIBRARIES} )

 ADD_EXECUTABLE( gtk-movie-player gtk-movie-player.cxx )
 TARGET_LINK_LIBRARIES( gtk-movie-player ${GTK_LIBRARIES} )

 ADD_EXECUTABLE( vidl-demo vidl-demo.cxx )
 TARGET_LINK_LIBRARIES( vidl-demo vidl_vil1 ${GTK_LIBRARIES} )
ENDIF(GTK_FOUND)
