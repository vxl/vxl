#ifndef mbl_read_props_h
#define mbl_read_props_h
//:
// \file
// \author Ian Scott
// \date 14-Aug-2001
// \brief Load properties from text files
// Along with mbl_parse_block, the functions in this file allow you to quickly
// parse hiearchical property files that look like:
// \verbatim
// {
//   n_disp_dx: 3
//   n_synth_targets: 30
//   parameter_displacements:
//   {
//    0 1 2 3
//   }
//   rfunc: m32_simple_residual_function
//   {
//     model:
//     {
//       apm_filepath: ../hip_cropped_lres.apm
//       background_filepath: gen:100x50x80:vxl_int_32:0
//       foobar_modifier: foobar_strategy3
//     }
//     comparator:
//     {
//       type: comparator_strategy2
//       initial_geometry:
//       {
//         centre_of_source: 0 -1000 0
//         centre_of_image_plane: 0 300 0
//       }
//     }
//     comparator:
//     {
//       type: comparator_strategy2
//       initial_geometry:
//       {
//         centre_of_source: 860.365 -1228.73 0
//         centre_of_image_plane: -172.073 245.746 0
//       }
//     }
//   }
// }
// \endverbatim


#include <vcl_map.h>
#include <vcl_iosfwd.h>
#include <vcl_string.h>

//: The type of the property dictionary
class mbl_read_props_type : public vcl_map<vcl_string, vcl_string >
{
public:
  // Return the contents for a given property prop.
  // prop is removed from the property list.
  // \throws mbl_exception_missing_property if prop doesn't exist
  vcl_string get_required_property(const vcl_string &prop);
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
mbl_read_props_type mbl_read_props_merge(
  const mbl_read_props_type& first,
  const mbl_read_props_type& second,
  bool first_overrides=true);


//: Throw error if there are any keys in props that aren't in ignore.
// \throw mbl_exception_unused_props
void mbl_read_props_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_props_type &props,
  const mbl_read_props_type &ignore);

//: Throw error if there are any keys in props.
// \throw mbl_exception_unused_props
inline void mbl_read_props_look_for_unused_props(
  const vcl_string & function_name,
  const mbl_read_props_type &props)
{
  mbl_read_props_look_for_unused_props(function_name, props,
    mbl_read_props_type());
}


#endif // mbl_read_props_h
