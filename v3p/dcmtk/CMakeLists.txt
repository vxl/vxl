PROJECT( dcmtk )

# Since this a relatively big library, and because not many people will need
# to read DICOM files, don't build it by default except for the dashboards

IF( DART_ROOT )
  OPTION( VXL_BUILD_DCMTK "Build the V3P DCMTK libraries for reading DICOM files?" YES )
ELSE( DART_ROOT )
  OPTION( VXL_BUILD_DCMTK "Build the V3P DCMTK libraries for reading DICOM files?" NO )
ENDIF( DART_ROOT )


IF( VXL_BUILD_DCMTK )

# ---------------------------------------------------------------------------
# Configuration

# When adding a new check whose result will be cached, it is important
# to add that variable to the "clear list" below, so that it can be
# forcibly recomputed when necessary.

# Updating this serial number will cause the configuration variables
# to be recomputed. This is useful if you make a change to a test, and
# you want to ensure that the builds don't use old, invalid cached
# values of the results. The number should be incremented to indicate
# change. The format follows a DNS-style numbering: the current date
# followed by a modification time within the day.
#
SET( DCMTK_CONFIG_SERIAL_CURRENT "2004-01-19-000" )

IF( ${DCMTK_CONFIG_SERIAL_CURRENT} MATCHES "^${DCMTK_CONFIG_SERIAL_LAST}$" )
  # The configuration is current
ELSE( ${DCMTK_CONFIG_SERIAL_CURRENT} MATCHES "^${DCMTK_CONFIG_SERIAL_LAST}$" )
  # The configuration has changed, so reset the stored variables to
  # force them to be recomputed
  #
  FOREACH( VAR
        DCMTK_DCM_DICT_DEFAULT_PATH
        DCMTK_ENVIRONMENT_PATH_SEPARATOR
        DCMTK_HAVE_ACCESS
        DCMTK_HAVE_ARPA_INET_H
        DCMTK_HAVE_ASSERT_H
        DCMTK_HAVE_BCMP
        DCMTK_HAVE_BCOPY
        DCMTK_HAVE_BZERO
        DCMTK_HAVE_CASSERT
        DCMTK_HAVE_CTYPE_H
        DCMTK_HAVE_CUSERID
        DCMTK_HAVE_DIRENT_H
        DCMTK_HAVE_ERRNO_H
        DCMTK_HAVE_FLOAT_H
        DCMTK_HAVE_GETENV
        DCMTK_HAVE_GETHOSTBYNAME
        DCMTK_HAVE_GETHOSTID
        DCMTK_HAVE_GETHOSTNAME
        DCMTK_HAVE_GETLOGIN
        DCMTK_HAVE_GETPID
        DCMTK_HAVE_INDEX
        DCMTK_HAVE_IO_H
        DCMTK_HAVE_ISO646_H
        DCMTK_HAVE_ITOA
        DCMTK_HAVE_LIBC_H
        DCMTK_HAVE_LIMITS_H
        DCMTK_HAVE_LOCALE_H
        DCMTK_HAVE_MATH_H
        DCMTK_HAVE_MEMCMP
        DCMTK_HAVE_MEMCPY
        DCMTK_HAVE_MEMMOVE
        DCMTK_HAVE_MEMSET
        DCMTK_HAVE_MKTEMP
        DCMTK_HAVE_NDIR_H
        DCMTK_HAVE_NETDB_H
        DCMTK_HAVE_NETINET_IN_H
        DCMTK_HAVE_PROTOTYPE_BZERO
        DCMTK_HAVE_PROTOTYPE_GETHOSTID
        DCMTK_HAVE_PROTOTYPE_GETTIMEOFDAY
        DCMTK_HAVE_PROTOTYPE_MKTEMP
        DCMTK_HAVE_PTHREAD_H
        DCMTK_HAVE_SEMAPHORE_H
        DCMTK_HAVE_SETJMP_H
        DCMTK_HAVE_SIGNAL_H
        DCMTK_HAVE_SSTREAM
        DCMTK_HAVE_SSTREAM_H
        DCMTK_HAVE_STAT
        DCMTK_HAVE_STDARG_H
        DCMTK_HAVE_STDDEF_H
        DCMTK_HAVE_STDIO_H
        DCMTK_HAVE_STDLIB_H
        DCMTK_HAVE_STD_NAMESPACE
        DCMTK_HAVE_STRCHR
        DCMTK_HAVE_STRINGS_H
        DCMTK_HAVE_STRING_H
        DCMTK_HAVE_STRLCAT
        DCMTK_HAVE_STRLCPY
        DCMTK_HAVE_STRSTR
        DCMTK_HAVE_STRSTREAM
        DCMTK_HAVE_STRSTREAM_H
        DCMTK_HAVE_STRSTREA_H
        DCMTK_HAVE_STRTOUL
        DCMTK_HAVE_SYNCH_H
        DCMTK_HAVE_SYSINFO
        DCMTK_HAVE_SYS_DIR_H
        DCMTK_HAVE_SYS_NDIR_H
        DCMTK_HAVE_SYS_SOCKET_H
        DCMTK_HAVE_SYS_STAT_H
        DCMTK_HAVE_SYS_TIME_H
        DCMTK_HAVE_SYS_TYPES_H
        DCMTK_HAVE_SYS_UTSNAME_H
        DCMTK_HAVE_THREAD_H
        DCMTK_HAVE_TIME_H
        DCMTK_HAVE_UNAME
        DCMTK_HAVE_UNISTD_H
        DCMTK_HAVE_UNIX_H
        DCMTK_HAVE_WCTYPE_H
        DCMTK_HAVE_ZLIB
        DCMTK_INCLUDE_MATH_H_AS_CXX
        DCMTK_PATH_SEPARATOR
        DCMTK_SIZEOF_LONG
        HAVE_DCMTK_SIZEOF_LONG )  # Created indirectly by CHECK_TYPE_SIZE
    SET( ${VAR} ${VAR} )
  ENDFOREACH( VAR )

  # Record that we've done the new config.
  SET( DCMTK_CONFIG_SERIAL_LAST ${DCMTK_CONFIG_SERIAL_CURRENT} CACHE INTERNAL "Serial number of last configuration" )
  
