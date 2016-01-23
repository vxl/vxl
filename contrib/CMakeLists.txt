# This is contrib/CMakeLists.txt
# Dependencies: mul --> (rpl,tbl);  oxl --> gel --> brl;  (gel,oxl) --> conversions

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_CONVERSIONS "Build the Conversions Examples?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/gel/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_GEL "Build the GE library package?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/gel/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_BRL "Build the Brown University library package?" ON "BUILD_CONTRIB" OFF)
  # Optionally Build this library only if VGUI is available
  CMAKE_DEPENDENT_OPTION(BUILD_BGUI3D "Build VGUI wrapper for Coin3d" OFF "BUILD_CONTRIB;BUILD_BRL;BUILD_GEL;HAS_VGUI" OFF)
  CMAKE_DEPENDENT_OPTION(BOXM2_USE_VOLM "Inlcude Volm in boxm2_batch" ON "BUILD_CONTRIB;BUILD_BRL;BUILD_GEL" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/brl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/mul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_MUL "Build the Manchester University library package?" ON "BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(BUILD_MUL_TOOLS "Build the MUL tools?" ON "BUILD_CONTRIB;BUILD_MUL" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/mul/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/oul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_OUL "Build the Otago University library package?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/oul/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/prip/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_PRIP "Build the T.U.Wien library package?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/prip/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_OXL "Build the Oxford University library package?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_RPL "Build the RPI library package?" ON "BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(BUILD_RPL_RGTL "Build Rensselaer Generic Tree Library (rgtl)" OFF "BUILD_CONTRIB;BUILD_RPL" OFF)
  MARK_AS_ADVANCED(BUILD_RPL_RTVL)
  CMAKE_DEPENDENT_OPTION(BUILD_RPL_RTVL "Build Rensselaer Tensor Voting Library (rtvl)" OFF "BUILD_CONTRIB;BUILD_RPL_RGTL" OFF)
  MARK_AS_ADVANCED(BUILD_RPL_RGTL)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_TBL "Build the TB library package?" ON "BUILD_CONTRIB" OFF)
ENDIF( EXISTS ${vxl_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )

IF( EXISTS ${vxl_SOURCE_DIR}/contrib/cul/CMakeLists.txt )
    CMAKE_DEPENDENT_OPTION(BUILD_CUL OFF "Build the Cornell University library package?" "BUILD_CONTRIB" OFF)
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
