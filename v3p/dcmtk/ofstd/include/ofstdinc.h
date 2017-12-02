/*
 *
 *  Copyright (C) 1997-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: encapsulation of old style vs. ISO C++ standard includes
 *
 */

// this file is not and should not be protected against multiple inclusion

#include <streambuf>
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwctype>
#include "osconfig.h"

#ifdef HAVE_STD_NAMESPACE
namespace std { };
using namespace std;
#endif

/* Header files as defined in ISO/IEC 14882:1998, Section 17.4.1.2, Table 11
 * These are header files for the Standard C++ Library.
 */

// define INCLUDE_ALGORITHM to include "ofalgo.h"
#ifdef INCLUDE_ALGORITHM
#include "ofalgo.h"
#endif

// we don't yet support <bitset>, <complex>, <deque>, <exception>, <functional>

// all stream related headers are handled by "ofstream.h"
#if defined(INCLUDE_IOSFWD) || defined(INCLUDE_IOSTREAM) || defined(INCLUDE_ISTREAM) || \
    defined(INCLUDE_OSTREAM) || defined(INCLUDE_IOMANIP) || defined(INCLUDE_IOS) || \
    defined(INCLUDE_FSTREAM) || defined(INCLUDE_SSTREAM)
#include "ofstream.h"
#endif

// we don't yet support <iterator>, <limits>

// define INCLUDE_LIST to include "oflist.h"
#ifdef INCLUDE_LIST
#include "oflist.h"
#endif

// we don't yet support <locale>, <map>, <memory>, <new>, <numeric>, <queue>, <set>

// define INCLUDE_STACK to include "ofstack.h"
#ifdef INCLUDE_STACK
#include "ofstack.h"
#endif

// we don't yet support <stdexcept>

// define INCLUDE_STREAMBUF to include <streambuf> or <streambuf.h> if available
#ifdef INCLUDE_STREAMBUF
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STREAMBUF_H)
#include <streambuf.h>
#endif
#endif

// define INCLUDE_STRING to include "ofstring.h"
#ifdef INCLUDE_STRING
#include "ofstring.h"
#endif

// we don't yet support <typeinfo>, <valarray>, <vector>


/* Header files as defined in ISO/IEC 14882:1998, Section 17.4.1.2, Table 12
 * These are header files for the Standard C Library.
 */

// define INCLUDE_CASSERT to include <cassert> or <assert.h> if available
#ifdef INCLUDE_CASSERT
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_ASSERT_H)
BEGIN_EXTERN_C
#include <assert.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CCTYPE to include <cctype> or <ctype.h> if available
#ifdef INCLUDE_CCTYPE
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_CTYPE_H)
BEGIN_EXTERN_C
#include <ctype.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CERRNO  to include <cerrno> or <errno.h> if available
#ifdef INCLUDE_CERRNO
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_ERRNO_H)
BEGIN_EXTERN_C
#include <errno.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CFLOAT  to include <cfloat> or <float.h> if available
#ifdef INCLUDE_CFLOAT
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_FLOAT_H)
BEGIN_EXTERN_C
#include <float.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CISO646 to include <ciso646> or <iso646.h> if available
#ifdef INCLUDE_CISO646
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_ISO646_H)
BEGIN_EXTERN_C
#include <iso646.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CLIMITS to include <climits> or <limits.h> if available
#ifdef INCLUDE_CLIMITS
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_LIMITS_H)
BEGIN_EXTERN_C
#include <limits.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CLOCALE to include <clocale> or <locale.h> if available
#ifdef INCLUDE_CLOCALE
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_LOCALE_H)
BEGIN_EXTERN_C
#include <locale.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CMATH to include <cmath> or <math.h> if available
#ifdef INCLUDE_CMATH
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_MATH_H)
#ifndef INCLUDE_MATH_H_AS_CXX
// some systems use C++ language features in <math.h>
BEGIN_EXTERN_C
#endif
#include <math.h>
#ifndef INCLUDE_MATH_H_AS_CXX
END_EXTERN_C
#endif
#endif
#endif

// define INCLUDE_CSETJMP to include <csetjmp> or <setjmp.h> if available
#ifdef INCLUDE_CSETJMP
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_SETJMP_H)
BEGIN_EXTERN_C
#include <setjmp.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSIGNAL to include <csignal> or <signal.h> if available
#ifdef INCLUDE_CSIGNAL
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_SIGNAL_H)
BEGIN_EXTERN_C
#include <signal.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDARG to include <cstdarg> or <stdarg.h> if available
#ifdef INCLUDE_CSTDARG
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STDARG_H)
BEGIN_EXTERN_C
#include <stdarg.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDDEF to include <cstddef> or <stddef.h> if available
#ifdef INCLUDE_CSTDDEF
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STDDEF_H)
BEGIN_EXTERN_C
#include <stddef.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDIO  to include <cstdio> or <stdio.h> if available
#ifdef INCLUDE_CSTDIO
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STDIO_H)
BEGIN_EXTERN_C
#include <stdio.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CSTDLIB to include <cstdlib> or <stdlib.h> if available
#ifdef INCLUDE_CSTDLIB
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STDLIB_H)
/* workaround for bug in Borland C++ Builder 4 */
#ifndef _BCB4
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef _BCB4
END_EXTERN_C
#endif
#endif
#endif

// define INCLUDE_CSTRING to include <cstring> or <string.h> if available
#ifdef INCLUDE_CSTRING
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_STRING_H)
BEGIN_EXTERN_C
#include <string.h>
END_EXTERN_C
#endif
// Some platforms define additional string functions like bzero or
// strcasecmp in <strings.h>, so we always include this file if available.
#ifdef HAVE_STRINGS_H
BEGIN_EXTERN_C
#include <strings.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CTIME to include <ctime> or <time.h> if available
#ifdef INCLUDE_CTIME
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_TIME_H)
BEGIN_EXTERN_C
#include <time.h>
END_EXTERN_C
#endif
#endif

// define INCLUDE_CWCTYPE to include <cwctype> or <wctype.h> if available
#ifdef INCLUDE_CWCTYPE
#ifdef USE_STD_CXX_INCLUDES
#elif defined(HAVE_WCTYPE_H)
BEGIN_EXTERN_C
#include <wctype.h>
END_EXTERN_C
#endif
#endif
