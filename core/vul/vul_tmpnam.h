#ifndef vul_tmpnam_h_
#define vul_tmpnam_h_
#ifdef __GNUC__
#pragma interface
#endif

// \file
// \author Amitha Perera <perera@cs.rpi.edu>
// \brief  Implements something similar to POSIX tmpnam.
// This is to avoid (GNU) linker warnings (and errors!) about calls to
// tmpnam being unsafe.

class vcl_string;

//: Generates a temporary filename.
// \param filename is a pathname with a set of trailing X's. (For
// example, "/tmp/fileXXXX".) If the return value is true, each X in
// \param filename will be replaced with a random character such that
// the new filename can be used as a temporary file. The return value
// will be false if the generated filename is not valid (could not be
// opened for writing).
//
// There is a possible race condition, in that some other process may
// open a the temporary file between the time tmpnam creates (and
// tests) it, and the time the calling program opens it.
bool tmpnam( vcl_string& filename );

#endif
