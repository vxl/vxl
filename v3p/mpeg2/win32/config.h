/* vc++/config.h - manually adapted from include/config.h.in */

/* ppc architecture */
/* #undef ARCH_PPC */

/* x86 architecture */
/* #undef ARCH_X86 */

/* maximum supported data alignment */
/* #undef ATTRIBUTE_ALIGNED_MAX */

/* Define if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define if you have the `memalign' function. */
/* #undef HAVE_MEMALIGN */

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the <stdint.h> header file. */
/* #undef HAVE_STDINT_H */

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file. */
/* #undef HAVE_SYS_TIME_H */

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* libmpeg2 mediaLib support */
/* #undef LIBMPEG2_MLIB */

/* libvo MGA support */
/* #undef LIBVO_MGA */

/* libvo mediaLib support */
/* #undef LIBVO_MLIB */

/* libvo SDL support */
/* #undef LIBVO_SDL */

/* libvo X11 support */
/* #undef LIBVO_X11 */

/* libvo Xv support */
/* #undef LIBVO_XV */

/* mpeg2dec profiling */
/* #undef MPEG2DEC_GPROF */

/* Name of package */
#define PACKAGE "mpeg2dec"

/* Define as the return type of signal handlers (`int' or `void'). */
/* #undef RETSIGTYPE */

/* The size of a `char', as computed by sizeof. */
#define SIZEOF_CHAR 1

/* The size of a `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of a `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.2.1"

/* Define if your processor stores words with the most significant byte first
   (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
#define inline __inline

/* Define as `__restrict' if that's what the C compiler calls it, or to
   nothing if it is not supported. */
#define restrict __restrict

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */
