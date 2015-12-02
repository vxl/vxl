# b3p/CMakeLists.txt
# Brown 3rd Party Libraries.

####################################################################
#                                                                  #
# This is a place to store third party libraries that are required #
# by other code in this repository.                                #
#                                                                  #
####################################################################

#XML Parser


OPTION( VXL_FORCE_B3P_EXPAT "Use B3P instead of any native EXPAT library?" NO)
MARK_AS_ADVANCED( VXL_FORCE_B3P_EXPAT )
add_subdirectory(expat)
add_subdirectory(expatpp)

OPTION( VXL_FORCE_B3P_SHAPELIB "Use B3P instead of any native SHAPELIB library?" YES)
MARK_AS_ADVANCED( VXL_FORCE_B3P_SHAPELIB )
add_subdirectory(shapelib)

add_subdirectory(minizip)


