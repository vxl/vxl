# b3p/CMakeLists.txt
# Brown 3rd Party Libraries.

####################################################################
#                                                                  #
# This is a place to store third party libraries that are required #
# by other code in this repository.                                #
#                                                                  #
####################################################################

#XML Parser
INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_expat.cmake )
IF(HAS_EXPAT)
SUBDIRS( expatpp )
ENDIF(HAS_EXPAT)