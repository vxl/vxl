#ifndef mbl_parse_block_h
#define mbl_parse_block_h

//:
// \file
// \author Ian Scott
// \date  25-Feb-2003
// \brief Load a block of text from a file.

#include <vcl_istream.h>
#include <vcl_string.h>

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
vcl_string mbl_parse_block(vcl_istream &afs, bool open_already = false, const char * comment = "//");
#endif // mbl_parse_block_h
