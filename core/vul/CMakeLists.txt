# core/vul/CMakeLists.txt

SET(vul_sources
  vul_fwd.h
  vul_whereami.h

  vul_arg.h                   vul_arg.cxx
  vul_awk.h                   vul_awk.cxx
  vul_expand_path.h           vul_expand_path.cxx
  vul_file.h                  vul_file.cxx
  vul_file_iterator.h         vul_file_iterator.cxx
  vul_get_timestamp.h         vul_get_timestamp.cxx
  vul_printf.h                vul_printf.cxx
  vul_psfile.h                vul_psfile.cxx
  vul_redirector.h            vul_redirector.cxx
  vul_reg_exp.h               vul_reg_exp.cxx
  vul_sequence_filename_map.h vul_sequence_filename_map.cxx
  vul_sprintf.h               vul_sprintf.cxx
  vul_string.h                vul_string.cxx
  vul_temp_filename.h         vul_temp_filename.cxx
  vul_timer.h                 vul_timer.cxx
  vul_timestamp.h             vul_timestamp.cxx
  vul_trace.h                 vul_trace.cxx
  vul_user_info.h             vul_user_info.cxx
)

# Some versions of Solaris (at least 5.8) has a brain-dead mechanism
# for implementing DNS services, where the user of a library that uses
# gethostbyname should link to -lnsl *before* linking to the
# library. This creates a kind of "pre-dependency" that the CMake
# dependency analysis is not equipped to handle. We cope by isolating
# the offending code into its own library vul_network (under Solaris
# only, of course). We can then control the dependencies of vul to
# link nsl before this vul_network.
#
# This workaround also occurs in vil
#
SET( vul_network_sources
  vul_url.h                   vul_url.cxx
)


IF(CMAKE_COMPILER_IS_GNUCXX)
  SET_SOURCE_FILES_PROPERTIES(vul_sequence_filename_map.cxx PROPERTIES COMPILE_FLAGS -O0)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF(APPLE)
  ADD_LIBRARY(vul ${vul_sources})
ELSE(APPLE)

IF(NOT SOLARIS)
  ADD_LIBRARY(vul ${vul_sources} ${vul_network_sources})
ELSE(NOT SOLARIS)
  ADD_LIBRARY(vul ${vul_sources})
  ADD_LIBRARY(vul_network ${vul_network_sources})
ENDIF(NOT SOLARIS)

IF (NOT UNIX)
  TARGET_LINK_LIBRARIES( vul ws2_32 )
ENDIF (NOT UNIX)

IF (SOLARIS)
  TARGET_LINK_LIBRARIES( vul nsl vul_network socket )
ENDIF (SOLARIS)

ENDIF(APPLE)

TARGET_LINK_LIBRARIES( vul vcl )

IF( BUILD_EXAMPLES )
  SUBDIRS( examples )
ENDIF( BUILD_EXAMPLES )

IF( BUILD_TESTING )
  SUBDIRS(tests)
ENDIF( BUILD_TESTING )
