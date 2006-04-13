#ifndef mbl_read_multi_props_h
#define mbl_read_multi_props_h
//:
// \file
// \author Ian Scott
// \date 21-Nov-2005
// \brief Load properties with repeated labels from text files

#include <vcl_map.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>
#include <vcl_vector.h>


//: The type of the property dictionary
class mbl_read_multi_props_type : public vcl_multimap<vcl_string, vcl_string >
{
public:

  //: Return a vector of all values for a given property label \a label.
  // All entries of \a label are removed from the property list.
  // \throws mbl_exception_missing_property if \a label doesn't occur 
  // at least once.
  // \sa get_optional_property().
  void get_required_property(const vcl_string& label,
                             vcl_vector<vcl_string>& values,
                             const unsigned nmax=1000000,
                             const unsigned nmin=1);

  //: Return a vector of all values for a given property label \a label.
  // All entries of \a label are removed from the property list.
  // Returns an empty vector if \a label doesn't occur at least once.
  // \sa get_required_property().
  void get_optional_property(const vcl_string& label,
                             vcl_vector<vcl_string>& values,
                             const unsigned nmax=1000000,
                             const unsigned nmin=0);
};


//: Read properties with repeated labels from a text stream.
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
//
// Differs from mbl_read_props_ws(afs) in that a property label can be repeated,
// and the all the repeated values will be returned.
mbl_read_multi_props_type mbl_read_multi_props_ws(vcl_istream &afs);


//: Print a list of properties for debugging purposes.
void mbl_read_multi_props_print(vcl_ostream &afs, mbl_read_multi_props_type props);


//: Throw error if there are any keys in props that aren't in ignore.
// \throw mbl_exception_unused_props
void mbl_read_multi_props_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_multi_props_type &props,
  const mbl_read_multi_props_type &ignore);

//: Throw error if there are any keys in props.
// \throw mbl_exception_unused_props
inline void mbl_read_multi_props_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_multi_props_type &props)
{
  mbl_read_multi_props_look_for_unused_props(function_name, props,
    mbl_read_multi_props_type());
}


#endif // mbl_read_multi_props_h
