LINK_LIBRARIES( vgui osl )

INCLUDE( ${CMAKE_ROOT}/Modules/FindGLUT.cmake )

ADD_EXECUTABLE( satellite satellite.cxx )

IF (GLUT_FOUND)
 ADD_EXECUTABLE( raw-glut1 raw-glut1.cxx )
 TARGET_LINK_LIBRARIES( raw-glut1 ${GLUT_LIBRARIES} )

 ADD_EXECUTABLE( raw-glut2 raw-glut2.cxx )
 TARGET_LINK_LIBRARIES( raw-glut2 ${GLUT_LIBRARIES} )

 ADD_EXECUTABLE( slab-glut slab-glut.cxx )
 TARGET_LINK_LIBRARIES( slab-glut ${GLUT_LIBRARIES} )
ENDIF (GLUT_FOUND)

ADD_EXECUTABLE( display-points display-points.cxx )
ADD_EXECUTABLE( display-canny display-canny.cxx )
ADD_EXECUTABLE( display-pyramid display-pyramid.cxx )
ADD_EXECUTABLE( display-images display-images.cxx )
ADD_EXECUTABLE( harris-moments harris-moments.cxx )

ADD_EXECUTABLE( polygon-scan polygon-scan.cxx )
