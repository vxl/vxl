# Locate VRML include paths and libraries

# This module defines
# LIBVRML_INCLUDE_DIR, where to find the header files etc.
# LIBVRML_LIBRARY, the libraries to link against to use the vrml classes.
# LIBVRML_FOUND, If false, do not try to use vrml.

 # try to find a path to the include directory
 # assume that vrml directory has been set
 FIND_PATH( LIBVRML_INCLUDE_DIR VrmlNode.h
   ${LIBVRML_DIR}/source ${LIBVRML_DIR}/include 
 )

 # try to find a path to the library
 # assume that vrml directory has been set
 FIND_LIBRARY( LIBVRML_LIBRARY NAMES libvrml97
   PATHS ${LIBVRML_DIR}/lib ${LIBVRML_DIR}/lib/release 
 )

 # only add the libvrml library if both the includes and the lib file are found
 IF ( LIBVRML_INCLUDE_DIR )
   IF ( LIBVRML_LIBRARY )
     SET( LIBVRML_FOUND "YES" )
     SET( LIBVRML_LIBRARIES ${LIBVRML_LIBRARY} )
   ENDIF ( LIBVRML_LIBRARY )
 ENDIF ( LIBVRML_INCLUDE_DIR )





