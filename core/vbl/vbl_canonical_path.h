#ifndef vbl_canonical_path_h_
#define vbl_canonical_path_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vbl/vbl_canonical_path.h


//:
// \file
// \brief Contains two functions to compute canonical form of a given path
// \author fsm@robots.ox.ac.uk
//
// \verbatim
// Modifications
// PDA (Manchester) 21/03/2001: Tidied up the documentation
// \endverbatim
//




#include <vcl_string.h>

//: Given a path in the file system, compute a canonical form.
// A canonical path is one which does not
//  - involve any symbolic links
//  - involve any . or ..
//  - begin with a ~
//  - contain any trailing or repeated /
//
// The result of each query is cached so that a subsequent
// query on the same path will be much faster and will not
// involve any system calls.

vcl_string vbl_canonical_path/*cached*/(vcl_string path);

//: Given a path in the file system, compute a canonical form.
// A canonical path is one which does not
//  - involve any symbolic links
//  - involve any . or ..
//  - begin with a ~
//  - contain any trailing or repeated /
//
// The result of each query is not cached.
vcl_string vbl_canonical_path_uncached (vcl_string path);

#endif // vbl_canonical_path_h_
