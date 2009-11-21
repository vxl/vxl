#
# This open cl setup is based on the requirements for the AMD SDK. 
# http://developer.amd.com/gpu/ATIStreamSDKBetaProgram/Pages/default.aspx
# Undoubtedly a different setup is required for the Nvidia SDK
#
SET(OPENCL_FOUND "NO")	
IF(WIN32)
	FIND_PATH( OPENCL_INCLUDE_PATH calcl.h)

	IF(OPENCL_INCLUDE_PATH)

	 FIND_PATH( OPENCL_LIBRARY_PATH aticalcl.lib )
	
	  IF(OPENCL_LIBRARY_PATH)
		 SET(OPENCL_FOUND "YES")
		 SET( OPENCL_LIBRARIES 
			  ${OPENCL_LIBRARY_PATH}/aticalcl.lib
			  ${OPENCL_LIBRARY_PATH}/amuabi.lib
			  ${OPENCL_LIBRARY_PATH}/aticalrt.lib 
			  ${OPENCL_LIBRARY_PATH}/x86/OpenCL.lib)
	  ENDIF(OPENCL_LIBRARY_PATH)
	ENDIF(OPENCL_INCLUDE_PATH)
ENDIF (WIN32)


IF (APPLE)
  SET (OPENCL_FOUND "YES")
  SET (OPENCL_LIBRARIES "-framework OpenCL" CACHE STRING "OpenCL lib for OSX")
ENDIF (APPLE)

MESSAGE("Found Open Cl?")
MESSAGE(${OPENCL_FOUND})
