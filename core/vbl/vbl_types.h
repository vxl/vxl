#ifndef vbl_types_h
#define vbl_types_h

// .HEADER	Basics Package
// .LIBRARY	vbl
// .INCLUDE	vbl/vbl_types.h
// .FILE	vbl/vbl_types.cxx
//
// .SECTION Description
//     This file defines basic vbl_types used by the libraries:
//     uchar, ushort, uint, ulong, byte, doublebyte, quadbyte,
//     boolean, true, false, u_char, nil, os_boolean, os_bool,
//     PI, MAXCHAR.

#include <math.h>

/* ------------------------------------ */
/*      DEFINE BASIC TYPES              */
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

/* ------------------------------------ */
/*   DEFINE MACHINE DEPENDENT TYPES     */
#ifndef _byte_h
typedef unsigned char  byte;
#endif
typedef unsigned short doublebyte;
typedef unsigned int   quadbyte;
typedef unsigned long  octbyte; // esp. needed for 64-bit compilers!!

/* ------------------------------------ */
/*       DEFINE A BOOLEAN TYPE          */
/*					*/
/*					*/
/* ------------------------------------ */
// make sure this works with InterViews
#ifndef boolean
#define boolean osboolean
#endif

#ifdef WIN32
typedef unsigned char boolean;
#else
typedef unsigned boolean;
#endif

// ***** WARNING!!! The following is a kludge to get around *****
// ***** a conflict between ObjectStore and InterViews      *****
#ifdef os_boolean                       // this if..else..endif is also
#undef os_boolean                       // an attempt to prevent conflict
typedef int os_boolean;                 // between ObjectStore and InterViews
typedef os_boolean os_bool;             // as *both* of them use the name
#define os_boolean os_bool              // os_boolean (ObjectStore in a 
#else                                   // typedef, InterViews in a #define)
typedef int os_boolean;                 // If os_boolean in ObjectStore
typedef os_boolean os_bool;             // changes in a future release, this
#endif                                  // will need to be updated accordingly

/* ------------------------------------ */
/*       DEFINE A COORDINATE TYPE       */


/* ------------------------------------ */
/*       DEFINE A STATUS TYPE           */
typedef int StatusCode;

/* ------------------------------------ */
/*       DEFINE A NIL POINTER           */

#ifndef nil
#define nil 0
#endif

/* ------------------------------------ */
/*       DEFINE A CONSTANTS [sic]       */

#ifndef PI
const double PI	     = 3.141592658979324;
#endif
#ifndef MAXCHAR
const int    MAXCHAR = 2048;
#endif

/* ------------------------------------ */
/*       DEFINE CALLBACK STUFF          */

typedef void (* Callback)(void*, void*);
// awf removed, it doesn't occur in free target. #define CALLBACK(func) (Callback) func

class vbl_attribute_value_pair {
public:
    char* label;
    void* value;

  // -- Copies the label, not the value
  vbl_attribute_value_pair* Copy() const;
  // -- No dtor, this is used only if copy was called.
  friend void Delete(vbl_attribute_value_pair* d);

// These constructors make this class unusable as a so-called aggregate. Because
// aggregate assignment is more efficient than going throught constructors, these
// are commented out by default, except if you define NEW_MENUS_.
#ifdef NEW_MENUS_
    vbl_attribute_value_pair(void* l, void* v) : label((char *)l), value(v) {}
    vbl_attribute_value_pair(char* l, void* v) : label(l), value(v) {}
    vbl_attribute_value_pair(char* l, int v) : label(l), value((void*)v) {}
    vbl_attribute_value_pair() : label(0), value(0) {}
#endif
#ifdef __GNUC__
    static vbl_attribute_value_pair null;
#else
    static void* null;
#endif
};

typedef vbl_attribute_value_pair UIChoice;

// New style macros -- USE THESE IF YOU ARE WRITING NEW CODE.
#define UIChoiceCopy(A)   {A.label,A.value}
#define UIChoicePair(L,V) {(char *)(L),(void*)(V)}
#define UIChoice_finish   {(char *)0, (void*)0}

// Old style Interview choices.  DEPRECIATED.
#ifndef NEW_MENUS_
#define UIChoice1(A)   {A.label,A.value}
#define UIChoice2(L,V) {(char *)(L),(void*)(V)}
#define UIChoice_end   UIChoice2(0,0)
#else
#define UIChoice1(A)    A
#undef UIChoiceCopy
#define UIChoiceCopy(A) A
#define UIChoice2       UIChoice
#undef UIChoicePair
#define UIChoicePair    UIChoice
#define UIChoice_end    UIChoice::null
#endif

#endif // vbl_types_h
