project( dcmtk )

# Since this a relatively big library, and because not many people will need
# to read DICOM files, don't build it by default except for the dashboards

option( VXL_BUILD_DCMTK "Build the V3P DCMTK libraries for reading DICOM files?" OFF )
mark_as_advanced(VXL_BUILD_DCMTK)
if( VXL_BUILD_DCMTK )

# ---------------------------------------------------------------------------
# Configuration

include( ${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake )
include( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )
include( ${CMAKE_ROOT}/Modules/CheckTypeSize.cmake )
include( ${VXL_CMAKE_DIR}/CheckPrototypeExistsCXX.cmake )


# The DCMTK HAVE_STD_NAMESPACE has a looser definition than the vcl
# ALLOWS_STD_NAMESPACE
if( DCMTK_HAVE_STD_NAMESPACE MATCHES "DCMTK_HAVE_STD_NAMESPACE" )
  message(STATUS "Checking if the compiler has namespace std")
  file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/test_have_namespace.cxx
       "#include <string>\n"
       "using namespace std;\n"
       "int main() { string a=\"a\"; return 0; }\n" )
  try_compile( DCMTK_HAVE_STD_NAMESPACE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/test_have_namespace.cxx
    CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS}
    OUTPUT_VARIABLE OUTPUT
  )
  if( DCMTK_HAVE_STD_NAMESPACE )
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
         "Checking for namespace std passed with the following output:\n"
         "${OUTPUT}\n\n" )
  else()
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
        "Checking for namespace std failed with the following output:\n"
        "${OUTPUT}\n\n" )
  endif()
endif()


CHECK_TYPE_SIZE( long DCMTK_SIZEOF_LONG )

CHECK_FUNCTION_EXISTS( access        DCMTK_HAVE_ACCESS )
CHECK_FUNCTION_EXISTS( bcmp          DCMTK_HAVE_BCMP )
CHECK_FUNCTION_EXISTS( bcopy         DCMTK_HAVE_BCOPY )
CHECK_FUNCTION_EXISTS( bzero         DCMTK_HAVE_BZERO )
CHECK_FUNCTION_EXISTS( cuserid       DCMTK_HAVE_CUSERID )
CHECK_FUNCTION_EXISTS( getenv        DCMTK_HAVE_GETENV )
CHECK_FUNCTION_EXISTS( gethostbyname DCMTK_HAVE_GETHOSTBYNAME )
CHECK_FUNCTION_EXISTS( gethostid     DCMTK_HAVE_GETHOSTID )
CHECK_FUNCTION_EXISTS( gethostname   DCMTK_HAVE_GETHOSTNAME )
CHECK_FUNCTION_EXISTS( getlogin      DCMTK_HAVE_GETLOGIN )
CHECK_FUNCTION_EXISTS( getpid        DCMTK_HAVE_GETPID )
CHECK_FUNCTION_EXISTS( index         DCMTK_HAVE_INDEX )
CHECK_FUNCTION_EXISTS( itoa          DCMTK_HAVE_ITOA )
CHECK_FUNCTION_EXISTS( memcmp        DCMTK_HAVE_MEMCMP )
CHECK_FUNCTION_EXISTS( memcpy        DCMTK_HAVE_MEMCPY )
CHECK_FUNCTION_EXISTS( memmove       DCMTK_HAVE_MEMMOVE )
CHECK_FUNCTION_EXISTS( memset        DCMTK_HAVE_MEMSET )
CHECK_FUNCTION_EXISTS( mktemp        DCMTK_HAVE_MKTEMP )
CHECK_FUNCTION_EXISTS( stat          DCMTK_HAVE_STAT )
CHECK_FUNCTION_EXISTS( strchr        DCMTK_HAVE_STRCHR )
CHECK_FUNCTION_EXISTS( strlcat       DCMTK_HAVE_STRLCAT )
CHECK_FUNCTION_EXISTS( strlcpy       DCMTK_HAVE_STRLCPY )
CHECK_FUNCTION_EXISTS( strstr        DCMTK_HAVE_STRSTR )
CHECK_FUNCTION_EXISTS( strtoul       DCMTK_HAVE_STRTOUL )
CHECK_FUNCTION_EXISTS( sysinfo       DCMTK_HAVE_SYSINFO )
CHECK_FUNCTION_EXISTS( uname         DCMTK_HAVE_UNAME )

