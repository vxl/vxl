# Locate Maya include paths and libraries

# This module defines
# MAYA_INCLUDE_DIR, where to find the header files etc.
# MAYA_LIBRARY, the libraries to link against to use the vrml classes.
# MAYA_FOUND, If false, do not try to use vrml.

 # try to find a path to the include directory
 # assume that path MAYA_DIR to the maya directory has been set
 FIND_PATH( MAYA_INCLUDE_DIR VrmlNode.h
   ${MAYA_DIR}/source ${MAYA_DIR}/include 
 )

 # try to find a path to the library
 # assume that path MAYA_DIR to the maya directory has been set
 FIND_LIBRARY( MAYA_LIBRARY libvrml97
   ${MAYA_DIR}/lib ${MAYA_DIR}/lib/release 
 )

 # only add the libvrml library if both the includes and the lib file are found
 IF ( MAYA_INCLUDE_DIR )
   IF ( MAYA_LIBRARY )
     SET( MAYA_FOUND "YES" )
   ENDIF ( MAYA_LIBRARY )
 ENDIF ( MAYA_INCLUDE_DIR )





