# ./brl/bmvl/CMakeLists.txt
# main package for multi-view related code, .e.g. 3-d reconstruction
INCLUDE(${BRL_INCLUDE_DIR}/bmods/find_xerces.cmake)

# camera calibration
SUBDIRS(bcal)

# 3-d reconstruction 
SUBDIRS(brct)

# multi-view gui stuff , xerces is required to read stuff in from Tjr
IF(HAS_XERCES)
SUBDIRS(bmvv)
ENDIF(HAS_XERCES)




