#ifndef mbl_read_props_h
#define mbl_read_props_h

//:
// \file
// \author Ian Scott
// \date 14-Aug-2001
// \brief Load properties from text files

#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_string.h>

//: The type of the property dictionary
typedef vcl_map<vcl_string, vcl_string > mbl_read_props_type;

//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// There should be one property per line, with a colon ':' after
// each property label. Each property label should not contain
// any whitespace.
mbl_read_props_type mbl_read_props(vcl_istream &afs);


//: Print a list of properties for debugging purposes.
void mbl_read_props_print(vcl_ostream &afs, mbl_read_props_type props);

#endif // mbl_read_props_h
