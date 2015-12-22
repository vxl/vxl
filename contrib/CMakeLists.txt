# This is contrib/CMakeLists.txt
# Dependencies: mul --> (rpl,tbl);  oxl --> gel --> brl;  (gel,oxl) --> conversions

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )
  OPTION(BUILD_CONVERSIONS "Build the Conversions Examples?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/CMakeLists.txt )
  OPTION(BUILD_BRL "Build the Brown University library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/gel/CMakeLists.txt )
  OPTION(BUILD_GEL "Build the GE library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/gel/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/mul/CMakeLists.txt )
  OPTION(BUILD_MUL "Build the Manchester University library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/mul/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/oul/CMakeLists.txt )
  OPTION(BUILD_OUL "Build the Otago University library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/oul/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/prip/CMakeLists.txt )
  OPTION(BUILD_PRIP "Build the T.U.Wien library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/prip/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )
  OPTION(BUILD_OXL "Build the Oxford University library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )
  OPTION(BUILD_RPL "Build the RPI library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )
  OPTION(BUILD_TBL "Build the TB library package?" YES)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/cul/CMakeLists.txt )
    OPTION(BUILD_CUL "Build the Cornell University library package?" NO)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/cul/CMakeLists.txt )


IF(BUILD_MUL)
  add_subdirectory(mul)
ENDIF(BUILD_MUL)

IF(BUILD_RPL)
  add_subdirectory(rpl)
ENDIF(BUILD_RPL)

IF(BUILD_TBL)
  add_subdirectory(tbl)
ENDIF(BUILD_TBL)

IF(BUILD_OUL)
  add_subdirectory(oul)
ENDIF(BUILD_OUL)

IF(BUILD_OXL)
  add_subdirectory(oxl)
ENDIF(BUILD_OXL)

IF(BUILD_GEL)
  add_subdirectory(gel)
ENDIF(BUILD_GEL)

IF(BUILD_BRL)
  add_subdirectory(brl)
  IF(BUILD_GEL)
  # since gel/vsrl depends on brl/bseg/sdet, it cannot be built before brl:
  IF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
    # Dense matcher
    add_subdirectory(gel/vsrl)
  ENDIF (BUILD_UNMAINTAINED_LIBRARIES OR BUILD_FOR_VXL_DASHBOARD)
  IF (BUILD_TESTING)
    # since gel/vifa/tests depends on brl/bseg/sdet, it must be built after brl:
    add_subdirectory(gel/vifa/tests)
  ENDIF (BUILD_TESTING)
  ENDIF(BUILD_GEL)
ENDIF(BUILD_BRL)

IF(BUILD_PRIP)
  add_subdirectory(prip)
ENDIF(BUILD_PRIP)

IF(BUILD_CONVERSIONS)
  add_subdirectory(conversions)
ENDIF(BUILD_CONVERSIONS)

IF(BUILD_CUL)
  add_subdirectory(cul)
ENDIF(BUILD_CUL)
