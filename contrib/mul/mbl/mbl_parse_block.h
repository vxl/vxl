#ifndef mbl_parse_block_h
#define mbl_parse_block_h

//:
// \file
// \author Ian Scott
// \date  25-Feb-2003
// \brief Load a block of text from a file.

#include <iostream>
#include <istream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Read a block of text from a stream.
// This function will read through a stream, and store the text found to a string.
// The function terminates when it finds the closing brace.
//
// The stream's fail bit will be set on error. Comment lines beginning with //
// will be ignored.
// \param open_already should be true if the client has already
// read the opening brace.
// \return the text. The first and last characters are guaranteed to be
// the opening and closing braces.
std::string mbl_parse_block(std::istream &afs, bool open_already = false, const char * comment = "//");
#endif // mbl_parse_block_h