CHECK_INCLUDE_FILE( arpa/inet.h   DCMTK_HAVE_ARPA_INET_H )
CHECK_INCLUDE_FILE( assert.h      DCMTK_HAVE_ASSERT_H )
CHECK_INCLUDE_FILE( ctype.h       DCMTK_HAVE_CTYPE_H )
CHECK_INCLUDE_FILE( errno.h       DCMTK_HAVE_ERRNO_H )
CHECK_INCLUDE_FILE( float.h       DCMTK_HAVE_FLOAT_H )
CHECK_INCLUDE_FILE( io.h          DCMTK_HAVE_IO_H )
CHECK_INCLUDE_FILE( iso646.h      DCMTK_HAVE_ISO646_H )
CHECK_INCLUDE_FILE( libc.h        DCMTK_HAVE_LIBC_H )
CHECK_INCLUDE_FILE( limits.h      DCMTK_HAVE_LIMITS_H )
CHECK_INCLUDE_FILE( locale.h      DCMTK_HAVE_LOCALE_H )
CHECK_INCLUDE_FILE( math.h        DCMTK_HAVE_MATH_H )
CHECK_INCLUDE_FILE( netdb.h       DCMTK_HAVE_NETDB_H )
CHECK_INCLUDE_FILE( pthread.h     DCMTK_HAVE_PTHREAD_H )
CHECK_INCLUDE_FILE( semaphore.h   DCMTK_HAVE_SEMAPHORE_H )
CHECK_INCLUDE_FILE( setjmp.h      DCMTK_HAVE_SETJMP_H )
CHECK_INCLUDE_FILE( signal.h      DCMTK_HAVE_SIGNAL_H )
CHECK_INCLUDE_FILE( stdarg.h      DCMTK_HAVE_STDARG_H )
CHECK_INCLUDE_FILE( stddef.h      DCMTK_HAVE_STDDEF_H )
CHECK_INCLUDE_FILE( stdio.h       DCMTK_HAVE_STDIO_H )
CHECK_INCLUDE_FILE( stdlib.h      DCMTK_HAVE_STDLIB_H )

CHECK_INCLUDE_FILE_CXX( cassert       DCMTK_HAVE_CASSERT )
CHECK_INCLUDE_FILE_CXX( sstream       DCMTK_HAVE_SSTREAM )
CHECK_INCLUDE_FILE_CXX( sstream.h     DCMTK_HAVE_SSTREAM_H )
CHECK_INCLUDE_FILE_CXX( string.h      DCMTK_HAVE_STRING_H )
CHECK_INCLUDE_FILE_CXX( strings.h     DCMTK_HAVE_STRINGS_H )
CHECK_INCLUDE_FILE_CXX( strstrea.h    DCMTK_HAVE_STRSTREA_H )
CHECK_INCLUDE_FILE_CXX( strstream     DCMTK_HAVE_STRSTREAM )
CHECK_INCLUDE_FILE_CXX( strstream.h   DCMTK_HAVE_STRSTREAM_H )

