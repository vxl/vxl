#ifndef mbl_read_multi_props_h
#define mbl_read_multi_props_h
//:
// \file
// \author Ian Scott
// \date 21-Nov-2005
// \brief Load properties with repeated labels from text files

#include <map>
#include <iostream>
#include <iosfwd>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The type of the property dictionary
class mbl_read_multi_props_type : public std::multimap<std::string, std::string>
{
 public:

  //: Return a single expected value of the given property \a label.
  // The matching entry is removed from the property list.
  // \throws mbl_exception_missing_property if \a label doesn't exist.
  // \throws mbl_exception_read_props_parse_error if there are two or more values of \a label.
  std::string get_required_property(const std::string& label);

  //: Return a single value of the given property \a label.
  // The matching entry is removed from the property list.
  // returns empty string or \a default_prop if \a label doesn't exist.
  // \throws mbl_exception_read_props_parse_error if there are two or more values of \a label.
  std::string get_optional_property(const std::string& label,
                                   const std::string& default_prop="");

  //: Return a vector of all values for a given property label \a label.
  // All entries of \a label are removed from the property list.
  // \throws mbl_exception_missing_property if \a label doesn't occur at least once.
  // \param nmax The maximum number of permitted occurrences of this label (default=max<unsigned>).
  // \param nmin The minimum number of permitted occurrences of this label (default=1).
  // \throws mbl_exception_read_props_parse_error if \a label occurs an invalid number of times.
  void get_required_properties(const std::string& label,
                               std::vector<std::string>& values,
                               const unsigned nmax= (unsigned)(-1), //=max<unsigned>
                               const unsigned nmin=1);

  //: Return a vector of all values for a given property label \a label.
  // All entries of \a label are removed from the property list.
  // Returns an empty vector if \a label doesn't occur at least once.
  // \param nmax The maximum number of permitted occurrences of this label (default=max<unsigned>).
  // \throws mbl_exception_read_props_parse_error if \a label occurs too many times.
  void get_optional_properties(const std::string& label,
                               std::vector<std::string>& values,
                               const unsigned nmax= (unsigned)(-1)); //=max<unsigned>
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
mbl_read_multi_props_type mbl_read_multi_props_ws(std::istream &afs);


//: Print a list of properties for debugging purposes.
void mbl_read_multi_props_print(std::ostream &afs, mbl_read_multi_props_type props);

//: Print a list of properties for debugging purposes. Limit each property value length to \p max_chars
// Useful for preventing diagnostic output from being flooded by large properties.
void mbl_read_multi_props_print(std::ostream &afs, mbl_read_multi_props_type props, unsigned max_chars);

//: Throw error if there are any keys in props that aren't in ignore.
// \throw mbl_exception_unused_props
void mbl_read_multi_props_look_for_unused_props(
  const std::string & function_name,
  const mbl_read_multi_props_type &props,
  const mbl_read_multi_props_type &ignore);

//: Throw error if there are any keys in props.
// \throw mbl_exception_unused_props
inline void mbl_read_multi_props_look_for_unused_props(
  const std::string & function_name,
  const mbl_read_multi_props_type &props)
{
  mbl_read_multi_props_look_for_unused_props(function_name, props,
                                             mbl_read_multi_props_type());
}


#endif // mbl_read_multi_props_h
