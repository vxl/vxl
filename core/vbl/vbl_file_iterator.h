//-*- c++ -*-------------------------------------------------------------------
#ifndef vbl_file_iterator_h_
#define vbl_file_iterator_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 27 Nov 00

#include <vcl_string.h>

struct vbl_file_iterator_data;

//: Iterate through directories and/or "glob" patterns (*.*)
// It is efficient to use 
//   for(vbl_file_iterator fn("/dir/*"); fn; ++fn) {
//     ... use fn() as filename
//   }
// simply to list the contents of a directory.  If you really
// want just the *.ext files, it is efficient to use
//   for(vbl_file_iterator fn("/dir/*.ext"); fn; ++fn) {
//     ... use fn() as filename
//   }
// rather than opendir/glob/etc.

// Valid glob patterns are unix-like.

class vbl_file_iterator {
public:

  vbl_file_iterator();

  //: Initialize, and scan to get first file from "glob"
  vbl_file_iterator(char const* glob);

  //: Initialize, and scan to get first file from "glob"
  vbl_file_iterator(vcl_string const& glob);

  ~vbl_file_iterator();

  //: Ask if done.
  // Won't spin the disk
  operator bool(); 

  //: Return the currently pointed-to pathname.
  // Won't spin the disk
  char const* operator()();

  //: Return the non-directory part of the current pathname.
  char const* filename();

  //: Return the match for the i'th glob wildcard character (* or ?).
  // Uses the most recent glob result.
  char const* match(int i);

  //: Increment to the next file
  // Will spin the disk
  vbl_file_iterator& operator++();
  
  //: Run a new match
  void reset(char const* glob);

protected:
  vbl_file_iterator_data* p;

private:
  // postfix++ privatized.
  vbl_file_iterator operator++(int) { return vbl_file_iterator(); }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vbl_file_iterator.

