# This is contrib/CMakeLists.txt
# Dependencies: mul --> (rpl,tbl);  oxl --> gel --> brl;  (gel,oxl) --> conversions

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_CONVERSIONS "Build the Conversions Examples?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/gel/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_GEL "Build the GE library package?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/brl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_BRL "Build the Brown University library package?" ON "BUILD_CONTRIB" OFF)
  # Optionally Build this library only if VGUI is available
  CMAKE_DEPENDENT_OPTION(BUILD_BGUI3D "Build VGUI wrapper for Coin3d" OFF "BUILD_CONTRIB;BUILD_BRL;BUILD_GEL;HAS_VGUI" OFF)
  CMAKE_DEPENDENT_OPTION(BOXM2_USE_VOLM "Inlcude Volm in boxm2_batch" ON "BUILD_CONTRIB;BUILD_BRL;BUILD_GEL" OFF)
  CMAKE_DEPENDENT_OPTION(BUILD_BRL_PYTHON "Build the python interfaces for BRL" OFF "BUILD_CONTRIB;BUILD_BRL" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/mul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_MUL "Build the Manchester University library package?" ON "BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(BUILD_MUL_TOOLS "Build the MUL tools?" ON "BUILD_CONTRIB;BUILD_MUL" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/oul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_OUL "Build the Otago University library package?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/prip/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_PRIP "Build the T.U.Wien library package?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_OXL "Build the Oxford University library package?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_RPL "Build the RPI library package?" ON "BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(BUILD_RPL_RGTL "Build Rensselaer Generic Tree Library (rgtl)" OFF "BUILD_CONTRIB;BUILD_RPL" OFF)
  mark_as_advanced(BUILD_RPL_RTVL)
  CMAKE_DEPENDENT_OPTION(BUILD_RPL_RTVL "Build Rensselaer Tensor Voting Library (rtvl)" OFF "BUILD_CONTRIB;BUILD_RPL_RGTL" OFF)
  mark_as_advanced(BUILD_RPL_RGTL)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(BUILD_TBL "Build the TB library package?" ON "BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${CMAKE_SOURCE_DIR}/contrib/cul/CMakeLists.txt )
    CMAKE_DEPENDENT_OPTION(BUILD_CUL OFF "Build the Cornell University library package?" "BUILD_CONTRIB" OFF)
endif()


if(BUILD_MUL)
  add_subdirectory(mul)
endif()

if(BUILD_RPL)
  add_subdirectory(rpl)
endif()

if(BUILD_TBL)
  add_subdirectory(tbl)
endif()

if(BUILD_OUL)
  add_subdirectory(oul)
endif()

if(BUILD_OXL)
  add_subdirectory(oxl)
endif()

if(BUILD_GEL)
  add_subdirectory(gel)
endif()

if(BUILD_BRL)
  add_subdirectory(brl)
  if(BUILD_GEL)
  if(BUILD_TESTING)
    # since gel/vifa/tests depends on brl/bseg/sdet, it must be built after brl:
    add_subdirectory(gel/vifa/tests)
  endif()
  endif()
endif()

if(BUILD_PRIP)
  add_subdirectory(prip)
endif()

if(BUILD_CONVERSIONS)
  add_subdirectory(conversions)
endif()

if(BUILD_CUL)
  add_subdirectory(cul)
endif()