ENDIF( ${DCMTK_CONFIG_SERIAL_CURRENT} MATCHES "^${DCMTK_CONFIG_SERIAL_LAST}$" )




INCLUDE( ${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )
INCLUDE( ${CMAKE_ROOT}/Modules/CheckTypeSize.cmake )

# This checks if a prototype for FUNC (with C linkage) has been
# declared in any one of the header files listed in INCLUDE. It uses
# the C++ compiler.
#
# (The check is actually whether declaring a prototype will cause a
# conflict and thus an error. The results may differ depending on the
# compiler. For example, gcc under Cygwin will issue a warning but g++
# will issue an error. In the DCMTK, the prototypes are used in a C++
# context, so we use the C++ compiler to check.
#
MACRO(CHECK_PROTOTYPE_EXISTS FUNC INCLUDE VARIABLE)
  IF("${VARIABLE}" MATCHES "^${VARIABLE}$")
    SET( CHECK_PROTOTYPE_EXISTS_FILE "${CMAKE_BINARY_DIR}/CheckPrototypeExists.cxx" )
    SET( CHECK_PROTOTYPE_EXISTS_EXTERNC_BEGIN "extern \"C\" {\n" )
    SET( CHECK_PROTOTYPE_EXISTS_EXTERNC_END "}\n" )

    SET(MACRO_CHECK_PROTOTYPE_EXISTS_FLAGS ${CMAKE_REQUIRED_FLAGS})
    MESSAGE(STATUS "Looking for prototype for ${FUNC} in ${INCLUDE}")

    SET( ${VARIABLE} 0 )
    FOREACH(FILE ${INCLUDE})

      # First check if the header exists. Cache the result in a variable named after
      # the header, so that we don't re-do the effort
      STRING( REGEX REPLACE "\\.|/" "_" CLEAN_FILE ${FILE} )
      SET( CHECK_PROTOTYPE_EXISTS_INCLUDE "CHECK_PROTOTYPE_EXISTS_INCLUDE_${CLEAN_FILE}" )
      CHECK_INCLUDE_FILE( ${FILE} ${CHECK_PROTOTYPE_EXISTS_INCLUDE} )
      IF( ${CHECK_PROTOTYPE_EXISTS_INCLUDE} )

        FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log "Trying struct with ${FILE}\n" )
        FILE(WRITE ${CHECK_PROTOTYPE_EXISTS_FILE}
          "#include <${FILE}>\n"
          ${CHECK_PROTOTYPE_EXISTS_EXTERNC_BEGIN}
          "typedef union { int member; } dummyStruct;\n"
          "dummyStruct ${FUNC}( dummyStruct );\n"
          ${CHECK_PROTOTYPE_EXISTS_EXTERNC_END}
          "int main(){return 0;}\n" )

        TRY_COMPILE( CHECK_PROTOTYPE_EXISTS_RESULT
          ${CMAKE_BINARY_DIR}
          ${CHECK_PROTOTYPE_EXISTS_FILE}
          CMAKE_FLAGS 
          -DCOMPILE_DEFINITIONS:STRING=${MACRO_CHECK_PROTOTYPE_EXISTS_FLAGS}
          OUTPUT_VARIABLE OUTPUT)
        IF( CHECK_PROTOTYPE_EXISTS_RESULT ) 
          FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log 
            "Determining if prototype ${FUNC} exists in ${FILE} "
            "failed with the following output:\n"
            "${OUTPUT}\n\n")
        ELSE( CHECK_PROTOTYPE_EXISTS_RESULT ) 
          FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log 
            "Determining if prototype ${FUNC} exists in ${FILE} "
            "passed with the following output:\n"
            "${OUTPUT}\n\n")
          MESSAGE(STATUS "    Found in ${FILE}")
          SET( ${VARIABLE} 1 )
        ENDIF( CHECK_PROTOTYPE_EXISTS_RESULT )

      ENDIF( ${CHECK_PROTOTYPE_EXISTS_INCLUDE} )
    ENDFOREACH(FILE)

    IF( ${VARIABLE} )
      MESSAGE(STATUS "Looking for prototype of ${FUNC} - found")
      SET(${VARIABLE} 1 CACHE INTERNAL "Have prototype ${VARIABLE}")
    ELSE(${VARIABLE})
      MESSAGE(STATUS "Looking for prototype of ${FUNC} - not found")
      SET(${VARIABLE} "" CACHE INTERNAL "Have prototype ${VARIABLE}")
    ENDIF(${VARIABLE})
  ENDIF("${VARIABLE}" MATCHES "^${VARIABLE}$")
