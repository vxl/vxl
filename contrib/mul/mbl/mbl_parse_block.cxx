
//:
// \file
// \author Ian Scott
// \date  25-Feb-2003
// \brief Load a block of text from a file.

#include "mbl_parse_block.h"
#include <vcl_cctype.h>
#include <vcl_cstring.h>
//: Read a block of text from a stream.
// This function will read through a stream, and store the text found to a string.
// The function terminates when it finds the closing brace.
//
// The stream's fail bit will be set on error. Comment lines beginning with //
// will be stripped.
// \param open_already should be true if the client has already
// read the opening brace.
// \return the text including the opening and closing braces.
// \param comment Lines begining with white space followed by this string will be ignored.
// Set to empty for no comment stripping.
vcl_string mbl_parse_block(vcl_istream &afs, bool open_already /*= false*/, const char * comment /*= "//"*/)
{
  if (!afs) return "{}";
  //: The last character to br read from the stream
  char c;
  if (!open_already)
  {
    afs >> c;
    if (c != '{')
    {
      vcl_cerr << " WARNING: mbl_parse_block()\n" <<
        "First non-ws char is '" << c <<"'. Should be '{'\n" << vcl_endl;
      afs.clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
      return "{}";
    }
  }
  //: The stored string.
  vcl_string s="{";
  //: The current line is stored separately
  // before being added to s, in case it turns out to be a comment.
  vcl_string s2="";
  //: The number of open braces.
  unsigned level=1;
  //: The current position in a comment token.
  unsigned comment_position;
  //: length of the comment token;
  const unsigned comment_length = vcl_strlen(comment);
  //: true if we are currently in the whitespace at the beggining of a line
  bool newline=true;

  while (!(!afs))
  {
    // read in one line at a time, to make decisions about comments
    while (!(!afs))
    {
      afs.get(c);
      s2 += c;
      if (c=='\n')
      {
        s+=s2;
        s2="";
        newline = true;
        comment_position = 0;
      }
      else if (vcl_isspace(c))
        comment_position = 0;
      else if (newline && comment_position < comment_length
                &&  c==comment[comment_position])
      {
        if(++comment_position == 2)
        {
          vcl_string dummy;
          vcl_getline(afs, dummy);
          s2 = "";
          newline = false;
          comment_position = 0;
        }
      }
      else
      {
        newline = false;
        comment_position = 0;
        if (c=='{') ++level;
        else if (c=='}')
          if (--level==0) return s+s2;
      }
    }

  }
  vcl_cerr << " WARNING: mbl_parse_block()\n" <<
    "Read problem (possibly end-of-file) before closing '}'\n" <<
    "Text parsed so far:\n" << s << vcl_endl;
  afs.clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
  return "{}";

}
