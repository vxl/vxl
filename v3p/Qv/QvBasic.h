#ifndef  _QV_BASIC_
#define  _QV_BASIC_

//#include <sys/types.h>
/* #include <libc.h> */
#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vcl_cstdio.h>

#ifndef FALSE
#   define FALSE	0
#   define TRUE		1
#endif

typedef int QvBool;

// This uses the preprocessor to quote a string
#if defined(WIN32) || defined(__STDC__) || defined(__ANSI_CPP__)		/* ANSI C */
#  define QV__QUOTE(str)	#str
#else							/* Non-ANSI C */
#  define QV__QUOTE(str)	"str"
#endif

// This uses the preprocessor to concatenate two strings
#if defined(__STDC__) || defined(__ANSI_CPP__)		/* ANSI C */
#   define QV__CONCAT(str1, str2)	str1##str2
#else							/* Non-ANSI C */
#   define QV__CONCAT(str1, str2)	str1/**/str2
#endif

// changed by Gerbert Orasche 260695
#ifndef u_long
#define u_long unsigned long
#endif
#ifdef __PC__
#define strdup _strdup
#endif

#include "Qv_pi.h"

#endif /* _QV_BASIC_ */
