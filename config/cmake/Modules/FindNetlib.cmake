#
# Try to find netlib
#

# AGAP: Is it even possible to have a "native" netlib? Even if not, it is
# good to have this file in place, so that all things in v3p are found
# via a module.

IF(NOT HAS_NETLIB)

  SET( HAS_NETLIB "YES" )
  SET( NETLIB_INCLUDE_PATH ${allvxl_SOURCE_DIR}/v3p/netlib )
  LINK_LIBRARIES( netlib )
  IF(UNIX)
    LINK_LIBRARIES( m )
  ENDIF(UNIX)
  # netlib needs --of course-- libm: for sqrt, exp, log, pow, sin, cos.

ENDIF(NOT HAS_NETLIB)
