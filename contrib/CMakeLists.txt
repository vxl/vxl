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


IF(BUILD_MUL)
  SUBDIRS(mul)
ENDIF(BUILD_MUL)

IF(BUILD_RPL)
  SUBDIRS(rpl)
ENDIF(BUILD_RPL)

IF(BUILD_TBL)
  SUBDIRS(tbl)
ENDIF(BUILD_TBL)

IF(BUILD_OUL)
  SUBDIRS(oul)
ENDIF(BUILD_OUL)

IF(BUILD_OXL)
  SUBDIRS(oxl)
ENDIF(BUILD_OXL)

IF(BUILD_GEL)
  SUBDIRS(gel)
ENDIF(BUILD_GEL)

IF(BUILD_BRL)
  SUBDIRS(brl)
ENDIF(BUILD_BRL)

IF(BUILD_PRIP)
  SUBDIRS(prip)
ENDIF(BUILD_PRIP)

IF(BUILD_CONVERSIONS)
  SUBDIRS(conversions)
ENDIF(BUILD_CONVERSIONS)