ENDMACRO(CHECK_PROTOTYPE_EXISTS)


# The DCMTK HAVE_STD_NAMESPACE has a looser definition than the vcl
# ALLOWS_STD_NAMESPACE
IF( DCMTK_HAVE_STD_NAMESPACE MATCHES "DCMTK_HAVE_STD_NAMESPACE" )
  MESSAGE(STATUS "Checking if the compiler has namespace std")
  FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/test_have_namespace.cxx
    "#include <string>\n"
    "using namespace std;\n"
    "int main() { string a="a"; return 0; }\n" )
  TRY_COMPILE( DCMTK_HAVE_STD_NAMESPACE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/test_have_namespace.cxx
    CMAKE_FLAGS -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS}
    OUTPUT_VARIABLE OUTPUT )
  IF( DCMTK_HAVE_STD_NAMESPACE )
    FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
      "Checking for namespace std passed with the following output:\n"
      "${OUTPUT}\n\n" )
  ELSE( DCMTK_HAVE_STD_NAMESPACE )
    FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
      "Checking for namespace std failed with the following output:\n"
      "${OUTPUT}\n\n" )
  ENDIF( DCMTK_HAVE_STD_NAMESPACE )
ENDIF( DCMTK_HAVE_STD_NAMESPACE MATCHES "DCMTK_HAVE_STD_NAMESPACE" )


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

CHECK_PROTOTYPE_EXISTS(
  bzero
  "string.h;strings.h;libc.h;unistd.h;stdlib.h"
  DCMTK_HAVE_PROTOTYPE_BZERO )
CHECK_PROTOTYPE_EXISTS(
  gethostid 
  "libc.h;unistd.h;stdlib.h;netdb.h"
  DCMTK_HAVE_PROTOTYPE_GETHOSTID )
CHECK_PROTOTYPE_EXISTS(
  gettimeofday
  "sys/time.h;unistd.h"
  DCMTK_HAVE_PROTOTYPE_GETTIMEOFDAY )