CHECK_INCLUDE_FILE( synch.h       DCMTK_HAVE_SYNCH_H )
CHECK_INCLUDE_FILE( sys/socket.h  DCMTK_HAVE_SYS_SOCKET_H )
CHECK_INCLUDE_FILE( sys/stat.h    DCMTK_HAVE_SYS_STAT_H )
CHECK_INCLUDE_FILE( sys/time.h    DCMTK_HAVE_SYS_TIME_H )
CHECK_INCLUDE_FILE( sys/types.h   DCMTK_HAVE_SYS_TYPES_H )
CHECK_INCLUDE_FILE( sys/utsname.h DCMTK_HAVE_SYS_UTSNAME_H )
CHECK_INCLUDE_FILE( thread.h      DCMTK_HAVE_THREAD_H )
CHECK_INCLUDE_FILE( time.h        DCMTK_HAVE_TIME_H )
CHECK_INCLUDE_FILE( unistd.h      DCMTK_HAVE_UNISTD_H )
CHECK_INCLUDE_FILE( unix.h        DCMTK_HAVE_UNIX_H )
CHECK_INCLUDE_FILE( wctype.h      DCMTK_HAVE_WCTYPE_H )
CHECK_INCLUDE_FILE( dirent.h      DCMTK_HAVE_DIRENT_H )
CHECK_INCLUDE_FILE( ndir.h        DCMTK_HAVE_NDIR_H )
CHECK_INCLUDE_FILE( sys/dir.h     DCMTK_HAVE_SYS_DIR_H )
CHECK_INCLUDE_FILE( sys/ndir.h    DCMTK_HAVE_SYS_NDIR_H )
CHECK_INCLUDE_FILE( netinet/in.h  DCMTK_HAVE_NETINET_IN_H )

CHECK_PROTOTYPE_EXISTS_CXX(
  bzero
  "string.h;strings.h;libc.h;unistd.h;stdlib.h"
  DCMTK_HAVE_PROTOTYPE_BZERO )
CHECK_PROTOTYPE_EXISTS_CXX(
  gethostid
  "libc.h;unistd.h;stdlib.h;netdb.h"
  DCMTK_HAVE_PROTOTYPE_GETHOSTID )
CHECK_PROTOTYPE_EXISTS_CXX(
  gettimeofday
  "sys/time.h;unistd.h"
  DCMTK_HAVE_PROTOTYPE_GETTIMEOFDAY )
CHECK_PROTOTYPE_EXISTS_CXX(
  mktemp
  "libc.h;unistd.h;stdlib.h"
  DCMTK_HAVE_PROTOTYPE_MKTEMP )

# Some platforms (e.g. MipsPRO with IRIX 6.5) don't have the standard
# include files (cassert, iostream, etc), but instead have the older
# iostream.h, etc.
if( DCMTK_HAVE_CASSERT )
  set( DCMTK_HAVE_STD_CXX_INCLUDES 1 )
else()
  set( DCMTK_HAVE_STD_CXX_INCLUDES )
endif()

include( ${VXL_CMAKE_DIR}/FindZLIB.cmake )
if( ZLIB_FOUND )
  include_directories( ${ZLIB_INCLUDE_DIR} )
  set( DCMTK_HAVE_ZLIB 1 )
else()
  set( DCMTK_HAVE_ZLIB )
endif()

set( DCMTK_DCM_DICT_DEFAULT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dcmdata/libsrc/dicom.dic" )
if( WIN32 )
  set( DCMTK_PATH_SEPARATOR "\\\\" )
  set( DCMTK_ENVIRONMENT_PATH_SEPARATOR ";" )
else()
  set( DCMTK_PATH_SEPARATOR "/" )
  set( DCMTK_ENVIRONMENT_PATH_SEPARATOR ":" )
endif()

if( "DCMTK_INCLUDE_MATH_H_AS_CXX" MATCHES "^${DCMTK_INCLUDE_MATH_H_AS_CXX}$" )
  file(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckIncludeMath.cxx
       "extern \"C\" { #include <math.h> }" )
  try_compile( DCMTK_INCLUDE_MATH_H_AS_CXX
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/CheckIncludeMath.cxx
    CMAKE_FLAGS
    -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS}
    OUTPUT_VARIABLE OUTPUT)
  if( DCMTK_INCLUDE_MATH_H_AS_CXX )
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
         "Determining math.h as cxx"
         "passed with the following output:\n"
         "${OUTPUT}\n\n" )
    set( DCMTK_INCLUDE_MATH_H_AS_CXX 1 CACHE INTERNAL "Can include math.h with extern C")
  else()
    file(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
         "Determining math.h as cxx"
         "failed with the following output:\n"
         "${OUTPUT}\n\n" )
    set( DCMTK_INCLUDE_MATH_H_AS_CXX "" CACHE INTERNAL "Can include math.h with extern C")
  endif()
