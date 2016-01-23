# b3p/CMakeLists.txt
# Brown 3rd Party Libraries.

####################################################################
#                                                                  #
# This is a place to store third party libraries that are required #
# by other code in this repository.                                #
#                                                                  #
####################################################################

#XML Parser


option( VXL_FORCE_B3P_EXPAT "Use B3P instead of any native EXPAT library?" NO)
mark_as_advanced( VXL_FORCE_B3P_EXPAT )
add_subdirectory(expat)
add_subdirectory(expatpp)

option( VXL_FORCE_B3P_SHAPELIB "Use B3P instead of any native SHAPELIB library?" YES)
mark_as_advanced( VXL_FORCE_B3P_SHAPELIB )
add_subdirectory(shapelib)

add_subdirectory(minizip)