CHECK_PROTOTYPE_EXISTS(
  mktemp
  "libc.h;unistd.h;stdlib.h"
  DCMTK_HAVE_PROTOTYPE_MKTEMP )

# Some platforms (e.g. MipsPRO with IRIX 6.5) don't have the standard
# include files (cassert, iostream, etc), but instead have the older
# iostream.h, etc.
IF( DCMTK_HAVE_CASSERT )
  SET( DCMTK_HAVE_STD_CXX_INCLUDES 1 )
ELSE( DCMTK_HAVE_CASSERT )
  SET( DCMTK_HAVE_STD_CXX_INCLUDES )
ENDIF( DCMTK_HAVE_CASSERT )

INCLUDE( ${MODULE_PATH}/FindZLIB.cmake )
IF( ZLIB_FOUND )
  INCLUDE_DIRECTORIES( ${ZLIB_INCLUDE_DIR} )
  SET( DCMTK_HAVE_ZLIB 1 )
ELSE( ZLIB_FOUND )
  SET( DCMTK_HAVE_ZLIB )
ENDIF( ZLIB_FOUND )

SET( DCMTK_DCM_DICT_DEFAULT_PATH "${PROJECT_SOURCE_DIR}/dcmdata/libsrc/dicom.dic" )
IF( WIN32 )
  SET( DCMTK_PATH_SEPARATOR "\\\\" )
  SET( DCMTK_ENVIRONMENT_PATH_SEPARATOR ";" )
ELSE( WIN32 )
  SET( DCMTK_PATH_SEPARATOR "/" )
  SET( DCMTK_ENVIRONMENT_PATH_SEPARATOR ":" )
ENDIF( WIN32 )

IF( "DCMTK_INCLUDE_MATH_H_AS_CXX" MATCHES "^${DCMTK_INCLUDE_MATH_H_AS_CXX}$" )
  FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeTmp/CheckIncludeMath.cxx
    "extern \"C\" { #include <math.h> }" )
  TRY_COMPILE( DCMTK_INCLUDE_MATH_H_AS_CXX
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/CMakeTmp/CheckIncludeMath.cxx
    CMAKE_FLAGS 
    -DCOMPILE_DEFINITIONS:STRING=${CMAKE_REQUIRED_FLAGS}
    OUTPUT_VARIABLE OUTPUT)
  IF( DCMTK_INCLUDE_MATH_H_AS_CXX ) 
    FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log 
      "Determining math.h as cxx"
      "passed with the following output:\n"
      "${OUTPUT}\n\n")
    SET( DCMTK_INCLUDE_MATH_H_AS_CXX 1 CACHE INTERNAL "Can include math.h with extern C")
  ELSE( DCMTK_INCLUDE_MATH_H_AS_CXX ) 
    FILE(APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log 
      "Determining math.h as cxx"
      "failed with the following output:\n"
      "${OUTPUT}\n\n")
    SET( DCMTK_INCLUDE_MATH_H_AS_CXX "" CACHE INTERNAL "Can include math.h with extern C")
  ENDIF( DCMTK_INCLUDE_MATH_H_AS_CXX ) 
ENDIF( "DCMTK_INCLUDE_MATH_H_AS_CXX" MATCHES "^${DCMTK_INCLUDE_MATH_H_AS_CXX}$" )

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/config/include/osconfig_cmake.h.in
  ${PROJECT_BINARY_DIR}/osconfig.h
)


# ---------------------------------------------------------------------------
# Source files

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/ofstd/include )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/dcmdata/include )
INCLUDE_DIRECTORIES( ${PROJECT_SOURCE_DIR}/dcmimgle/include )


SET( dcmtk_sources
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

ADD_LIBRARY( dcmtk ${dcmtk_sources} )
IF(WIN32)
  TARGET_LINK_LIBRARIES( dcmtk Netapi32 )
ENDIF(WIN32)
IF( DCMTK_HAVE_ZLIB )
  TARGET_LINK_LIBRARIES( dcmtk ${ZLIB_LIBRARIES} )
ENDIF( DCMTK_HAVE_ZLIB )

ENDIF( VXL_BUILD_DCMTK )
