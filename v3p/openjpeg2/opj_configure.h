/*
 * here is where system comupted values get stored these values should only
 * change when the target compile platform changes
 */

/* what byte order */
#ifndef __OPJ_CONFIGURE_H
#define __OPJ_CONFIGURE_H
/* #undef CMAKE_WORDS_BIGENDIAN */
#ifdef CMAKE_WORDS_BIGENDIAN
        #define OPJ_BIG_ENDIAN
#else
        #define OPJ_LITTLE_ENDIAN
#endif

#endif /* __OPJ_CONFIGURE_H */
