// This is core/vul/vul_file_iterator.h
#ifndef vul_file_iterator_h_
#define vul_file_iterator_h_
//:
// \file
// \brief class to iterate through directories and/or "glob" patterns (*.*)
// \author awf@robots.ox.ac.uk
// \date 27 Nov 00
//
// \verbatim
//  Modifications
//   PDA (Manchester) 21/03/2001: Tidied up the documentation
//   Peter Vanroose   27/05/2001: Corrected the documentation
//   Ian Scott        12/06/2003: Added filen?m[abc].* notation to unix and dos version
// \endverbatim

#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

struct vul_file_iterator_data;

//: Iterate through directories and/or "glob" patterns (*.*)
// It is efficient to use
// \code
//   for (vul_file_iterator fn="/dir/*"; fn; ++fn) {
//     ... use fn() as filename
//   }
// \endcode
// simply to list the contents of a directory.  If you really
// want just the *.ext files, it is efficient to use
// \code
//   for (vul_file_iterator fn="/dir/*.ext"; fn; ++fn) {
//     ... use fn() as filename
//   }
// \endcode
// rather than opendir/glob/etc.
//
// Valid glob patterns are unix-like - '?' matches precisely one character
// '*' matches any sequence (including empty), [abc] matches either 'a' or 'b' or 'c'
//
// \note There is no implicit ordering of the files in a directory;
// the order is OS-dependent and is not guaranteed by this class.
// You may wish, therefore, to store the filenames returned and sort them yourself,
// if you want to process files in (for example) alphanumeric order.
class vul_file_iterator
{

 public:

  vul_file_iterator() : p(nullptr) {}

  //: Initialize, and scan to get first file from "glob"
  vul_file_iterator(char const* glob);

  //: Initialize, and scan to get first file from "glob"
  vul_file_iterator(std::string const& glob);

  ~vul_file_iterator();

  //: Ask if done.
  // Won't spin the disk
  explicit operator bool() const;

  //: Inverse boolean value
  bool operator!() const;

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
  vul_file_iterator& operator++();

  //: Run a new match
  void reset(char const* glob);

 protected:
  vul_file_iterator_data* p;

 private:
  // postfix++ privatized.
  vul_file_iterator operator++(int) { return vul_file_iterator(); }
};

#endif // vul_file_iterator_h_