endif()

vxl_configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config/include/osconfig_cmake.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/osconfig.h v3p/dcmtk
)


# The gcc 3.3 compiler produces lots of warnings like
#   ./v3p/dcmtk/ofstd/include/ofcond.h:350: warning: cast
#     from `const OFConditionBase*' to `OFConditionBase*' discards qualifiers from
#     pointer target type
# and
#   ./v3p/dcmtk/dcmdata/libsrc/dcuid.cxx:773: warning: unused
#     variable `bool found'
# There is no point in us fixing them locally. IMS
#
# Unfortunately, -Wno-unused-variable only applies to gcc 3.3; it is
# not a valid gcc 2.94.3 option. AP

if( DCMTK_CMAKE_COMPILER_IS_GNUCXX_2XX MATCHES "DCMTK_CMAKE_COMPILER_IS_GNUCXX_2XX")
  if( CMAKE_COMPILER_IS_GNUCXX )
    exec_program(${CMAKE_CXX_COMPILER} ARGS --version OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
    if(CMAKE_CXX_COMPILER_VERSION MATCHES ".*2\\.9[0-9]\\.[0-9].*")
      message( STATUS "gcc 2.x")
      set( DCMTK_CMAKE_COMPILER_IS_GNUCXX_2XX 1 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    else()
      message( STATUS "gcc not 2.x")
      set( DCMTK_CMAKE_COMPILER_IS_GNUCXX_2XX 0 CACHE INTERNAL "Are we using an version of gcc < 3.0")
    endif()
  endif()
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
  if( NOT DCMTK_CMAKE_COMPILER_IS_GNUCXX_2XX )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-cast-qual -Wno-unused-variable")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-cast-qual -Wno-unused-variable")
  endif()
endif()

# ---------------------------------------------------------------------------
# Source files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ofstd/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/dcmdata/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/dcmimgle/include )


set( dcmtk_sources
  ofstd/libsrc/ofcond.cxx
  ofstd/libsrc/ofconsol.cxx
  ofstd/libsrc/ofcrc32.cxx
  ofstd/libsrc/ofdate.cxx
  ofstd/libsrc/ofdatime.cxx
  ofstd/libsrc/oflist.cxx
  ofstd/libsrc/ofstd.cxx
  ofstd/libsrc/ofstring.cxx
  ofstd/libsrc/ofthread.cxx
  ofstd/libsrc/oftime.cxx
  dcmdata/libsrc/dcbytstr.cxx
  dcmdata/libsrc/dcchrstr.cxx
  dcmdata/libsrc/dccodec.cxx
  dcmdata/libsrc/dcdatset.cxx
  dcmdata/libsrc/dcdebug.cxx
  dcmdata/libsrc/dcdicent.cxx
  dcmdata/libsrc/dcdict.cxx
  dcmdata/libsrc/dcdictzz.cxx
  dcmdata/libsrc/dcdirrec.cxx
  dcmdata/libsrc/dcelem.cxx
  dcmdata/libsrc/dcerror.cxx
  dcmdata/libsrc/dcfilefo.cxx
  dcmdata/libsrc/dchashdi.cxx
  dcmdata/libsrc/dcistrma.cxx
  dcmdata/libsrc/dcistrmf.cxx
  dcmdata/libsrc/dcistrmz.cxx
  dcmdata/libsrc/dcitem.cxx
  dcmdata/libsrc/dclist.cxx
  dcmdata/libsrc/dcmetinf.cxx
  dcmdata/libsrc/dcobject.cxx
  dcmdata/libsrc/dcostrma.cxx
  dcmdata/libsrc/dcostrmf.cxx
  dcmdata/libsrc/dcostrmz.cxx
  dcmdata/libsrc/dcpcache.cxx
  dcmdata/libsrc/dcpixel.cxx
  dcmdata/libsrc/dcpixseq.cxx
  dcmdata/libsrc/dcpxitem.cxx
  dcmdata/libsrc/dcsequen.cxx
  dcmdata/libsrc/dcstack.cxx
  dcmdata/libsrc/dcswap.cxx
  dcmdata/libsrc/dctag.cxx
  dcmdata/libsrc/dctagkey.cxx
  dcmdata/libsrc/dctypes.cxx
  dcmdata/libsrc/dcuid.cxx
  dcmdata/libsrc/dcvm.cxx
  dcmdata/libsrc/dcvr.cxx
  dcmdata/libsrc/dcvrae.cxx
  dcmdata/libsrc/dcvras.cxx
  dcmdata/libsrc/dcvrat.cxx
  dcmdata/libsrc/dcvrcs.cxx
  dcmdata/libsrc/dcvrda.cxx
  dcmdata/libsrc/dcvrds.cxx
  dcmdata/libsrc/dcvrdt.cxx
  dcmdata/libsrc/dcvrfd.cxx
  dcmdata/libsrc/dcvrfl.cxx
  dcmdata/libsrc/dcvris.cxx
  dcmdata/libsrc/dcvrlo.cxx
  dcmdata/libsrc/dcvrlt.cxx
  dcmdata/libsrc/dcvrobow.cxx
  dcmdata/libsrc/dcvrof.cxx
  dcmdata/libsrc/dcvrpn.cxx
  dcmdata/libsrc/dcvrpobw.cxx
  dcmdata/libsrc/dcvrsh.cxx
  dcmdata/libsrc/dcvrsl.cxx
  dcmdata/libsrc/dcvrss.cxx
  dcmdata/libsrc/dcvrst.cxx
  dcmdata/libsrc/dcvrtm.cxx
  dcmdata/libsrc/dcvrui.cxx
  dcmdata/libsrc/dcvrul.cxx
  dcmdata/libsrc/dcvrulup.cxx
  dcmdata/libsrc/dcvrus.cxx
  dcmdata/libsrc/dcvrut.cxx
  dcmdata/libsrc/dcxfer.cxx
  dcmimgle/libsrc/dcmimage.cxx
  dcmimgle/libsrc/dibaslut.cxx
  dcmimgle/libsrc/didislut.cxx
  dcmimgle/libsrc/didispfn.cxx
  dcmimgle/libsrc/didocu.cxx
  dcmimgle/libsrc/digsdfn.cxx
  dcmimgle/libsrc/digsdlut.cxx
  dcmimgle/libsrc/diimage.cxx
  dcmimgle/libsrc/diinpx.cxx
  dcmimgle/libsrc/diluptab.cxx
  dcmimgle/libsrc/dimo1img.cxx
  dcmimgle/libsrc/dimo2img.cxx
  dcmimgle/libsrc/dimoimg.cxx
  dcmimgle/libsrc/dimoimg3.cxx
  dcmimgle/libsrc/dimoimg4.cxx
  dcmimgle/libsrc/dimoimg5.cxx
  dcmimgle/libsrc/dimomod.cxx
  dcmimgle/libsrc/dimoopx.cxx
  dcmimgle/libsrc/dimopx.cxx
  dcmimgle/libsrc/diovdat.cxx
  dcmimgle/libsrc/diovlay.cxx
  dcmimgle/libsrc/diovlimg.cxx
  dcmimgle/libsrc/diovpln.cxx
  dcmimgle/libsrc/diutils.cxx
)

vxl_add_library(LIBRARY_NAME dcmtk LIBRARY_SOURCES ${dcmtk_sources})
if(WIN32)
  target_link_libraries( dcmtk Netapi32 )
endif()
if( DCMTK_HAVE_ZLIB )
  target_link_libraries( dcmtk ${ZLIB_LIBRARIES} )
endif()

if( BUILD_TESTING )
  add_subdirectory(tests)
endif()

endif()
