# brl/bmvl/CMakeLists.txt
# main package for multi-view related code, .e.g. 3-d reconstruction
# All libraries are mutually independent, except for bmvv <-- btom 
# Also, bcal and btom are independent of the rest of brl; brct depends on bbas.

INCLUDE( ${vxl_SOURCE_DIR}/contrib/brl/bmods/find_xerces.cmake )

# camera calibration
SUBDIRS(bcal)

# 3-d reconstruction 
SUBDIRS(brct)

# 3-d tomography
SUBDIRS(btom)

# multi-view gui stuff , xerces is required to read stuff in from Tjr
SUBDIRS(bmvv)
