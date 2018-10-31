# This is contrib/CMakeLists.txt
# Dependencies: mul --> (rpl,tbl);  oxl --> gel --> brl;  (gel,oxl) --> conversions

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/conversions/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_CONVERSIONS "Build the Conversions Examples?" ON "VXL_BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/gel/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_GEL "Build the GE library package?" ON "VXL_BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/brl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_BRL "Build the Brown University library package?" ON
          "VXL_BUILD_CONTRIB;VXL_BUILD_CORE_NUMERICS;VXL_BUILD_CORE_GEOMETRY;VXL_BUILD_CORE_IMAGING;VXL_BUILD_CORE_VIDEO;" OFF)
  # Optionally Build this library only if VGUI is available
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_BGUI3D "Build VGUI wrapper for Coin3d" ON "VXL_BUILD_CONTRIB;VXL_BUILD_BRL;VXL_BUILD_GEL;VXL_BUILD_VGUI" OFF)
#  CMAKE_DEPENDENT_OPTION(VXL_BUILD_BGUI3D "Build VGUI wrapper for Coin3d" ON "VXL_BUILD_CONTRIB;VXL_BUILD_BRL;VXL_BUILD_GEL;HAS_VGUI" OFF)
  CMAKE_DEPENDENT_OPTION(VXL_BOXM2_USE_VOLM "Inlcude Volm in boxm2_batch" ON "VXL_BUILD_CONTRIB;VXL_BUILD_BRL;VXL_BUILD_GEL" OFF)
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_BRL_PYTHON "Build the python interfaces for BRL" OFF "VXL_BUILD_CONTRIB;VXL_BUILD_BRL" OFF)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/mul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_MUL "Build the Manchester University library package?" ON "VXL_BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_MUL_TOOLS "Build the MUL tools?" ON "VXL_BUILD_CONTRIB;VXL_BUILD_MUL" OFF)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/oul/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_OUL "Build the Otago University library package?" ON "VXL_BUILD_CONTRIB" OFF)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/prip/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_PRIP "Build the T.U.Wien library package?" ON "VXL_BUILD_CONTRIB" OFF)
  mark_as_advanced(VXL_BUILD_PRIP)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/oxl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_OXL "Build the Oxford University library package?" ON "VXL_BUILD_CONTRIB" OFF)
  mark_as_advanced(VXL_BUILD_PRIP)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/rpl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_RPL "Build the RPI library package?" ON "VXL_BUILD_CONTRIB" OFF)
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_RPL_RGTL "Build Rensselaer Generic Tree Library (rgtl)" OFF "VXL_BUILD_CONTRIB;VXL_BUILD_RPL" OFF)
  mark_as_advanced(VXL_BUILD_RPL_RTVL)
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_RPL_RTVL "Build Rensselaer Tensor Voting Library (rtvl)" OFF "VXL_BUILD_CONTRIB;VXL_BUILD_RPL_RGTL" OFF)
  mark_as_advanced(VXL_BUILD_RPL_RGTL)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/tbl/CMakeLists.txt )
  CMAKE_DEPENDENT_OPTION(VXL_BUILD_TBL "Build the TB library package?" ON "VXL_BUILD_CONTRIB" OFF)
  mark_as_advanced(VXL_BUILD_TBL)
endif()

if( EXISTS ${VXL_ROOT_SOURCE_DIR}/contrib/cul/CMakeLists.txt )
    CMAKE_DEPENDENT_OPTION(VXL_BUILD_CUL OFF "Build the Cornell University library package?" "VXL_BUILD_CONTRIB" OFF)
    mark_as_advanced(VXL_BUILD_CUL)
endif()


if(VXL_BUILD_MUL)
  add_subdirectory(mul)
endif()

if(VXL_BUILD_RPL)
  add_subdirectory(rpl)
endif()

if(VXL_BUILD_TBL)
  add_subdirectory(tbl)
endif()

if(VXL_BUILD_OUL)
  add_subdirectory(oul)
endif()

if(VXL_BUILD_OXL)
  add_subdirectory(oxl)
endif()

if(VXL_BUILD_GEL)
  add_subdirectory(gel)
endif()

if(VXL_BUILD_BRL)
  add_subdirectory(brl)
  if(VXL_BUILD_GEL)
  if(BUILD_TESTING)
    # since gel/vifa/tests depends on brl/bseg/sdet, it must be built after brl:
    add_subdirectory(gel/vifa/tests)
  endif()
  endif()
endif()

if(VXL_BUILD_PRIP)
  add_subdirectory(prip)
endif()

if(VXL_BUILD_CONVERSIONS)
  add_subdirectory(conversions)
endif()

if(VXL_BUILD_CUL)
  add_subdirectory(cul)
endif()
