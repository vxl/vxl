# This is brcv/shp/bmsh3d/pro/CMakeLists.txt
# Brown Mesh Processing library
INCLUDE_DIRECTORIES( ${BRL_INCLUDE_DIR} )

SET ( bmsh3d_pro_sources
    bmsh3d_cmdpara.cxx         bmsh3d_cmdpara.h
)


ADD_LIBRARY ( bmsh3d_pro ${bmsh3d_pro_sources} )

TARGET_LINK_LIBRARIES ( bmsh3d_pro bmsh3d bmsh3d_algo)

