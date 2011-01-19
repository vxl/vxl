# Find the ECW library - Enhanced Compression Wavelets for JPEG2000.
#
# Sets
#   ECW_FOUND.  If false, don't try to use ecw
#   ECW_INCLUDE_DIR
#   ECW_LIBRARIES

# The original sponsorring website of this library appears to have vanished,
# but there are still traces at http://www.gdal.org/frmt_ecw.html and a
# distribution at https://svn.zib.de/lenne3d/lib/libecw/current - IMS  7-Dec-2009.
IF( VXL_FORCE_V3P_J2K )
ELSE( VXL_FORCE_V3P_J2K )
SET( ECW_FOUND "NO" )

FIND_PATH( ECW_INCLUDE_DIR NCSEcw.h
  /usr/include
  /usr/local/include
)

IF( ECW_INCLUDE_DIR )

  FIND_LIBRARY( ECW_ncsutil_LIBRARY NCSUtild
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )

  FIND_LIBRARY( ECW_ncsecw_LIBRARY NCSEcwd
    /usr/lib
    /usr/local/lib
    /usr/lib64
    /usr/local/lib64
  )
  
  IF( ECW_ncsutil_LIBRARY )
  IF( ECW_ncsecw_LIBRARY )

    SET( ECW_FOUND "YES" )
	SET( ECW_LIBRARIES ${ECW_ncsutil_LIBRARY} ${ECW_ncsecw_LIBRARY} )

  ENDIF( ECW_ncsecw_LIBRARY )
  ENDIF( ECW_ncsutil_LIBRARY )


ENDIF( ECW_INCLUDE_DIR )
ENDIF( VXL_FORCE_V3P_J2K )

IF (ECW_FOUND)
	SET(VXL_USING_NATIVE_J2K "YES")
ELSE (ECW_FOUND)
 IF((EXISTS ${vxl_SOURCE_DIR}/v3p/j2k/Source/include/NCSEcw.h))

    SET( ECW_FOUND "YES" )
    SET( ECW_INCLUDE_DIR ${vxl_SOURCE_DIR}/v3p/j2k/Source/include)  
    SET( ECW_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_DIR}/include/vxl/v3p/j2k)
    SET( ECW_LIBRARIES NCSEcw NCSUtil )
  
  ENDIF((EXISTS ${vxl_SOURCE_DIR}/v3p/j2k/Source/include/NCSEcw.h))
 ENDIF(ECW_FOUND)