#ifndef mbl_read_props_h
#define mbl_read_props_h
//:
// \file
// \author Ian Scott
// \date 14-Aug-2001
// \brief Load properties from text files

#include <vcl_map.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>

//: The type of the property dictionary
class mbl_read_props_type : public vcl_map<vcl_string, vcl_string >
{
};

//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// There should be one property per line, with a colon ':' after
// each property label. Each property label should not contain
// any whitespace.  However, property values may contain white space
// they are assumed to be terminated by an end-of-line.
//
// Differs from mbl_read_props_ws(afs) in that it treats everything
// after the label up to the end-of-line as the value.
// Thus it treats the single line "a: a b: b" as
// containing a single label ("a:") with value "a b: b".
// If you want such a line treated as two label-values, use mbl_read_props_ws()
mbl_read_props_type mbl_read_props(vcl_istream &afs);

//: Read properties from a text stream.
// The function will terminate on an eof. If one of
// the opening lines contains an opening brace '{', then the function
// will also stop reading the stream after finding a line containing
// a closing brace '}'
//
// Every property label ends in ":", and should not contain
// any whitespace.
// If there is a brace after the first string following the label,
// the following text up to matching
// braces is included in the property value.

// Differs from mbl_read_props(afs) in that all whitespace is treated
// as a separator.  Thus it treats the single line "a: a b: b" as
// containing two separate label-value pairs.
mbl_read_props_type mbl_read_props_ws(vcl_istream &afs);


//: Print a list of properties for debugging purposes.
void mbl_read_props_print(vcl_ostream &afs, mbl_read_props_type props);

//: merge two property sets.
// \param first_overrides
// properties in "first" will override identically named properties in "second"
mbl_read_props_type mbl_read_props_merge(const mbl_read_props_type& first,
                                         const mbl_read_props_type& second,
                                         bool first_overrides=true);

#endif // mbl_read_props_h
