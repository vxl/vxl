#ifndef vbl_canonical_path_h_
#define vbl_canonical_path_h_
#ifdef __GNUC__
#pragma interface "vbl_canonical_path"
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Given a path in the file system, compute a canonical form.
// A canonical path is one which does not
//  - involve any symbolic links
//  - involve any . or ..
//  - begin with a ~
//  - contain any trailing or repeated /
//
// The result of each query is cached so that a subsequent
// query on the same path will be much faster and will not
// involve any system calls.

#include <vcl/vcl_string.h>

vcl_string vbl_canonical_path/*cached*/(vcl_string path);
vcl_string vbl_canonical_path_uncached (vcl_string path);

#endif
